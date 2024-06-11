#!/bin/python3
# Simple Python script to convert a bitmap into a map file
#
# black pixel == wall
# white pixel == air

from PIL import Image
import sys

img = Image.open(sys.argv[1])

width, height = img.size

img = img.convert("L")


print(height, width)
for y in range(height):
    for x in range(width):
        print('W' if img.getpixel((x,y)) < 127 else '.', end = "")
    print("")
