#!/usr/bin/env python
from pyTuttle import tuttle
from scipy import misc
import numpy as np

# This is called by Tuttle with the output image data
def getImage(time, data, width, height, rowSizeBytes, bitDepth, components, field):
    # FIXME this assumes 8bit RGB image. Check bitDepth, components, field
    flatarray = np.fromstring(data, np.uint8, rowSizeBytes*height)
    outImage = np.array(np.flipud(np.reshape(flatarray, (height, width, 3))))
    misc.imsave("foo.jpg", outImage)    

tuttle.Core.instance().preload()
g = tuttle.Graph()

# Input node
input_node = g.createNode("tuttle.jpegreader")
in_param = input_node.getParamByScriptName( "filename" )
in_param.setValue("data/input.jpg")

# Output node
output_buffer = g.createOutputBuffer()
output_buffer.setPyCallback(getImage)

# Connect nodes and compute
g.connect( input_node, output_buffer.getNode() )
g.compute( tuttle.NodeListArg(output_buffer.getNode()) )


