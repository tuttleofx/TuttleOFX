# scons: Checkerboard Invert AudioVideo Png

from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testSimpleNodeList():
	tuttle.compute( [
		tuttle.NodeInit( "tuttle.checkerboard", size=[50,50], explicitConversion="8i" ),
		tuttle.NodeInit( "tuttle.invert" ),
		tuttle.NodeInit( "tuttle.swscale", height=500 ),
		tuttle.NodeInit( "tuttle.pngwriter", filename=".tests/output.png" ),
		] )
