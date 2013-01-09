# scons: OpenImageIO Invert TimeShift Gamma Jpeg

from pyTuttle.tuttle import *

def setUp():
	core().preload(False)

def testTimeShift():

	compute( [
		#NodeInit( "tuttle.oiioreader", filename="/home/fab/imgTests/seqTga/###.tga" ),
		NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
		NodeInit( "tuttle.invert" ),
		NodeInit( "tuttle.timeshift", 12 ),
		NodeInit( "tuttle.gamma", master=.5 ),
		NodeInit( "tuttle.jpegwriter", filename=".tests/fromExr/output-####.jpg" ),
		] )

