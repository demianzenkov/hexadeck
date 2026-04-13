# Hexadeck

<img src="media/rev2/device-rev2-2.jpg" width="600"/>

**Hexadeck** is a USB MIDI controller featuring 16 rotary encoders with individual configurable TFT displays. It exposes a composite USB device with both MIDI and Serial (CDC ACM) interfaces, allowing real-time control and configuration from any DAW or host application.

## Features

- **16 Rotary Encoders** — Bourns PEC12R, 24-pulse quadrature with push-button
- **16 TFT Displays** — 160×80px, individually configurable (name, colors, value bar)
- **USB MIDI** — Standard MIDI CC output, configurable channels and CC numbers per encoder
- **USB Serial** — CDC ACM interface for ASCII command-based configuration
- **MIDI SysEx Configuration** — Set/get all parameters via MIDI SysEx messages
- **Web Configurator** — Browser-based WebMIDI interface for device setup
- **Preset Storage** — Save and load configurations to on-chip flash (NVS)
- **Button MIDI** — Configurable push-button MIDI output (toggle, momentary)
- **On-device Menu** — Navigate presets and settings directly on the device
- **DFU Firmware Update** — Update firmware via USB DFU from host PC or web interface

## Hardware

| Component | Specification |
|-----------|--------------|
| MCU | STM32F446VET6 (Cortex-M4, 512KB Flash, 128KB RAM) |
| Encoders | 16× Bourns PEC12R, 24 quadrature pulses/revolution |
| Displays | 16× TFT LCD, 160×80px, SPI interface |
| Connectivity | USB-C (power + data) |
| USB Classes | Composite — MIDI + CDC ACM (Serial) |

## Project Structure

```
├── firmware/       STM32 firmware (C/C++, FreeRTOS, LVGL)
├── webapp/         Browser-based WebMIDI configurator
├── board/          PCB design files and schematics
├── case/           Enclosure design (SolidWorks, STL)
├── scripts/        Host-side Python tools and utilities
└── media/          Photos, screenshots, and demo video
```

| Directory | Description |
|-----------|-------------|
| [`firmware/`](firmware/) | Embedded firmware — build system, drivers, tasks, UI |
| [`webapp/`](webapp/) | Web-based device configurator using WebMIDI API |
| [`board/`](board/) | PCB documentation and schematics |
| [`case/`](case/) | Mechanical enclosure design files |
| [`scripts/`](scripts/) | Python utilities for firmware update, configuration, and integration |

## Assembled Device

<details>
<summary>Rev-A</summary>
<img src="media/rev1/device-rev1.jpg" width="400"/>
</details>
<details>
<summary>Rev-B</summary>
<img src="media/rev2/device-rev2-1.jpg" width="400"/>
<img src="media/rev2/device-rev2-3.jpg" width="400"/>
</details>

## User Interface

<details>
<summary>Display Interface. Simple & normal modes</summary>
<img src="media/interface/interface-1.png" width="400"/>
<img src="media/interface/interface-2.png" width="400"/>
</details>

## Demo

https://github.com/user-attachments/assets/066187ef-49a9-449c-b158-acdf62c2ad6f

## Firmware Update

The device supports USB DFU firmware updates. Enter DFU mode via:
- MIDI SysEx command from host
- Serial command: `/fw/update`
- On-device menu → "Firmware Update"

Place the firmware binary into `./scripts/firmware_updater/binaries/`.

### macOS / Linux

```bash
./scripts/firmware_updater/update_macos.sh -D hexadeck_fw_v0.bin
```

### Windows

```bash
scripts\firmware_updater\update_windows.bat -D hexadeck.bin
```

### Web

Open the [webapp](webapp/) and use the firmware update panel (requires Chrome or Edge).

## License

This project is provided as-is for reference and personal use.
