#!/usr/bin/env python3
"""
Hexadeck MIDI Firmware Updater

Sends firmware binary to the Hexadeck bootloader via USB MIDI SysEx.

Protocol:
  1. Send BL_CMD_START_UPDATE → bootloader erases app flash
  2. Send BL_CMD_FW_DATA chunks → bootloader writes to flash
  3. Send BL_CMD_FW_VERIFY with size + CRC → bootloader verifies
  4. Send BL_CMD_REBOOT → bootloader jumps to new firmware

Usage:
  python midi_fw_update.py <firmware.bin> [--port NAME]

Requirements:
  pip install mido python-rtmidi
"""

import sys
import time
import struct
import argparse
import binascii

try:
    import mido
except ImportError:
    print("Error: mido is required. Install with: pip install mido python-rtmidi")
    sys.exit(1)

# Protocol constants
# The bootloader's SysEx parser strips F0, manufacturer ID 0x7D, and F7.
# So bootloader protocol messages are sent as: F0 7D <category> <cmd> ... F7
# The firmware's SysEx handler does NOT strip manufacturer ID — it reads
# buffer[0] directly as the command byte. So firmware messages are: F0 <cmd> ... F7
MANUFACTURER_ID = 0x7D
BL_SYSEX_CATEGORY = 0x60
BL_CMD_START_UPDATE = 0x01
BL_CMD_FW_DATA = 0x02
BL_CMD_FW_VERIFY = 0x03
BL_CMD_REBOOT = 0x04
BL_RSP_ACK = 0x10
BL_STATUS_OK = 0x00

# Firmware SysEx command to enter custom bootloader (sent to running application)
MIDI_SYS_CUSTOM_BOOTLOADER = 41  # enum value in task_midi.h

# Firmware constraints
APP_FLASH_START = 0x08010000
APP_FLASH_SIZE = 448 * 1024  # 448KB max
CHUNK_SIZE = 128  # Raw bytes per SysEx message

ACK_TIMEOUT = 10.0  # Seconds to wait for ACK


def encode_7bit(data: bytes) -> bytes:
    """Encode raw bytes to MIDI 7-bit safe format.

    For every 7 bytes of input, outputs 8 bytes:
      byte[0] = MSB flags (bit N = MSB of input byte N)
      bytes[1..7] = input bytes with MSB stripped
    """
    encoded = bytearray()
    i = 0
    while i < len(data):
        group = data[i:i + 7]
        msb_flags = 0
        for j, b in enumerate(group):
            if b & 0x80:
                msb_flags |= (1 << j)
        encoded.append(msb_flags)
        for b in group:
            encoded.append(b & 0x7F)
        i += 7
    return bytes(encoded)


def decode_7bit(data: bytes) -> bytes:
    """Decode MIDI 7-bit encoded data back to raw bytes."""
    decoded = bytearray()
    i = 0
    while i < len(data):
        msb_flags = data[i]
        i += 1
        for j in range(7):
            if i >= len(data):
                break
            val = data[i] & 0x7F
            if msb_flags & (1 << j):
                val |= 0x80
            decoded.append(val)
            i += 1
    return bytes(decoded)


def crc32(data: bytes) -> int:
    """Calculate CRC32 matching the bootloader implementation."""
    return binascii.crc32(data) & 0xFFFFFFFF


