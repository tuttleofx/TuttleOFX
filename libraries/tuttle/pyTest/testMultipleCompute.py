# scons: pluginCheckerboard pluginInvert pluginColorTransform pluginPng

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testMultipleCompute_simpleNodes():

	assert tuttle.compute( [
		tuttle.NodeInit( "tuttle.checkerboard", format="PAL" ),
		tuttle.NodeInit( "tuttle.invert" ),
		tuttle.NodeInit( "tuttle.colortransform", offsetGlobal=.2 ),
		tuttle.NodeInit( "tuttle.pngwriter", filename=".tests/output.png" )
		] )


def testMultipleCompute_renderSomeFrames():

	g = tuttle.Graph()
	checkerboard = g.createNode( "tuttle.checkerboard", size=[50,50] )
	invert = g.createNode( "tuttle.invert" )
	write = g.createNode( "tuttle.pngwriter", filename=".tests/output.png" )

	g.connect( [checkerboard,invert,write] )
	assert g.compute( write, tuttle.ComputeOptions(0) )
	assert g.compute( write, tuttle.ComputeOptions(5) )
	assert g.compute( write, tuttle.ComputeOptions(0) )

