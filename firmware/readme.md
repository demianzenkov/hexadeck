
# Hexadeck Firmware

Embedded firmware for the Hexadeck MIDI controller, targeting the **STM32F446VET6** MCU.

## Architecture

The firmware is built on **FreeRTOS** with a task-based architecture and uses **LVGL** for display rendering.

### RTOS Tasks

| Task | Description |
|------|-------------|
| `TaskOS` | Main orchestrator — encoder event processing, menu navigation, preset management |
| `TaskMIDI` | MIDI event handling — CC output, SysEx parsing and response |
| `TaskACM` | USB CDC serial interface — ASCII command parsing and response |
| `TaskButtons` | Push-button event detection (single press, quad-press) |
| `TaskLVGL` | Display rendering — LVGL tick processing, screen updates |

### Modules

| Module | Description |
|--------|-------------|
| `display` | SPI display driver, LVGL display interface, custom MIPI LCD driver |
| `ui` | LVGL screens, styles, fonts, images, and UI action handlers |
| `nvs` | Non-volatile storage — preset save/load to internal flash |
| `protocols` | MIDI and Serial API definitions and constants |
| `state` | Module state structure (per-encoder configuration) |
| `bootloader` | DFU bootloader jump logic |

### Module State

Each of the 16 encoder modules maintains its own state:

```
- Display ID, Value, Channel, CC
- Min/Max range, Step size
- Name (up to 16 characters)
- Colors: background, bar, text, border (RGB)
- Button: MIDI enable, channel, CC, press/release values, on-click mode
- Simple screen mode toggle
```

## Build

### Requirements

- ARM GCC toolchain (`arm-none-eabi-gcc`)
- GNU Make
- [STM32 for VS Code](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) extension (optional, for VS Code integration)
- OpenOCD (for flashing/debugging)

### Build Commands

```bash
# Debug build
make -f STM32Make.make DEBUG=1

# Release build
make -f STM32Make.make DEBUG=0

# Flash to device
make -f STM32Make.make flash

# Clean
make -f STM32Make.make clean
```

Or use VS Code tasks: **Build STM**, **Build Clean STM**, **Flash STM**.

## Communication Interfaces

### MIDI Output

On every encoder event, the device sends MIDI CC messages to the USB host. Each encoder's channel and CC number are independently configurable.

<details>
<summary>MidiView monitor screenshot</summary>
<img src="../media/midiview/midiview.png" width="500"/>
</details>

### MIDI SysEx Configuration

All device parameters can be read and written via MIDI SysEx messages. The webapp and Python scripts use this interface for configuration. SysEx commands include:

- Set/Get: name, value, channel, CC, range (min/max), step
- Set/Get: colors (background, border, text, bar)
- Set/Get: button mode, button MIDI channel/CC/values
- Set/Get: simple screen mode
- Trigger firmware update (DFU)

### Serial Interface (CDC ACM)

The device exposes a USB serial port for ASCII command-based configuration. Each command is a string terminated with `\n`.

| Command | Description | Parameters |
|---------|-------------|------------|
| `/set/value/x/y` | Set MIDI value | *x* — display id, *y* — value |
| `/set/channel/x/y` | Set MIDI channel | *x* — display id, *y* — channel |
| `/set/cc/x/y` | Set MIDI CC | *x* — display id, *y* — CC number |
| `/set/name/x/y` | Set display name | *x* — display id, *y* — name string |
| `/set/range/x/y` | Set max range | *x* — display id, *y* — range [1..127] |
| `/set/step/x/y` | Set encoder step | *x* — display id, *y* — step [1..127] |
| `/set/color/x/y/z` | Set element color | *x* — display id, *y* — element (`bg`, `text`, `border`, `bar`), *z* — hex RGB (`ff0000`) |
| `/fw/update` | Enter DFU mode | — |

## Display Interface

Each display shows: channel name, CC number, current value, value bar, and color-coded elements.

<details>
<summary>Interface Screenshots</summary>
<img src="../media/interface/interface-1.png" width="400"/>
<img src="../media/interface/interface-2.png" width="400"/>
</details>

## Source Structure

```
firmware/
├── Core/               STM32 HAL configuration, startup, interrupts
├── Drivers/            STM32 HAL and CMSIS drivers
├── Middlewares/         FreeRTOS, USB Composite (CDC + HID/MIDI)
├── User/
│   ├── Modules/
│   │   ├── bootloader/ DFU bootloader jump
│   │   ├── display/    SPI display driver + LVGL integration
│   │   ├── nvs/        Flash-based preset storage
│   │   ├── protocols/  MIDI and Serial API definitions
│   │   ├── state/      Per-module configuration state
│   │   └── ui/         LVGL screens, styles, fonts, images
│   └── Tasks/
│       ├── task_os      Main task — encoders, menus, presets
│       ├── task_midi    MIDI I/O and SysEx handling
│       ├── task_acm     Serial command parsing
│       ├── task_buttons Button event detection
│       └── task_lvgl    Display rendering
├── Makefile
├── STM32Make.make
├── hexadeck.ioc        STM32CubeMX project file
└── openocd.cfg         OpenOCD debug configuration
```                            


