/*
 * Hexadeck Bootloader - MIDI SysEx Firmware Update Protocol
 *
 * SysEx Protocol (all messages use Manufacturer ID 0x7D):
 *
 * Host → Bootloader:
 *   CMD_START_UPDATE (0x60 0x01)      : Erase app flash, prepare for update
 *   CMD_FW_DATA     (0x60 0x02 ...)   : Write firmware chunk
 *   CMD_FW_VERIFY   (0x60 0x03 ...)   : Verify firmware CRC
 *   CMD_REBOOT      (0x60 0x04)       : Reboot into application
 *
 * Bootloader → Host:
 *   RSP_ACK         (0x60 0x10 status): Acknowledge with status
 *
 * Data encoding (7-bit MIDI safe):
 *   Raw bytes are encoded in groups of 7 → 8 bytes:
 *   [MSB_flags] [b0 & 0x7F] [b1 & 0x7F] ... [b6 & 0x7F]
 *   MSB_flags bit N = MSB of byte N in the group
 */

#ifndef BL_PROTOCOL_H
#define BL_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SysEx command bytes (after manufacturer ID 0x7D) */
#define BL_SYSEX_CATEGORY       0x60

#define BL_CMD_START_UPDATE     0x01
#define BL_CMD_FW_DATA          0x02
#define BL_CMD_FW_VERIFY        0x03
#define BL_CMD_REBOOT           0x04

#define BL_RSP_ACK              0x10

/* ACK status codes */
#define BL_STATUS_OK            0x00
#define BL_STATUS_ERROR         0x01
#define BL_STATUS_CRC_FAIL      0x02
#define BL_STATUS_BUSY          0x03
#define BL_STATUS_READY         0x04

/* Max firmware chunk size (raw bytes, before encoding) */
#define BL_MAX_CHUNK_SIZE       128

void bl_protocol_init(void);
void bl_protocol_process(void);

#ifdef __cplusplus
}
#endif

#endif /* BL_PROTOCOL_H */
