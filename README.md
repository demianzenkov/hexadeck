### STM32 16-Display x 16-Encoder MIDI Controller

Firmware for composite USB device with MIDI & Serial interface running on STM32F446 MCU. Device has 16 rotary encoder & 16 TFT LCD displays (160x80px).

##### MIDI Interface

On every encoder event device sends MIDI commands to the connected USB host. Easiest way to check MIDI commands is using MidiView app.

<img src="media/midiview.png" style="float: left;" width="400"/>

##### Display Elements

Each of 16 displays has channel, name, value and bar elements.

<img src="media/display.png" style="float: left;" width="200"/>

##### Serial Interface

Device supports a set of commands to setup interface fields through serial port. Each command is an ASCII-string with '\n' terminator. 

| Command                  | Parameters                                                   |
| ------------------------ | ------------------------------------------------------------ |
| `/set/value/{x}/{y}`     | {x} - display id<br />{y} - value                            |
| `/set/level/{x}/{y}`     | {x} - display id<br />{y} - bar level                        |
| `/set/channel/{x}/{y}`   | {x} - display id<br />{y} - channel                          |
| `/set/name/{x}/{y}`      | {x} - display id<br />{y} - name                             |
| `/set/range/{x}/{y}`     | {x} - display id<br />{y} - max range limit for bar & MIDI values [1..127] |
| `/set/color/{x}/{y}/{z}` | {x} - display id<br />{y} - color element: "bg", "text", "border", "bar"<br />{z} - RGB color in hex, e.g. - "ff0000" - red, "ffffff" - white, etc. |

##### Interface example

<img src="media/interface.jpg" style="float: left;" width="500"/>

##### Demo

https://github.com/user-attachments/assets/066187ef-49a9-449c-b158-acdf62c2ad6f

##### Board Bringup

<img src="media/board_raw.jpg" width="380"/>    <img src="media/board_bringup.jpeg" width="320" />

##### TODO 

- MIDI channel configuration

##### 
