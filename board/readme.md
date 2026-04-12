# Hexadeck PCB

Hardware design documentation for the Hexadeck MIDI controller.

## Specifications

| Parameter | Value |
|-----------|-------|
| MCU | STM32F446VET6 (LQFP-100) |
| Flash | 512 KB |
| RAM | 128 KB |
| Core | ARM Cortex-M4 @ 180 MHz, FPU |
| Encoders | 16× Bourns PEC12R (24 pulses/rev, with push-button) |
| Displays | 16× TFT LCD 160×80px (SPI) |
| USB | USB-C — Composite device (MIDI + CDC ACM) |
| Power | USB bus-powered (5V) |

## Schematics

See [schematics_v0_revB.pdf](schematics_v0_revB.pdf) for the full schematic of the Rev-B board.

## Board Revisions

<details>
<summary>Rev-A</summary>
<img src="../media/rev1/pcb_rev1-1.jpg" width="350"/>
<img src="../media/rev1/pcb_rev1-2.jpeg" width="350"/>
</details>

<details>
<summary>Rev-B</summary>
<img src="../media/rev2/pcb_rev2.jpg" width="400"/>
</details>
