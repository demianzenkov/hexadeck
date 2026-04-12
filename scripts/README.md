# Hexadeck Scripts

Host-side Python utilities for firmware updates, device configuration, and integration with external applications.

## Tools

### Firmware Updater

Flashes firmware binaries to the device over USB DFU.

```bash
# macOS/Linux — auto-installs dependencies (libusb, pyusb, pyfu-usb)
./scripts/firmware_updater/update_macos.sh -D hexadeck_fw_v0.bin

# Windows
./scripts/firmware_updater/update_windows.bat -D hexadeck_fw_v0.bin
```

Place firmware binaries in `firmware_updater/binaries/`. The device must be in DFU mode — trigger via serial command (`/fw/update`), MIDI SysEx, or the on-device menu.

**Dependencies**: `libusb`, `pyusb`, `pyfu-usb`

---

### MIDI Configurator

Python demo for configuring the device via MIDI SysEx messages.

```bash
python3 scripts/midi_configurator/midi_config_demo.py
```

Demonstrates setting names, values, channels, CC numbers, ranges, and colors for all 16 modules. Uses the `mido` library for MIDI communication.

**Dependencies**: `mido`, `python-rtmidi`

---

### Serial Configurator

Python demo for configuring the device via the USB serial (CDC ACM) interface using ASCII commands.

```bash
python3 scripts/serial_configurator/serial_config_demo.py
```

Demonstrates the full serial command set: setting values, channels, names, and per-module colors. Includes animated bar demo sequences.

**Dependencies**: `pyserial`

---

### Image Sender

Sends binary image data to the device over serial for display customization.

```bash
python3 scripts/image_sender/image_sender.py
```

Reads a `.bin` image file, splits it into 64-byte USB packets, and transmits them over the serial connection.

**Dependencies**: `pyserial`, `Pillow`

---

### MIDI-to-OSC Bridge

Bridges MIDI input from the Hexadeck to OSC (Open Sound Control) messages for integration with applications like TouchDesigner, Max/MSP, or any OSC-compatible software.

```bash
python3 scripts/midi_osc_plugin/midi_osc.py --device "Hexadeck Controller" --host 127.0.0.1 --port 8000
```

Converts MIDI messages to OSC addresses:

| MIDI Message | OSC Address | Value |
|-------------|-------------|-------|
| CC | `/cc/{channel}/{controller}` | value |
| Note On | `/note/{channel}/{note}` | velocity |
| Note Off | `/note/{channel}/{note}` | 0 |
| Pitch Bend | `/pitch/{channel}` | pitch |

**Dependencies**: `mido`, `python-rtmidi`, `python-osc`

## Structure

```
scripts/
├── firmware_updater/
│   ├── binaries/               Firmware .bin files
│   ├── update_macos.sh         macOS/Linux update script
│   └── update_windows.bat      Windows update script
├── image_sender/
│   └── image_sender.py         Serial image transfer tool
├── midi_configurator/
│   └── midi_config_demo.py     MIDI SysEx configuration demo
├── midi_osc_plugin/
│   └── midi_osc.py             MIDI-to-OSC bridge
└── serial_configurator/
    └── serial_config_demo.py   Serial configuration demo
```
