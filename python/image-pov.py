#!/usr/bin/env python

# Light each LED in sequence, and repeat.
from PIL import Image
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
import thread
import ctypes
import os


server = OSCServer(("0.0.0.0", 7110))
	
client = opc.Client('localhost:7890')

# server = OSCServer( ("localhost", 7110) )
# server.timeout = 0

# numLEDs = 64
fileNameArg = ""
filename  = "onepix.png" # Image file to load
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

width = 0
height = 0
pixels = []
gamma = []
column1 = []
brightness = 0.5
image_list = 0
def on_exit(server,client):
# def on_exit(client):
	# server.timed_out = True
	server.close()
	print "on exit"
	for x in range(width):         # For each column1 of image...
		client.put_pixels( [ (0,0,0) ]*height )
		client.put_pixels([ (0,0,0) ]*height )	
# Load image in RGB format and get dimensions:
def Loading():
	global pixels
	global width
	global height
	global gamma
	global image_list ;
	print "Loading..."
	image_list  = [f for f in os.listdir('.') if os.path.isfile(f)  if  f.endswith(".png") ]
	for f in image_list:
		print "file  : " + f
	img       = Image.open(random.choice(image_list)).convert("RGB")
	# img       = Image.open("arrow-01.png").convert("RGB")
	pixels    = img.load()
	
	width     = img.size[0]
	height    = img.size[1]
	print "%dx%d pixels" % img.size
	# Calculate gamma correction table, makes mid-range colors look 'right':
	gamma = bytearray(256)
	for i in range(256):
		gamma[i] = int(pow(float(i) / 255.0, 2.7) * 255.0 * brightness  + 0.5)
		# print str(i) + " : gamma " + str(gamma[i]) 


def Allocate():
	# Allocate list of bytearrays, one for each column1 of image.
	# Each pixel REQUIRES 4 bytes (0xFF, B, G, R).
	print "Allocating..."
	global column1
	
	column1 = [0 for x in range(width)]
	
	for x in range(width):
		# column1[x] = bytearray(height * 4)
		column1[x] = [ (0,0,0) ]*(int)(128)
		

# Convert entire RGB image into column1-wise BGR bytearray list.
# The image-paint.py example proceeds in R/G/B order because it's counting
# on the library to do any necessary conversion.  Because we're preparing
# data directly for the strip, it's necessary to work in its native order.
def Convert():
	global column1
	global column2
	global gamma
	print "Converting..."
	global pixels
	for x in range(width):          # For each column1 of image...
		for y in range(height): # For each pixel in column1...
			if y<=31:
				value             = pixels[x, y]    # Read pixel in image
				y3                = y            # Position in raw buffer
				column1[x][y3]     = (gamma[value[0]], gamma[value[1]], gamma[value[2]])
			else:
				value             = pixels[x, y]    # Read pixel in image
				y3                = 60+y-32            # Position in raw buffer
				column1[x][y3]     = (gamma[value[0]], gamma[value[1]], gamma[value[2]])

def LoadingWithPath(filePath):
	global pixels
	global width
	global height
	global gamma
	global image_list ;
	print "Loading... "+filePath
	
	img       = Image.open(filePath).convert("RGB")
	# img       = Image.open("arrow-01.png").convert("RGB")
	pixels    = img.load()
	
	width     = img.size[0]
	height    = img.size[1]
	print "%dx%d pixels" % img.size
	# Calculate gamma correction table, makes mid-range colors look 'right':
	gamma = bytearray(256)
	for i in range(256):
		gamma[i] = int(pow(float(i) / 255.0, 2.7) * 255.0* brightness  + 0.5)
		# print str(i) + " : gamma " + str(gamma[i]) 


def init():
	
	Loading()
	Allocate()
	Convert()

def reset(filePath):
	LoadingWithPath(filePath)
	Allocate()
	Convert()

# this method of reporting timeouts only works by convention
# that before calling handle_request() field .timed_out is 
# set to False
# def handle_timeout(self):
	# self.timed_out = True


def change_image_callback(path, tags, args, source):
	# which user will be determined by path:
	# we just throw away all slashes and join together what's left
	
	# tags will contain 'fff'
	# args is a OSCMessage with data
	# source is where the message came from (in case you need to reply)
	print ("Now do something with",args[0]) 
	global fileNameArg 
	fileNameArg = args[0]

def off_callback(path, tags, args, source):
	print ("off",args[0]) 
	global fileNameArg 
	fileNameArg = "off"

def brightness_callback(path, tags, args, source):
	print ("brightness_callback",args[0]) 
	global brightness 
	brightness = args[0]
	



def change_speed_callback(path, tags, args, source):
	# which user will be determined by path:
	# we just throw away all slashes and join together what's left
	print ("change_speed_callback ",path) 
	print ("change_speed_callback ",tags) 
	print ("change_speed_callback ",args) 
	print ("change_speed_callback ",source) 
	# tags will contain 'fff'
	# args is a OSCMessage with data
	# source is where the message came from (in case you need to reply)
	print ("Now do something with",args[0]) 



def each_frame(server):
	# clear timed_out flag
	# handle all pending requests then return
	try:
		while True:
			# print "handle_request..."
			server.handle_request()
	except Exception as errtxt:
		print errtxt



def each_frame_led(client):
	print "Displaying..."
	

if __name__ == "__main__":
	init()
	server.addMsgHandler( "/change_image", change_image_callback )
	server.addMsgHandler( "/change_speed", change_speed_callback )
	server.addMsgHandler( "/brightness", brightness_callback )
	server.addMsgHandler( "/off", off_callback )

	# print server
	thread.start_new_thread(each_frame, (server,))
	# thread.start_new_thread(each_frame_led, (client,))
	atexit.register(on_exit, server, client);
	# atexit.register(on_exit, client);
	while True:                            # Loop forever
		if fileNameArg != "" and fileNameArg != "off":
			reset(fileNameArg)
			fileNameArg = ""
		elif fileNameArg == "off":
			for x in range(width):         # For each column1 of image...
				client.put_pixels([ (0,0,0) ]*(int)(128))  # Write raw data to strip
				time.sleep(0.0005)
		else:
			for x in range(width):         # For each column1 of image...
				client.put_pixels(column1[x])  # Write raw data to strip
				time.sleep(0.0005)
	