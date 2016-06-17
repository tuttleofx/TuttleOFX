from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testLinearGraph():

	nodes = [
			tuttle.NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.timeshift", 12 ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
		]
	for i in range(100):
		nodes.append( tuttle.NodeInit( "tuttle.timeshift", 0 ) )
	nodes.append( tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ) )
	
	tuttle.compute( nodes )
