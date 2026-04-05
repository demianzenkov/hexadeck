
#### Display Elements

Each display interface consists of channel, cc, name, range, stevalue and bar elements.

#### MIDI Output Interface

On every encoder event device sends MIDI commands to the connected USB host. Easy way to check MIDI commands on host PC is to use MidiView app.
<details>
<summary>MidiView monitor screenshot</summary>
<img src="../media/midiview/midiview.png" style="float: left;" width="500"/>
</details>


##### Serial Interface

Device supports a set of commands to setup interface fields through serial port. Each command is an ASCII-string with '\n' terminator. 

| Command            | Description                                  | Parameters                                                   |
| ------------------ | -------------------------------------------- | ------------------------------------------------------------ |
| `/set/value/x/y`   | Set current MIDI value                       | *x* - display id<br />*y* - value                            |
| `/set/channel/x/y` | Set MIDI channel                             | *x* - display id<br />*y* - channel                          |
| `/set/cc/x/y`      | Set MIDI CC                                  | *x* - display id<br />*y* - cc                               |
| `/set/name/x/y`    | Set channel name on display                  | *x* - display id<br />*y* - name                             |
| `/set/range/x/y`   | Set maximum range limit for MIDI values      | *x* - display id<br />*y* - range limit [1..127]             |
| `/set/step/x/y`    | Set  value step for one encoder incremention | *x* - display id<br />*y* - step [1..127]                    |
| `/set/color/x/y/z` | Set display interface element color          | *x* - display id<br />*y* - color element: "bg", "text", "border", "bar"<br />*z* - RGB color in hex, e.g. - "ff0000" - red, "ffffff" - white, etc. |
| `/fw/update`       | Put device into DFU mode for update          |                            


