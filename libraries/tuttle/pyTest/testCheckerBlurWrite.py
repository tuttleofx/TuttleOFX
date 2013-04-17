# scons: Checkerboard Blur Png

from pyTuttle.tuttle import *

def setUp():
	core().preload(False)

def testCheckerBlurWrite():

	g = Graph()
	read = g.createNode( "tuttle.checkerboard", size=[20,20] )
	blur = g.createNode( "tuttle.blur", size=[0.03, 0.05] )
	write = g.createNode( "tuttle.pngwriter", filename=".tests/output.png" )

	g.connect( [read, blur, write] )
	g.compute( write )
