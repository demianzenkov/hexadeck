# Script for connecting to device via Serial Port and send an image as a byte array
import serial
import time

SLEEP_TIMEOUT = 0.005
ser = None

def set_value(disp, value):
	message = '/set/value/' + str(disp) + '/' + str(value) + '\n'
	ser.write(message.encode())
	time.sleep(SLEEP_TIMEOUT)
 

def set_level(disp, level):
    message = '/set/level/' + str(disp) + '/' + str(level) + '\n'
    ser.write(message.encode())
    time.sleep(SLEEP_TIMEOUT)

def set_channel(disp, channel):
    message = '/set/channel/' + str(disp) + '/' + str(channel) + '\n'
    ser.write(message.encode())
    time.sleep(SLEEP_TIMEOUT)

def set_name(disp, name):
	message = '/set/name/' + str(disp) + '/' + name + '\n'
	ser.write(message.encode())
	time.sleep(SLEEP_TIMEOUT)

def set_color(disp, element, color):
    message = '/set/color/' + str(element) + '/' + str(disp) + '/' + color + '\n'
    ser.write(message.encode())
    time.sleep(SLEEP_TIMEOUT)
    
def set_border_color(disp, color):
    return set_color(disp, 'border', color)

def set_bar_color(disp, color):
    return set_color(disp, 'bar', color)

def set_text_color(disp, color):
    return set_color(disp, 'text', color)

def set_bg_color(disp, color):
    return set_color(disp, 'bg', color)


def init_values():
	# Pretty gradient colors for each display
	gradient_colors = [
		'1A1A2E', '16213E', '0F3460', '533483',  # Dark blue to purple
		'2D1B69', '3E065F', '700B97', 'A0236E',  # Purple to magenta
		'C42847', 'E63946', 'F77F00', 'FCBF49',  # Red to orange to yellow
		'06FFA5', '4ECDC4', '45B7D1', '96CEB4'   # Green to turquoise to blue
	]
	
	# Musical instrument names for better context
	instrument_names = [
		'BASS', 'KICK', 'SNARE', 'HIHAT',
		'LEAD', 'PAD', 'PLUCK', 'ARPEG',
		'VERB', 'DELAY', 'FILT', 'DIST',
		'VOL1', 'VOL2', 'VOL3', 'MASTER'
	]
	
	for i in range(0, 16):
		set_name(i, instrument_names[i])
		set_value(i, '0.0')
		set_level(i, 64)  # Start at middle position
		set_bg_color(i, gradient_colors[i])
		set_bar_color(i, 'FFFFFF')  # Clean white bars
		set_text_color(i, 'FFFFFF')  # White text for good contrast
		set_border_color(i, 'FFFFFF')  # White borders
		set_channel(i, i+1)
  
