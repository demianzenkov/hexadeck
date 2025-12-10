import mido
import time
from enum import Enum

class MidiConfigMessage(Enum):
	MIDI_SYS_SET_NAME = 0
	MIDI_SYS_SET_VALUE = 1
	MIDI_SYS_SET_CHANNEL = 2
	MIDI_SYS_SET_CC = 3
	MIDI_SYS_SET_RANGE = 4
	MIDI_SYS_SET_COLOR_BG = 5
	MIDI_SYS_SET_COLOR_BORDER = 6
	MIDI_SYS_SET_COLOR_TEXT = 7
	MIDI_SYS_SET_COLOR_BAR = 8
	MIDI_SYS_FIRMWARE_UPDATE = 9

init_bg_colors = [
	'1A1A2E', '16213E', '0F3460', '533483',  # Dark blue to purple
	'2D1B69', '3E065F', '700B97', 'A0236E',  # Purple to magenta
	'C42847', 'E63946', 'F77F00', 'FCBF49',  # Red to orange to yellow
	'06FFA5', '4ECDC4', '45B7D1', '96CEB4'   # Green to turquoise to blue
]

init_names = [
	'BASS', 'KICK', 'SNARE', 'HIHAT',
	'LEAD', 'PAD', 'PLUCK', 'ARPEG',
	'VERB', 'DELAY', 'FILT', 'DIST',
	'VOL1', 'VOL2', 'VOL3', 'MASTER'
]

def send_set_name(channel: int, name: str):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_NAME.value, channel] + [ord(c) for c in name])
	outport.send(msg)
	time.sleep(0.05)

def send_set_value(channel: int, value: int):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_VALUE.value, channel, value])
	outport.send(msg)
	time.sleep(0.05)

def send_set_channel(channel: int, midi_channel: int):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_CHANNEL.value, channel, midi_channel])
	outport.send(msg)
	time.sleep(0.05)

def send_set_cc(channel: int, cc: int):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_CC.value, channel, cc])
	outport.send(msg)
	time.sleep(0.05)

def send_set_range(channel: int, min_value: int, max_value: int):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_RANGE.value, channel, min_value, max_value])
	outport.send(msg)
	time.sleep(0.05)

def send_set_color_bg(channel: int, r: int, g: int, b: int):
	# Send each color as two 7-bit bytes: [LSB, MSB]
	def split_byte(val):
		return [val & 0x7F, (val >> 7) & 0x01]
	r_bytes = split_byte(r)
	g_bytes = split_byte(g)
	b_bytes = split_byte(b)
	data = [MidiConfigMessage.MIDI_SYS_SET_COLOR_BG.value, channel] + r_bytes + g_bytes + b_bytes
	msg = mido.Message('sysex', data=data)
	outport.send(msg)
	time.sleep(0.05)

def send_set_color_border(channel: int, r: int, g: int, b: int):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_COLOR_BORDER.value, channel, r, g, b])
	outport.send(msg)
	time.sleep(0.05)
 
def send_set_color_text(channel: int, r: int, g: int, b: int):
	msg = mido.Message('sysex', data=[MidiConfigMessage.MIDI_SYS_SET_COLOR_TEXT.value, channel, r, g, b])
	outport.send(msg)
	time.sleep(0.05)

def init_sequence():
	for i in range(16):
    # set names and bg color from init arrays
		send_set_name(i, init_names[i])
		color_hex = init_bg_colors[i]
		r = int(color_hex[0:2], 16)
		g = int(color_hex[2:4], 16)
		b = int(color_hex[4:6], 16)
		send_set_color_bg(i, r, g, b)
		

outport = mido.open_output('Hexadeck Controller')
init_sequence()