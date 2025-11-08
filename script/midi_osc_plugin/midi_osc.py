import mido
import argparse
from pythonosc import udp_client
import time


class MidiToOscConverter:
	def __init__(self, osc_host="127.0.0.1", osc_port=8000, midi_device=None):
		self.osc_client = udp_client.SimpleUDPClient(osc_host, osc_port)
		self.midi_device = midi_device
		
	def list_midi_devices(self):
		"""List available MIDI input devices"""
		print("Available MIDI input devices:")
		for name in mido.get_input_names():
			print(f"  - {name}")
	
	def process_midi_message(self, msg):
		"""Convert MIDI message to OSC and send"""
		if msg.type == 'control_change':
			# Send CC as /cc/channel/controller value
			osc_address = f"/cc/{msg.channel}/{msg.control}"
			self.osc_client.send_message(osc_address, msg.value)
			print(f"CC: {osc_address} -> {msg.value}")
			
		elif msg.type == 'note_on':
			# Send note on as /note/channel/note velocity
			osc_address = f"/note/{msg.channel}/{msg.note}"
			self.osc_client.send_message(osc_address, msg.velocity)
			print(f"Note On: {osc_address} -> {msg.velocity}")
			
		elif msg.type == 'note_off':
			# Send note off as /note/channel/note 0
			osc_address = f"/note/{msg.channel}/{msg.note}"
			self.osc_client.send_message(osc_address, 0)
			print(f"Note Off: {osc_address} -> 0")
			
		elif msg.type == 'pitchwheel':
			# Send pitch bend as /pitch/channel value
			osc_address = f"/pitch/{msg.channel}"
			self.osc_client.send_message(osc_address, msg.pitch)
			print(f"Pitch: {osc_address} -> {msg.pitch}")
	
	def run(self):
		"""Main loop to process MIDI input"""
		try:
			# Open MIDI input
			if self.midi_device:
				inport = mido.open_input(self.midi_device)
			else:
				# Use first available device
				devices = mido.get_input_names()
				if not devices:
					print("No MIDI input devices found!")
					return
				inport = mido.open_input(devices[0])
				print(f"Using MIDI device: {devices[0]}")
			
			print(f"Sending OSC to {self.osc_client._address}:{self.osc_client._port}")
			print("Listening for MIDI messages... Press Ctrl+C to quit")
			
			# Process incoming MIDI messages
			with inport:
				for msg in inport:
					self.process_midi_message(msg)
					
		except KeyboardInterrupt:
			print("\nShutting down...")
		except Exception as e:
			print(f"Error: {e}")

def main():
	parser = argparse.ArgumentParser(description='MIDI to OSC converter')
	parser.add_argument('--host', default='127.0.0.1', help='OSC host (default: 127.0.0.1)')
	parser.add_argument('--port', type=int, default=8000, help='OSC port (default: 8000)')
	parser.add_argument('--midi-device', help='MIDI device name')
	parser.add_argument('--list-devices', action='store_true', help='List available MIDI devices')
	
	args = parser.parse_args()
	
	converter = MidiToOscConverter(args.host, args.port, args.midi_device)
	
	if args.list_devices:
		converter.list_midi_devices()
	else:
		converter.run()

if __name__ == "__main__":
	main()