#!/usr/bin/env python

# Light each LED in sequence, and repeat.
import Image
import opc, time
import atexit
import random
"""
This program sends 10 random values between 0.0 and 1.0 to the /filter address,
waiting for 1 seconds between each value.
"""
import argparse
import math
from OSC import OSCServer
import sys
import threading
import ctypes


# numLEDs = 64
client = opc.Client('localhost:7890')
filename  = "arrow-01.png" # Image file to load
# Notice the number of LEDs is set to 0.  This is on purpose...we're asking
# the DotStar module to NOT allocate any memory for this strip...we'll handle
# our own allocation and conversion and will feed it 'raw' data.

# So we'll write directly to the pixel data buffer.  Data is not necessarily
# in RGB order -- current DotStars use BRG order, and older ones are GBR.
# Additionally, byte 0 of each 4-byte pixel is always 0xFF, so the RGB
# offsets are always in range 1-3.
# Here's the offsets for current (2015+) strips:
rOffset = 2
gOffset = 3
bOffset = 1
# For older strips, change values to 3, 1, 2
# This is ONLY necessary because we're raw-writing; for normal setPixelColor
# use, offsets can be changed with the 'order' keyword (see strandtest.py).

def on_exit():
	server.timed_out = True
	global threads
	for t in threads:
		t._stop()
	for x in range(width):         # For each column of image...
		client.put_pixels([ (0,0,0) ]*height)
	


atexit.register(on_exit)
width = 0
height = 0
pixels = []
gamma = []
column = []
image_list = 0
# Load image in RGB format and get dimensions:
def Loading():
	global pixels
	global width
	global height
	global gamma
	global image_list ;
	print "Loading..."
	image_list  = ["arrow-01.png", "hello-01.png", "rainbow-01.png", "square-01.png", "star-01.png", "star-01_.png", "triangle-01.png"]
	img       = Image.open(random.choice(image_list)).convert("RGB")
	
	pixels    = img.load()
	
	width     = img.size[0]
	height    = img.size[1]
	print "%dx%d pixels" % img.size
	# Calculate gamma correction table, makes mid-range colors look 'right':
	gamma = bytearray(256)
	for i in range(256):
		gamma[i] = int(pow(float(i) / 255.0, 2.7) * 255.0 + 0.5)
def Allocate():
	# Allocate list of bytearrays, one for each column of image.
	# Each pixel REQUIRES 4 bytes (0xFF, B, G, R).
	print "Allocating..."
	global column
	column = [0 for x in range(width)]
	for x in range(width):
		# column[x] = bytearray(height * 4)
		column[x] = [ (0,0,0) ]*height

# Convert entire RGB image into column-wise BGR bytearray list.
# The image-paint.py example proceeds in R/G/B order because it's counting
# on the library to do any necessary conversion.  Because we're preparing
# data directly for the strip, it's necessary to work in its native order.
def Convert():
	global column
	global gamma
	print "Converting..."
	global pixels
	for x in range(width):          # For each column of image...
		for y in range(height): # For each pixel in column...
			value             = pixels[x, y]    # Read pixel in image
			y3                = y            # Position in raw buffer
			column[x][y3]     = (gamma[value[0]], gamma[value[1]], gamma[value[2]])


def init():
	Loading()
	Allocate()
	Convert()

def print_volume_handler(unused_addr, args, volume):
	print("[{0}] ~ {1}".format(args[0], volume))

def print_compute_handler(unused_addr, args, volume):
	try:
		print("[{0}] ~ {1}".format(args[0], args[1](volume)))
	except ValueError: pass
def debug_print(unused_addr, args, volume):
	try:
		print("[{0}] ~ {1}".format(args[0], args[1](volume)))
	except ValueError: pass


# this method of reporting timeouts only works by convention
# that before calling handle_request() field .timed_out is 
# set to False
def handle_timeout(self):
	self.timed_out = True
def user_callback(path, tags, args, source):
	# which user will be determined by path:
	# we just throw away all slashes and join together what's left
	user = ''.join(path.split("/"))
	# tags will contain 'fff'
	# args is a OSCMessage with data
	# source is where the message came from (in case you need to reply)
	print ("Now do something with", user,args[2],args[0],1-args[1]) 

def quit_callback(path, tags, args, source):
	# don't do this at home (or it'll quit blender)
	global run
	run = False
	# user script that's called by the game engine every frame
def debug_callback(path, tags, args, source):
	print ("debug do something with", user,args[2],args[0],1-args[1]) 
def each_frame():
	# clear timed_out flag
	server.timed_out = False
	# handle all pending requests then return
	while not server.timed_out:
		print "handle_request..."
		server.handle_request()


threads = []
if __name__ == "__main__":
	init()
	parser = argparse.ArgumentParser()
	parser.add_argument("--ip",
		default="localhost", help="The ip to listen on")
	parser.add_argument("--port",
		type=int, default=3000, help="The port to listen on")
	args = parser.parse_args()

	server = OSCServer((args.ip, args.port))
	server.addMsgHandler( "/user/1", user_callback )
	server.addMsgHandler( "/user/2", user_callback )
	server.addMsgHandler( "/user/3", user_callback )
	server.addMsgHandler( "/user/4", user_callback )
	server.addMsgHandler( "/quit", quit_callback )
	server.addMsgHandler( "/", debug_callback )
	print server
	global threads
	t = threading.Thread(target=each_frame, args="")
	
	threads.append(t)
	t.start()
	
	while True:                            # Loop forever
		print "Displaying..."
		# each_frame()

		for x in range(width):         # For each column of image...
			client.put_pixels(column[x],0)  # Write raw data to strip
			client.put_pixels(column[x],1)  # Write raw data to strip
			time.sleep(0.0005)