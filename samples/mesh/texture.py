#!/usr/bin/env python3

# Requires PIL:
#  pip3 install Pillow

# To convert texture.png to texture.h, run:
#  ./texture.py texture.png > texture.h

import sys
from PIL import Image

# Load an image and convert it to RGBA
image = Image.open(sys.argv[1])
image = image.convert("RGBA")

# Store metadata
print("const unsigned int texture_width = %d;" % image.width)
print("const unsigned int texture_height = %d;" % image.height)
print("const unsigned int texture_pitch = %d;" % (image.width * 4))

# Loop over all pixels and output them
print("const uint8_t texture_rgba[] = {")
pixels = image.load()
for y in range(image.height):
  for x in range(image.width):
    r, g, b, a = pixels[x, y]
    print("\t0x%02x, 0x%02x, 0x%02x, 0x%02x," % (r, g, b, a))
print("};")
