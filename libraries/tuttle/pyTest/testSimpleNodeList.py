# scons: Checkerboard Invert FFMpeg Png

from pyTuttle.tuttle import *

def setUp():
	core().preload(False)

def testSimpleNodeList():

	compute( [
		NodeInit( "tuttle.checkerboard", size=[50,50], explicitConversion="8i" ),
		NodeInit( "tuttle.invert" ),
		NodeInit( "tuttle.swscale", height=500 ),
		NodeInit( "tuttle.pngwriter", filename=".tests/output.png" ),
		] )
