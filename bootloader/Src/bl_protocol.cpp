/*
 * Hexadeck Bootloader - MIDI SysEx Firmware Update Protocol Implementation
 *
 * Receives firmware data via USB MIDI SysEx and writes to flash.
 * Uses 7-bit encoding for MIDI-safe binary transfer.
 */

#include "bl_protocol.h"
#include "bl_flash.h"
#include "main.h"
#include "usbd_midi_if.h"
#include <string.h>

/* MIDI_IDLE enum value from usbd_midi.h (avoid including it due to
   missing extern "C" guards causing linkage mismatch) */
#define MIDI_IDLE 0

/* ---- SysEx reassembly buffer ---- */
#define SYSEX_BUF_SIZE  512
static uint8_t sysex_buf[SYSEX_BUF_SIZE];
static uint16_t sysex_len = 0;
static volatile bool sysex_ready = false;
static bool sysex_receiving = false;

/* ---- Protocol state ---- */
typedef enum {
    BL_STATE_IDLE,
    BL_STATE_READY,      /* Flash erased, awaiting data */
    BL_STATE_RECEIVING,  /* Receiving firmware chunks */
    BL_STATE_DONE,       /* Firmware received, ready to verify/boot */
} bl_state_t;

static bl_state_t bl_state = BL_STATE_IDLE;

/* ---- 7-bit decode ---- */
/* Decodes MIDI 7-bit encoded data back to raw bytes.
 * For every 8 encoded bytes → 7 raw bytes.
 * encoded[0] = MSB flags, encoded[1..7] = lower 7 bits. */
static uint32_t decode_7bit(const uint8_t *encoded, uint32_t enc_len, uint8_t *decoded)
{
    uint32_t dec_len = 0;
    uint32_t i = 0;

    while (i < enc_len) {
        uint8_t msb_flags = encoded[i++];
        for (int j = 0; j < 7 && i < enc_len; j++, i++) {
            uint8_t val = encoded[i] & 0x7F;
            if (msb_flags & (1 << j)) {
                val |= 0x80;
            }
            decoded[dec_len++] = val;
        }
    }

    return dec_len;
}

/* ---- 7-bit encode (for responses) ---- */
static uint32_t encode_7bit(const uint8_t *raw, uint32_t raw_len, uint8_t *encoded)
{
    uint32_t enc_len = 0;
    uint32_t i = 0;

    while (i < raw_len) {
        uint8_t msb_flags = 0;
        uint32_t group_start = enc_len;
        encoded[enc_len++] = 0; /* placeholder for MSB flags */

        for (int j = 0; j < 7 && i < raw_len; j++, i++) {
            if (raw[i] & 0x80) {
                msb_flags |= (1 << j);
            }
            encoded[enc_len++] = raw[i] & 0x7F;
        }

        encoded[group_start] = msb_flags;
    }

    return enc_len;
}

/* ---- Send SysEx response ---- */
static void send_sysex_response(uint8_t cmd, uint8_t status)
{
    /* Build response: F0 7D 60 cmd status F7 */
    uint8_t resp[] = { 0xF0, 0x7D, BL_SYSEX_CATEGORY, cmd, status, 0xF7 };
    uint32_t len = sizeof(resp);
    uint32_t idx = 0;

    while (idx < len) {
        uint8_t usb_pkt[4] = {0};
        uint32_t remaining = len - idx;

        if (remaining >= 3) {
            if (remaining == 3) {
                usb_pkt[0] = 0x07; /* SysEx ends with 3 bytes */
            } else {
                usb_pkt[0] = 0x04; /* SysEx start/continue */
            }
            usb_pkt[1] = resp[idx++];
            usb_pkt[2] = resp[idx++];
            usb_pkt[3] = resp[idx++];
        } else if (remaining == 2) {
            usb_pkt[0] = 0x06; /* SysEx ends with 2 bytes */
            usb_pkt[1] = resp[idx++];
            usb_pkt[2] = resp[idx++];
        } else {
            usb_pkt[0] = 0x05; /* SysEx ends with 1 byte */
            usb_pkt[1] = resp[idx++];
        }

        /* Wait for MIDI TX idle */
        while (MIDI_GetState() != MIDI_IDLE) { }
        MIDI_SendReport(usb_pkt, 4);
    }
}

