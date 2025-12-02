# Script for connecting to device via Serial Port and send an image as a byte array
import serial
from PIL import Image
import io
from time import sleep

USB_FAS_MAX_PACKET_SIZE = 64
PACKET_SIZE = USB_FAS_MAX_PACKET_SIZE

with open('icon.bin', 'rb') as bin_file:
    byte_arr = bin_file.read()


# Slice the byte array into chunks of 256 bytes
chunks = [byte_arr[i:i + PACKET_SIZE] for i in range(0, len(byte_arr), PACKET_SIZE)]

print('image size:', len(byte_arr))
print('number of chunks:', len(chunks))
print('chunk size:', len(chunks[0]))

# Establish a serial connection with the device
ser = serial.Serial('/dev/cu.usbmodem3695347034302', 115200)
# Iterate over the chunks
for chunk in chunks:
    # Send each chunk over the serial connection
    ser.write(chunk)
    sleep(0.01)
ser.close()

print('image sent')

