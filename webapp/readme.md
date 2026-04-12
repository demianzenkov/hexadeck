
# Hexadeck Web Configurator

Browser-based configuration tool for the Hexadeck MIDI controller using the **WebMIDI API**.

<img src="../media/interface/interface-1.png" width="500"/>

## Features

- **Live Device Grid** — Visual representation of all 16 encoder modules
- **Display Settings** — Configure name, colors (background, border, text, bar) per module
- **Knob Settings** — Set MIDI channel, CC number, value range, step size
- **Button Settings** — Configure push-button MIDI output (channel, CC, press/release values, on-click mode)
- **State Sync** — Read current device configuration via MIDI SysEx
- **Firmware Update** — USB DFU firmware flashing directly from the browser (WebUSB)
- **Demo Mode** — Preview the interface without a connected device

## Requirements

- **Browser**: Chrome or Edge (WebMIDI requires a secure context)
- **Connection**: Hexadeck controller connected via USB
- **Server**: Local HTTP server (WebMIDI requires `localhost` or HTTPS)

## Run Locally

```bash
cd webapp
python3 -m http.server 8000
```

Then open [http://localhost:8000](http://localhost:8000) in Chrome or Edge.

### Alternative

```bash
# macOS/Linux
./run.sh

# Windows
run.bat
```

## Usage

1. Open the webapp in Chrome/Edge
2. Click **Connect** to request MIDI access
3. Select the Hexadeck Controller from the MIDI input/output dropdowns
4. Click **Sync** to read the current device state
5. Click any module in the grid to open the configuration panel
6. Use the **Display**, **Knob**, and **Button** tabs to modify settings
7. Changes are sent to the device immediately via MIDI SysEx

### Firmware Update

1. Click the **Firmware** button in the command panel
2. Select a `.bin` firmware file
3. The device will enter DFU mode and the update will proceed automatically

## Communication

The webapp communicates with the device exclusively through **MIDI SysEx** messages. All configuration parameters (name, value, channel, CC, range, step, colors, button settings) are transferred using a custom SysEx protocol defined in `app.js`.

## File Structure

```
webapp/
├── app.js          Main application logic and MIDI SysEx protocol
├── index.html      UI layout
├── styles.css      Styling
├── run.sh          Launch script (macOS/Linux)
├── run.bat         Launch script (Windows)
└── modules/
    └── webdfu/     WebUSB DFU library for firmware updates
```