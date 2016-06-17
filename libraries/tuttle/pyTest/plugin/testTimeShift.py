from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testTimeShift():
	tuttle.compute( [
		#tuttle.NodeInit( "tuttle.oiioreader", filename="/home/fab/imgTests/seqTga/###.tga" ),
		tuttle.NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
		tuttle.NodeInit( "tuttle.invert" ),
		tuttle.NodeInit( "tuttle.timeshift", 12 ),
		tuttle.NodeInit( "tuttle.gamma", master=.5 ),
		tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		] )
