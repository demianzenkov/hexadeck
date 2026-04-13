# Hexadeck Custom Bootloader

USB MIDI bootloader for the Hexadeck controller (STM32F446VET6).

## Memory Layout

| Region | Address | Size | Flash Sectors |
|--------|---------|------|---------------|
| Bootloader | `0x08000000` | 48 KB | 0-2 (3 × 16 KB) |
| NVS Data | `0x0800C000` | 16 KB | 3 |
| Application | `0x08010000` | 448 KB | 4-7 (64 + 3 × 128 KB) |

## Boot Flow

1. MCU resets → bootloader starts at `0x08000000`
2. Check RTC backup register for update magic (`0xB00710AD`)
3. If magic found → clear register, init USB MIDI, enter **update mode**
4. If no magic → check if valid application exists at `0x08010000`
5. If valid app → jump to application
6. If no valid app → enter **update mode** (fallback)

## Firmware Update Protocol (MIDI SysEx)

All SysEx messages use non-commercial manufacturer ID `0x7D` and category byte `0x60`.

### Commands (Host → Bootloader)

| Command | Bytes | Description |
|---------|-------|-------------|
| START_UPDATE | `F0 7D 60 01 F7` | Erase app sectors, prepare for update |
| FW_DATA | `F0 7D 60 02 <addr> <data> F7` | Write firmware chunk at offset |
| FW_VERIFY | `F0 7D 60 03 <size> <crc32> F7` | Verify CRC32 of written firmware |
| REBOOT | `F0 7D 60 04 F7` | Reboot into application |

### Response (Bootloader → Host)

| Response | Bytes | Description |
|----------|-------|-------------|
| ACK | `F0 7D 60 10 <status> F7` | Acknowledge: `00`=OK, `01`=Error, `02`=CRC fail |

### 7-bit Data Encoding

Binary data is encoded for MIDI (bytes must be 0x00-0x7F):
- Group 7 raw bytes → 8 encoded bytes
- Byte 0: MSB flags (bit N = MSB of raw byte N)
- Bytes 1-7: raw bytes with MSB cleared

## Build

```bash
# Debug build
make DEBUG=1

# Release build
make DEBUG=0

# Flash bootloader to device (via ST-Link)
make flash
```

## Triggering Update from Firmware

The main firmware calls `JumpToBootloader()` which writes the magic value to `RTC->BKP0R` and resets the MCU. The bootloader detects this on startup and enters update mode.

This is triggered via MIDI SysEx command `MIDI_SYS_FIRMWARE_UPDATE` from the webapp or configurator tools.

## Python Firmware Updater

```bash
cd ../scripts/firmware_updater
pip install -r requirements.txt
python midi_fw_update.py ../firmware/build/release/hexadeck-release.bin
```