/* ---- SysEx message handler ---- */
static void handle_sysex(const uint8_t *data, uint16_t len)
{
    /* Minimum: category + command = 2 bytes (after manufacturer ID stripped) */
    if (len < 2) return;

    if (data[0] != BL_SYSEX_CATEGORY) return;

    uint8_t cmd = data[1];

    switch (cmd) {
        case BL_CMD_START_UPDATE: {
            /* Erase application flash */
            int ret = bl_flash_erase_app();
            if (ret == 0) {
                bl_state = BL_STATE_READY;
                send_sysex_response(BL_RSP_ACK, BL_STATUS_OK);
            } else {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
            }
            break;
        }

        case BL_CMD_FW_DATA: {
            if (bl_state != BL_STATE_READY && bl_state != BL_STATE_RECEIVING) {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
                break;
            }

            /* Format: [category] [cmd] [addr_encoded (5 bytes)] [data_encoded ...]
             * Address is 4 raw bytes → 5 encoded bytes (7-bit packed).
             * addr is offset from APP_FLASH_START. */
            if (len < 2 + 5) {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
                break;
            }

            /* Decode address (4 raw bytes from 5 encoded bytes) */
            uint8_t addr_raw[4];
            uint32_t addr_dec_len = decode_7bit(&data[2], 5, addr_raw);
            if (addr_dec_len < 4) {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
                break;
            }

            uint32_t offset = ((uint32_t)addr_raw[0] << 24) |
                              ((uint32_t)addr_raw[1] << 16) |
                              ((uint32_t)addr_raw[2] << 8)  |
                              ((uint32_t)addr_raw[3]);
            uint32_t flash_addr = APP_FLASH_START + offset;

            /* Decode firmware data */
            uint8_t fw_data[BL_MAX_CHUNK_SIZE + 8];
            uint32_t enc_data_len = len - 2 - 5;
            uint32_t fw_len = decode_7bit(&data[2 + 5], enc_data_len, fw_data);

            if (fw_len == 0 || (flash_addr + fw_len) > APP_FLASH_END) {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
                break;
            }

            int ret = bl_flash_write(flash_addr, fw_data, fw_len);
            if (ret == 0) {
                bl_state = BL_STATE_RECEIVING;
                send_sysex_response(BL_RSP_ACK, BL_STATUS_OK);
            } else {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
            }
            break;
        }

        case BL_CMD_FW_VERIFY: {
            /* Format: [category] [cmd] [size_encoded (5 bytes)] [crc_encoded (5 bytes)] */
            if (len < 2 + 5 + 5) {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
                break;
            }

            /* Decode firmware size */
            uint8_t size_raw[4];
            decode_7bit(&data[2], 5, size_raw);
            uint32_t fw_size = ((uint32_t)size_raw[0] << 24) |
                               ((uint32_t)size_raw[1] << 16) |
                               ((uint32_t)size_raw[2] << 8)  |
                               ((uint32_t)size_raw[3]);

            /* Decode expected CRC */
            uint8_t crc_raw[4];
            decode_7bit(&data[2 + 5], 5, crc_raw);
            uint32_t expected_crc = ((uint32_t)crc_raw[0] << 24) |
                                    ((uint32_t)crc_raw[1] << 16) |
                                    ((uint32_t)crc_raw[2] << 8)  |
                                    ((uint32_t)crc_raw[3]);

            if (fw_size > APP_FLASH_SIZE) {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_ERROR);
                break;
            }

            uint32_t actual_crc = bl_flash_crc32(APP_FLASH_START, fw_size);
            if (actual_crc == expected_crc) {
                bl_state = BL_STATE_DONE;
                send_sysex_response(BL_RSP_ACK, BL_STATUS_OK);
            } else {
                send_sysex_response(BL_RSP_ACK, BL_STATUS_CRC_FAIL);
            }
            break;
        }

        case BL_CMD_REBOOT: {
            send_sysex_response(BL_RSP_ACK, BL_STATUS_OK);
            /* Small delay for USB response to be sent */
            HAL_Delay(100);
            NVIC_SystemReset();
            break;
        }

        default:
            break;
    }
}

/* ---- USB MIDI Data Callback (called from USB interrupt context) ---- */
/* Parse incoming USB MIDI packets and reassemble SysEx messages.
 * Must be extern "C" to override the __weak definition in usbd_midi.c */
extern "C" void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length)
{
    if (!usb_rx_buffer_length) return;

    /* Process USB MIDI packets (4 bytes each) */
    for (uint8_t i = 0; i + 3 < usb_rx_buffer_length; i += 4) {
        uint8_t cin = usb_rx_buffer[i] & 0x0F;
        int data_bytes = 0;
        bool sysex_end = false;

        switch (cin) {
            case 0x04: data_bytes = 3; break;              /* SysEx start/continue */
            case 0x05: data_bytes = 1; sysex_end = true; break; /* SysEx ends with 1 byte */
            case 0x06: data_bytes = 2; sysex_end = true; break; /* SysEx ends with 2 bytes */
            case 0x07: data_bytes = 3; sysex_end = true; break; /* SysEx ends with 3 bytes */
            default: continue; /* Not a SysEx packet, ignore */
        }

        for (int j = 1; j <= data_bytes; j++) {
            uint8_t val = usb_rx_buffer[i + j];

            /* Skip SysEx start byte (0xF0) */
            if (val == 0xF0) {
                sysex_len = 0;
                sysex_receiving = true;
                continue;
            }

            /* Skip manufacturer ID 0x7D (first byte after F0) */
            if (sysex_receiving && sysex_len == 0 && val == 0x7D) {
                continue;
            }

            /* Skip SysEx end byte (0xF7) */
            if (val == 0xF7) {
                continue;
            }

            /* Store data byte */
            if (sysex_receiving && sysex_len < SYSEX_BUF_SIZE) {
                sysex_buf[sysex_len++] = val;
            }
        }

        if (sysex_end && sysex_receiving) {
            sysex_receiving = false;
            sysex_ready = true;
        }
    }
}

/* ---- Public API ---- */

void bl_protocol_init(void)
{
    bl_state = BL_STATE_IDLE;
    sysex_len = 0;
    sysex_ready = false;
    sysex_receiving = false;
}

void bl_protocol_process(void)
{
    if (sysex_ready) {
        sysex_ready = false;
        handle_sysex(sysex_buf, sysex_len);
        sysex_len = 0;
    }
}
