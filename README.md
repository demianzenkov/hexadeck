### Hexadeck

<img src="media/rev2/device-rev2-2.jpg" style="float: left;" width="600"/>

#### Device Features

- Composite USB interface - MIDI & Serial classes
- Configurable MIDI outputs for encoders & push-buttons (channels, cc)
- Configurable UI screen elements
- Configuration through MIDI input & Serial interfaces

#### Hardware
- MCU - STM32F446VET6
- 16x Encoders - Bourns PEC12R, 24 quadrature 
- 16x Displays - TFT LCD, 160x80px
- USB-C for power and data

#### Assembled Device

<details>
<summary>Rev-A</summary>
<img src="media/rev1/device-rev1.jpg" width="400"/>
</details>
<details>
<summary>Rev-B</summary>
<img src="media/rev2/device-rev2-1.jpg" width="400"/>
<img src="media/rev2/device-rev2-3.jpg" width="400"/>
</details>

#### Demo [Rev-A]

https://github.com/user-attachments/assets/066187ef-49a9-449c-b158-acdf62c2ad6f

#### Firmware update

Device can be update through DFU mode. To put device into DFU mode use MIDI Sysex or Serial commands from host PC or go to the menu and select "Firmware Update". Place the firmware into the `./scripts/firmware_updater/binaries/` folder.

##### MacOS/Linux

```bash
./scripts/firmware_updater/update.sh -D hexadeck_fw_v0.bin
```

##### Windows

```bash
./scripts/firmware_updater/update.bat -D hexadeck_fw_v0.bin
```
