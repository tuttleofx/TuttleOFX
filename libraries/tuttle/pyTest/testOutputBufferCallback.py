# scons: pluginJpeg

from pyTuttle import tuttle
from tempfile import *

from PIL import Image
import numpy

def setUp():
	tuttle.core().preload(False)

# This is called by Tuttle with the output image data
def writeImage(time, data, width, height, rowSizeBytes, bitDepth, components, field):
	# FIXME this assumes 8bit RGB image. Check bitDepth, components, field
	flatarray = numpy.fromstring(data, numpy.uint8, rowSizeBytes*height)
	outImage = numpy.array(numpy.flipud(numpy.reshape(flatarray, (height, width, 3))))
	
	filepath = NamedTemporaryFile( prefix="outputBufferCallbackTest-", suffix=".jpg" )
	Image.fromarray(outImage).save( filepath.name )


def testOutputBufferCallback():

	g = tuttle.Graph()

	# Input node
	input_node = g.createNode("tuttle.jpegreader", filename="TuttleOFX-data/image/jpeg/MatrixLarge.jpg", channel="rgb", bitDepth="8i")

	# Output node
	output_buffer = g.createOutputBuffer()
	output_buffer.setPyCallback(writeImage)

	# Connect nodes and compute
	g.connect( input_node, output_buffer.getNode() )
	g.compute( output_buffer.getNode() )
