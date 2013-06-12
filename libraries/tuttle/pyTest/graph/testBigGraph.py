# scons: OpenImageIO Invert TimeShift Gamma Jpeg

from pyTuttle.tuttle import *

def setUp():
	core().preload(False)


def testLinearGraph():

	nodes = [
			NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			NodeInit( "tuttle.invert" ),
			NodeInit( "tuttle.timeshift", 12 ),
			NodeInit( "tuttle.gamma", master=.5 ),
		]
	for i in xrange(100):
		nodes.append( NodeInit( "tuttle.timeshift", 0 ) )
	nodes.append( NodeInit( "tuttle.jpegwriter", filename=".tests/fromExr/output-####.jpg" ) )
	
	compute( nodes )

