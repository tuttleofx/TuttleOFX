#!/usr/bin/env python
from pyTuttle import tuttle
import Image
import numpy

# This is called by Tuttle with the output image data
def writeImage(time, data, width, height, rowSizeBytes, bitDepth, components, field):
	# Get pixel component type and size
        if bitDepth == 1:
            comptype = np.uint8
            bytes_per_comp = 1
        elif bitDepth == 2:
            comptype = np.uint16
            bytes_per_comp = 2
        elif bitDepth == 3:
            comptype = np.float32
            bytes_per_comp = 4
        else:
            raise Exception("Unknown bitDepth getting image from OutputBuffer")

        # FIXME: This doesn't work if rowSizeBytes isn't  a multiple of bytes_per_comp.
	#        It seems unlikely that rows would be allocated with a different component type
	#        but it is possible. In that case, each row would have to be allocated separately
	if rowSizeBytes % bytes_per_comp != 0:
		raise Exception("Unsupported image layout")

	# Get number of pixel components
        if components == 0 or components == 3:
            num_comps = 1
        elif components == 1:
            num_comps = 4
        elif components == 2:
            num_comps = 3
        else:
            raise Exception("Unknown components getting image from OutputBuffer")
        
        bytes_per_pixel = bytes_per_comp*num_comps

	# Convert data into numpy array
        outImage = np.fromstring(data, comptype, (rowSizeBytes/bytes_per_comp)*height)
        if rowSizeBytes != width*bytes_per_pixel:
            outImage = np.reshape(outImage,(height, rowSizeBytes/bytes_per_comp))
            outImage = outImage[0:height,0:width*num_comps]
        outImage = np.reshape(outImage,(height,width,num_comps))

	Image.fromarray(outImage).save("foo.jpg")

tuttle.Core.instance().preload()
g = tuttle.Graph()

# Input node
input_node = g.createNode("tuttle.jpegreader", filename="data/input.jpg", channel="rgb", bitDepth="8i")

# Output node
output_buffer = g.createOutputBuffer()
output_buffer.setPyCallback(writeImage)

# Connect nodes and compute
g.connect( input_node, output_buffer.getNode() )
g.compute( output_buffer.getNode() )


