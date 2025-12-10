#ifndef PROTOCOLS_API_MIDI_H__
#define PROTOCOLS_API_MIDI_H_


/* USB-MIDI Event Packets

Byte 0								Byte 1	  		Byte 2			Byte 3
Cable Number | Code Index Number  	Status Byte		Data Byte 1		Data Byte
*/

/* Code Index Number Classifications
------------------------------------------------------------
CIN 	MIDI_x Size 	Description
------------------------------------------------------------
0x0 	1, 2 or 3 		Miscellaneous function codes. Reserved for future extensions.
0x1 	1, 2 or 3 		Cable events. Reserved for future expansion.
0x2 	2 				Two-byte System Common messages like MTC, SongSelect, etc.
0x3 	3 				Three-byte System Common messages like SPP, etc.
0x4 	3 				SysEx starts or continues
0x5 	1 				Single-byte System Common Message or SysEx ends with following single byte.
0x6 	2 				SysEx ends with following two bytes.
0x7 	3 				SysEx ends with following three bytes.
0x8 	3	 			Note-off
0x9 	3 				Note-on
0xA 	3 				Poly-KeyPress
0xB 	3 				Control Change
0xC 	2 				Program Change
0xD		2 				Channel Pressure
0xE 	3				PitchBend Change
0xF		1				Single Byte
*/

/* Status Byte Classifications (n - MIDI Channel Number)
------------------------------------------------------------
Message					Status	Data 1				Data 2
------------------------------------------------------------
Note Off				8n		Note Number			Velocity
Note On					9n		Note Number			Velocity
Polyphonic Aftertouch	An		Note Number			Pressure
Control Change			Bn		Controller Number	Data
Program Change			Cn		Program Number		Unused
Channel Aftertouch		Dn		Pressure			Unused
Pitch Wheel				En		LSB					MSB
*/

/* System Messages Classifications
------------------------------------------------------------
Message					Status	Data 1				Data 2
------------------------------------------------------------
System Exclusive Start	F0		Manufacturers ID	Data, data, data
Song Pointer			F2		LSB					MSB
Song Select				F3		Song Number	
Tune Request			F6	
System Exclusive End	F7
Quarter Frame			F1		Data
Timing Clock			F8		
Measure End				F9		Unused	
Start					FA		
Continue				FB		
Stop					FC		
Active Sensing			FE		
Reset					FF	
*/

#define MIDI_MSG_TYPE_NOTE_OFF			0x80
#define MIDI_MSG_TYPE_NOTE_ON			0x90
#define MIDI_MSG_TYPE_POLY_AT			0xA0
#define MIDI_MSG_TYPE_CC				0xB0
#define MIDI_MSG_TYPE_PC				0xC0
#define MIDI_MSG_TYPE_CH_AT				0xD0
#define MIDI_MSG_TYPE_PW				0xE0
#define MIDI_MSG_TYPE_SYS				0xF0

#define MIDI_MSG_CIN_SYSEX_CONTINUE	 	0x04
#define MIDI_MSG_CIN_SYSEX_END_1BYTE	0x05
#define MIDI_MSG_CIN_SYSEX_END_2BYTE	0x06
#define MIDI_MSG_CIN_SYSEX_END_3BYTE	0x07

#define MIDI_MSG_STATUS_SYSEX_START	 	0xF0
#define MIDI_MSG_STATUS_SONG_POINTER	0xF2
#define MIDI_MSG_STATUS_SONG_SELECT		0xF3
#define MIDI_MSG_STATUS_TUNE_REQUEST	0xF6
#define MIDI_MSG_STATUS_SYSEX_END		0xF7
#define MIDI_MSG_STATUS_QUARTER_FRAME	0xF1
#define MIDI_MSG_STATUS_TIMING_CLOCK	0xF8
#define MIDI_MSG_STATUS_MEASURE_END		0xF9
#define MIDI_MSG_STATUS_START			0xFA
#define MIDI_MSG_STATUS_CONTINUE		0xFB
#define MIDI_MSG_STATUS_STOP			0xFC
#define MIDI_MSG_STATUS_ACTIVE_SENSING	0xFE
#define MIDI_MSG_STATUS_RESET			0xFF


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* PROTOCOLS_API_MIDI_H__ */