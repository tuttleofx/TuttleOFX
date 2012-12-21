# scons: Png

from pyTuttle import tuttle
import numpy
import Image

def setUp():
	tuttle.core().preload()


# This is called by Tuttle as an input of the graph
def getImage(time):
	img = numpy.asarray( Image.open("data/input.jpg") )
	img = numpy.flipud(img)
	return (img.tostring(), img.shape[1], img.shape[0], img.strides[0])


def testInputBufferCallback():

	g = tuttle.Graph()

	ib = g.createInputBuffer()
	ib.setComponents( tuttle.InputBufferWrapper.ePixelComponentRGB );
	ib.setBitDepth( tuttle.InputBufferWrapper.eBitDepthUByte );
	ib.setOrientation( tuttle.InputBufferWrapper.eImageOrientationFromTopToBottom );
	ib.setPyCallback( getImage )

	w = g.createNode("tuttle.pngwriter", filename=".tests/foo.png")

	g.connect( ib.getNode(), w )
	g.compute( w )