class MidiFirmwareUpdater:
    def __init__(self, port_name: str = None):
        self._port_name = port_name
        self.outport = None
        self.inport = None
        self.ack_received = False
        self.ack_status = None
        self._open_ports(port_name)

    def _find_port(self, ports: list, port_name: str = None) -> str:
        """Find a MIDI port by name (partial match) or auto-detect Hexadeck."""
        if port_name:
            for name in ports:
                if port_name.lower() in name.lower():
                    return name
            raise RuntimeError(f"MIDI port '{port_name}' not found. Available: {ports}")

        # Auto-detect
        for name in ports:
            if "hexadeck" in name.lower():
                return name
        for name in ports:
            if "midi" in name.lower():
                return name

        if ports:
            print(f"Available MIDI ports: {ports}")
            print(f"Using first port: {ports[0]}")
            return ports[0]

        raise RuntimeError("No MIDI ports found")

    def _open_ports(self, port_name: str):
        """Open MIDI input and output ports."""
        out_ports = mido.get_output_names()
        in_ports = mido.get_input_names()

        out_name = self._find_port(out_ports, port_name)
        in_name = self._find_port(in_ports, port_name)

        self.outport = mido.open_output(out_name)
        self.inport = mido.open_input(in_name, callback=self._midi_callback)

        print(f"MIDI OUT: {out_name}")
        print(f"MIDI IN:  {in_name}")

    def _midi_callback(self, message):
        """Handle incoming MIDI messages (ACK responses)."""
        if message.type != 'sysex':
            return
        data = message.data
        # Bootloader ACK: F0 7D 60 10 <status> F7
        # mido strips F0/F7, so data = [7D, 60, 10, status]
        if (len(data) >= 4 and
                data[0] == MANUFACTURER_ID and
                data[1] == BL_SYSEX_CATEGORY and
                data[2] == BL_RSP_ACK):
            self.ack_status = data[3]
            self.ack_received = True

    def _wait_for_ack(self, timeout: float = ACK_TIMEOUT) -> int:
        """Wait for ACK response from bootloader. Returns status code."""
        self.ack_received = False
        self.ack_status = None

        start = time.time()
        while not self.ack_received:
            if time.time() - start > timeout:
                raise TimeoutError("Timeout waiting for bootloader ACK")
            time.sleep(0.01)

        return self.ack_status

    def _send_bl_sysex(self, payload: list):
        """Send a SysEx message to the bootloader (includes manufacturer ID 0x7D).

        The bootloader's parser strips 0x7D, so payload should start with
        BL_SYSEX_CATEGORY followed by the command and data."""
        msg = mido.Message('sysex', data=[MANUFACTURER_ID] + payload)
        self.outport.send(msg)

    def _send_fw_sysex(self, payload: list):
        """Send a SysEx message to the running firmware (no manufacturer ID).

        The firmware's SysEx handler reads buffer[0] as the command byte directly."""
        msg = mido.Message('sysex', data=payload)
        self.outport.send(msg)

    def send_start_update(self):
        """Send START_UPDATE command to erase app flash."""
        print("Erasing application flash...")
        self._send_bl_sysex([BL_SYSEX_CATEGORY, BL_CMD_START_UPDATE])
        status = self._wait_for_ack(timeout=30.0)  # Erase can take a while
        if status != BL_STATUS_OK:
            raise RuntimeError(f"START_UPDATE failed with status {status:#x}")
        print("Flash erased successfully.")

    def send_firmware_chunk(self, offset: int, data: bytes):
        """Send a firmware data chunk."""
        addr_bytes = struct.pack(">I", offset)
        addr_encoded = encode_7bit(addr_bytes)
        data_encoded = encode_7bit(data)

        payload = [BL_SYSEX_CATEGORY, BL_CMD_FW_DATA] + list(addr_encoded) + list(data_encoded)
        self._send_bl_sysex(payload)
        status = self._wait_for_ack()
        if status != BL_STATUS_OK:
            raise RuntimeError(f"FW_DATA failed at offset {offset:#x} with status {status:#x}")

    def send_verify(self, fw_size: int, fw_crc: int):
        """Send VERIFY command with firmware size and CRC."""
        size_encoded = encode_7bit(struct.pack(">I", fw_size))
        crc_encoded = encode_7bit(struct.pack(">I", fw_crc))

        payload = [BL_SYSEX_CATEGORY, BL_CMD_FW_VERIFY] + list(size_encoded) + list(crc_encoded)
        self._send_bl_sysex(payload)
        status = self._wait_for_ack()
        if status != BL_STATUS_OK:
            raise RuntimeError(f"VERIFY failed with status {status:#x}")
        print("Firmware verification passed!")

    def send_reboot(self):
        """Send REBOOT command."""
        print("Rebooting into new firmware...")
        self._send_bl_sysex([BL_SYSEX_CATEGORY, BL_CMD_REBOOT])
        try:
            self._wait_for_ack(timeout=2.0)
        except TimeoutError:
            pass  # Device may reboot before sending ACK

    def enter_bootloader_mode(self):
        """Send SysEx to running firmware to reboot into custom bootloader.

        Firmware expects: F0 <cmd_byte> F7 (no manufacturer ID).
        After this the device resets; the host must re-enumerate USB and
        re-open MIDI ports before continuing with the update."""
        print("Requesting device to enter bootloader mode...")
        self._send_fw_sysex([MIDI_SYS_CUSTOM_BOOTLOADER])
        time.sleep(0.1)
        print("Sent. Device will reboot into bootloader.")

    def update_firmware(self, firmware_path: str, enter_bl: bool = True):
        """Full firmware update procedure.

        Args:
            firmware_path: Path to the .bin firmware file.
            enter_bl: If True, send command to running firmware to reboot
                      into bootloader first (set False if already in bootloader).
        """
        # Read firmware binary
        with open(firmware_path, "rb") as f:
            firmware = f.read()

        fw_size = len(firmware)
        if fw_size > APP_FLASH_SIZE:
            raise ValueError(f"Firmware too large: {fw_size} bytes (max {APP_FLASH_SIZE})")

        fw_crc = crc32(firmware)
        print(f"Firmware: {firmware_path}")
        print(f"Size: {fw_size} bytes ({fw_size / 1024:.1f} KB)")
        print(f"CRC32: {fw_crc:#010x}")
        print()

        # Step 0 (optional): Tell running firmware to enter bootloader
        if enter_bl:
            self.enter_bootloader_mode()
            self.close()
            print("Waiting for device to re-enumerate...")
            time.sleep(3)
            self._open_ports(self._port_name)
            print()

        # Step 1: Erase
        self.send_start_update()
        print()

        # Step 2: Send firmware data
        total_chunks = (fw_size + CHUNK_SIZE - 1) // CHUNK_SIZE
        print(f"Sending {total_chunks} chunks...")

        for i in range(0, fw_size, CHUNK_SIZE):
            chunk = firmware[i:i + CHUNK_SIZE]
            self.send_firmware_chunk(i, chunk)

            # Progress
            progress = min(100, ((i + len(chunk)) * 100) // fw_size)
            chunks_done = (i // CHUNK_SIZE) + 1
            sys.stdout.write(f"\r  [{chunks_done}/{total_chunks}] {progress}%")
            sys.stdout.flush()

        print("\n")

        # Step 3: Verify
        self.send_verify(fw_size, fw_crc)
        print()

        # Step 4: Reboot
        self.send_reboot()
        print("Firmware update complete!")

    def close(self):
        """Close MIDI ports."""
        if self.outport:
            self.outport.close()
        if self.inport:
            self.inport.close()


def list_ports():
    """List available MIDI ports."""
    print("MIDI Output ports:")
    for i, name in enumerate(mido.get_output_names()):
        print(f"  [{i}] {name}")

    print("\nMIDI Input ports:")
    for i, name in enumerate(mido.get_input_names()):
        print(f"  [{i}] {name}")


def main():
    parser = argparse.ArgumentParser(description="Hexadeck MIDI Firmware Updater")
    parser.add_argument("firmware", nargs="?", help="Path to firmware .bin file")
    parser.add_argument("--port", "-p", help="MIDI port name (partial match)")
    parser.add_argument("--list", "-l", action="store_true", help="List available MIDI ports")
    parser.add_argument("--already-in-bootloader", action="store_true",
                        help="Skip sending reboot-to-bootloader command (device already in bootloader)")
    args = parser.parse_args()

    if args.list:
        list_ports()
        return

    if not args.firmware:
        parser.error("firmware .bin file path is required")

    updater = MidiFirmwareUpdater(port_name=args.port)
    try:
        updater.update_firmware(args.firmware, enter_bl=not args.already_in_bootloader)
    except Exception as e:
        print(f"\nError: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        updater.close()


if __name__ == "__main__":
    main()
