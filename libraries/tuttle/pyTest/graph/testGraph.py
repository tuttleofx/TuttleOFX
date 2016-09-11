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
			tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = g.addConnectedNodes(n)
	
	g.renameNode( nodes[0], "plop" )
	g.renameNode( nodes[2], "plip" )
	
	# rename to the same name
	g.renameNode( nodes[2], "plip" )
	
	# rename to an existing name
	assert_raises( Exception, g.renameNode, nodes[4], "plop" )


def testDeleteNode():

	g = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.timeshift", 12 ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.timeshift", 0 ),
			tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = g.addConnectedNodes(n)
	
	assert_equals(g.getNbNodes(), 6)

	g.deleteNode(nodes[3])
	g.deleteNode(nodes[1])

	assert_equals(g.getNbNodes(), 4)

	checkerboard = g.createNode( "tuttle.gamma", master=.5 ).asImageEffectNode()
	lensdistort  = g.createNode( "tuttle.timeshift", 0 ).asImageEffectNode()

	assert_equals(g.getNbNodes(), 6)


def testClearGraph():

	g = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.timeshift", 12 ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.timeshift", 0 ),
			tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = g.addConnectedNodes(n)

	assert_equals(g.getNbNodes(), 6)
	assert_equals(g.getNbConnections(), 5)

	g.clear()

	assert_equals(g.getNbNodes(), 0)
	assert_equals(g.getNbConnections(), 0)

	checkerboard = g.createNode( "tuttle.gamma", master=.5 ).asImageEffectNode()
	lensdistort  = g.createNode( "tuttle.timeshift", 0 ).asImageEffectNode()

	assert_equals(g.getNbNodes(), 2)


def testDeleteUnconnectedNodes():

	g = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.oiioreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.timeshift", 12 ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.timeshift", 0 ),
			tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = g.addConnectedNodes(n)

	assert_equals(g.getNbNodes(), 6)
	assert_equals(g.getNbConnections(), 5)

	g.deleteUnconnectedNodes( nodes[-1] )

	assert_equals(g.getNbNodes(), 6)
	assert_equals(g.getNbConnections(), 5)

	checkerboard = g.createNode( "tuttle.gamma", master=.5 ).asImageEffectNode()
	lensdistort  = g.createNode( "tuttle.timeshift", 0. ).asImageEffectNode()

	assert_equals(g.getNbNodes(), 8)
	assert_equals(g.getNbConnections(), 5)

	assert_equals(len(g.getUnconnectedNodes(nodes[-1])), 2)
	assert_equals(len(g.getConnectedNodes(nodes[-1])), 6)

	g.deleteUnconnectedNodes( nodes[-1] )

	assert_equals(g.getNbNodes(), 6)
	assert_equals(g.getNbConnections(), 5)