# Demo of controlling the device UI via serial commands
# 15 seconds sequence with cool visual effects
def demo_sequence():
    import math
    print("Starting demo sequence...")
    
    # Phase 1: Introduction with names (2 seconds)
    print("Phase 1: Introduction")
    intro_names = ['BASS', 'KICK', 'SNARE', 'HIHAT', 
                   'LEAD', 'PAD', 'ARP', 'FX1',
                   'VOL1', 'VOL2', 'VOL3', 'VOL4',
                   'SEND1', 'SEND2', 'SEND3', 'MASTER']
    
    for i in range(16):
        set_name(i, intro_names[i])
        set_bg_color(i, '1E1E1E')  # Dark background
        set_text_color(i, 'FFFFFF')  # White text
        set_bar_color(i, '00FF88')  # Green bars
        set_value(i, '0.0')
        set_level(i, 0)
        time.sleep(0.1)
    
    time.sleep(1)
    
    # Phase 2: Wave animation with instrument themes (3 seconds)
    print("Phase 2: Musical wave animation")
    wave_names = ['NOTE', 'BEAT', 'RIFF', 'TONE', 'WAVE', 'SYNC', 'LOOP', 'PLAY']
    pastel_colors = ['FF6B6B', '4ECDC4', '45B7D1', 'FFA07A', 'FFE66D', 'D63384', '6F42C1', '20C997']
    
    for wave in range(8):  # 8 waves across displays
        for i in range(16):
            # Create wave effect with levels
            level = int(64 + 50 * math.sin((wave * 0.5 + i * 0.3)))
            level = max(0, min(127, level))
            set_level(i, level)
            
            # Dynamic value display
            value = f"{level/127*100:.0f}%"
            set_value(i, value)
            
            # Cycle through names and colors
            name_idx = (i + wave) % len(wave_names)
            color_idx = (i + wave) % len(pastel_colors)
            
            set_name(i, wave_names[name_idx])
            set_bg_color(i, pastel_colors[color_idx])
            set_bar_color(i, 'FFFFFF')  # White bars
            set_text_color(i, '000000')  # Black text
            
        time.sleep(0.4)
    
    # Phase 3: Drum machine simulation (3 seconds)
    print("Phase 3: Drum machine")
    drum_names = ['KICK', 'SNARE', 'HIHAT', 'CRASH',
                  'TOM1', 'TOM2', 'TOM3', 'RIDE',
                  'PERC1', 'PERC2', 'PERC3', 'PERC4',
                  'VOL', 'PITCH', 'FILTER', 'MASTER']
    
    # Set drum machine theme with better contrast
    for i in range(16):
        set_name(i, drum_names[i])
        set_bg_color(i, '1A1A1A')  # Dark background
        set_text_color(i, 'FFFFFF')  # White text for good contrast
        set_bar_color(i, '666666')  # Gray bars when inactive
        set_level(i, 10)  # Small level when inactive
        set_value(i, 'READY')  # Show "READY" instead of "OFF"
    
    # Simulate drum pattern with better visual feedback
    drum_pattern = [
        [0, 4, 8, 12],    # Kick pattern
        [2, 6, 10, 14],   # Snare pattern
        [1, 3, 5, 7, 9, 11, 13, 15],  # Hi-hat pattern
        [0, 2, 4, 6],     # Mix pattern
    ]
    
    # Different colors for different drum types
    drum_colors = ['FF3030', 'FFD700', '00FF80', 'FF8040']  # Red, Gold, Green, Orange
    
    for pattern_step in range(8):
        # Reset all to inactive state
        for i in range(16):
            set_level(i, 10)
            set_value(i, 'READY')
            set_bg_color(i, '1A1A1A')
            set_bar_color(i, '666666')
        
        # Trigger pattern with color coding
        pattern_idx = pattern_step % len(drum_pattern)
        color = drum_colors[pattern_idx]
        
        for drum_id in drum_pattern[pattern_idx]:
            set_level(drum_id, 127)  # Full level when triggered
            set_value(drum_id, 'BANG!')
            set_bg_color(drum_id, color)
            set_bar_color(drum_id, 'FFFFFF')  # White bars when active
        
        time.sleep(0.375)
    
    # Phase 4: Synthesizer control (3 seconds)
    print("Phase 4: Synthesizer")
    synth_names = ['OSC1', 'OSC2', 'OSC3', 'NOISE',
                   'FILTER', 'RESON', 'ENV1', 'ENV2',
                   'LFO1', 'LFO2', 'DELAY', 'REVERB',
                   'DIST', 'COMP', 'EQ', 'MASTER']
    
    synth_colors = ['FF0040', 'FF4000', 'FF8000', 'FFBF00',
                    '00FF40', '00FF80', '00FFBF', '00BFFF',
                    '0080FF', '0040FF', '4000FF', '8000FF',
                    'BF00FF', 'FF00BF', 'FF0080', 'FF0040']
    
    for i in range(16):
        set_name(i, synth_names[i])
        set_bg_color(i, '0A0A0A')  # Very dark
        set_text_color(i, synth_colors[i])
        set_bar_color(i, synth_colors[i])
    
    # Animate synthesizer parameters
    for cycle in range(12):
        for i in range(16):
            # Each control has different behavior
            if i < 4:  # Oscillators
                level = int(80 + 40 * math.sin(cycle * 0.3 + i))
            elif i < 8:  # Filter section
                level = int(60 + 60 * math.cos(cycle * 0.2 + i))
            elif i < 12:  # Modulation
                level = int(40 + 80 * math.sin(cycle * 0.4 + i * 0.5))
            else:  # Effects
                level = int(70 + 50 * math.cos(cycle * 0.15 + i))
            
            level = max(0, min(127, level))
            set_level(i, level)
            set_value(i, f"{level}")
        
        time.sleep(0.25)
    
    # Phase 5: DJ mixer finale (4 seconds)
    print("Phase 5: DJ Mixer finale")
    dj_names = ['DECK A', 'DECK B', 'DECK C', 'DECK D',
                'HIGH A', 'MID A', 'LOW A', 'GAIN A',
                'HIGH B', 'MID B', 'LOW B', 'GAIN B',
                'XFADER', 'CUE', 'BOOTH', 'MASTER']
    
    # Set DJ theme
    for i in range(16):
        set_name(i, dj_names[i])
        set_bg_color(i, '000000')  # Black
        set_text_color(i, '00FFFF')  # Cyan
        set_border_color(i, '00FFFF')  # Cyan border
        set_bar_color(i, 'FF0080')  # Pink bars
    
    # Final synchronized show
    for cycle in range(16):
        # All faders move together in a final crescendo
        level = int(64 + 60 * math.sin(cycle * 0.5))
        level = max(0, min(127, level))
        
        # Create stereo effect
        for i in range(16):
            if i % 2 == 0:  # Left channel
                actual_level = level
            else:  # Right channel - slightly offset
                actual_level = int(64 + 60 * math.sin(cycle * 0.5 + 0.3))
                actual_level = max(0, min(127, actual_level))
            
            set_level(i, actual_level)
            set_value(i, f"{actual_level}")
            
            # Flash effect on peak
            if actual_level > 110:
                set_bg_color(i, 'FFFFFF')
                set_text_color(i, '000000')
            else:
                set_bg_color(i, '000000')
                set_text_color(i, '00FFFF')
        
        time.sleep(0.25)
    
    # Return to initial state
    print("Demo sequence complete!")
    time.sleep(0.5)
    init_values()


def test_methods():
    for i in range(16):
        # set_value(i, 100)
        # set_name(i, 'D' + str(i+1))
        # set_level(i, 100)
        set_bg_color(i, '000000')
        # set_bar_color(i, '00FF00')
        # set_text_color(i, '0000FF')
        # set_channel(i, i+1)
  
# Establish a serial connection with the device
ser = serial.Serial('/dev/cu.usbmodem3962346234323', 576000)
init_values()
demo_sequence()
# test_methods()
ser.close()

print('parameters sent')


