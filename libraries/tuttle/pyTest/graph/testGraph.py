# scons: OpenImageIO Invert TimeShift Gamma Jpeg

from pyTuttle import tuttle
from nose.tools import *

def setUp():
	tuttle.core().preload(False)


def testRenameNode():

	g = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.timeshift", 12 ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.timeshift", 0 ),
			tuttle.NodeInit( "tuttle.jpegwriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = g.addConnectedNodes(n)
	
	g.renameNode( nodes[0], "plop" )
	g.renameNode( nodes[2], "plip" )
	
	# rename to the same name
	g.renameNode( nodes[2], "plip" )
	
	# rename to an existing name
	assert_raises( Exception, g.renameNode, nodes[4], "plop" )
