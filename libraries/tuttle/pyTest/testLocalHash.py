# scons: Checkerboard Blur Png

from pyTuttle import tuttle

from nose.tools import *

def setUp():
	tuttle.core().preload(False)


def testBasic():
	g = tuttle.Graph()
	read1 = g.createNode( "tuttle.checkerboard", size=[50,50] )
	read2 = g.createNode( "tuttle.checkerboard", size=[50,49] )
	read2b = g.createNode( "tuttle.checkerboard", size=[50,49] )

	print "dir:", dir(read1)
	#print "dir:", dir(read1.getLocalHashAtTime)
	#print "help:", help(read1)
	#print "read1   0.0:", read1.getLocalHashAtTime()
	#print "read1   0.0:", read1.getLocalHashAtTime("oo")
	print "read1   0.0:", read1.asImageEffectNode().getLocalHashAtTime(0.0)
	print "read1   0.0:", read1.getLocalHashAtTime(0.0)
	print "read2  12.0:", read2.getLocalHashAtTime(12.0)
	print "read2b  0.0:", read2b.getLocalHashAtTime(0.0)
	assert_equal( read1.getLocalHashAtTime(0.0), read1.getLocalHashAtTime(12.0) )
	assert_equal( read2.getLocalHashAtTime(0.0), read2b.getLocalHashAtTime(12.0) )
	
	assert_not_equal( read1.getLocalHashAtTime(0.0), read2.getLocalHashAtTime(0.0) )


def testAnimation():
	g = tuttle.Graph()
	blur = g.createNode( "tuttle.blur", size={1.0:[80.0, 40.0], 9.0:0.0, 60.0:[80.0, 40.0]} )
	
	assert_not_equal( blur.getLocalHashAtTime(1.0), blur.getLocalHashAtTime(30.0) )
	assert_equal( blur.getLocalHashAtTime(1.0), blur.getLocalHashAtTime(60.0) )


def testFrameVarying():
	g = tuttle.Graph()
	read_stillImage = g.createNode( "tuttle.pngreader", filename="TuttleOFX-data/image/png/Gradient-16bit.png" )
	read_imageSequence = g.createNode( "tuttle.pngreader", filename = "TuttleOFX-data/image/png/Gradient-@bit.png" )

	# Need to call setup to have the node information (like frame varying) in each node
	g.setup()

	assert_not_equal( read_stillImage.getLocalHashAtTime(0.0), read_imageSequence.getLocalHashAtTime(0.0) )

	assert_equal( read_stillImage.getLocalHashAtTime(1.0), read_stillImage.getLocalHashAtTime(60.0) )

	assert_not_equal( read_imageSequence.getLocalHashAtTime(1.0), read_imageSequence.getLocalHashAtTime(60.0) )


def te_stClipOrder():
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,50])
	a_read2 = a_g.createNode("tuttle.checkerboard", size=[50,49])
	a_merge = a_g.createNode("tuttle.merge")
	a_g.connect(a_read1, a_merge.getClip("A"))
	a_g.connect(a_read2, a_merge.getClip("B"))
	
	b_g = tuttle.Graph()
	b_read1 = b_g.createNode("tuttle.checkerboard", size=[50,50])
	b_read2 = b_g.createNode("tuttle.checkerboard", size=[50,49])
	b_merge = b_g.createNode("tuttle.merge")
	b_g.connect(b_read1, b_merge.getClip("A"))
	b_g.connect(b_read2, b_merge.getClip("B"))
	
	assert_equal( a_merge.getLocalHashAtTime(1.0), b_merge.getLocalHashAtTime(1.0) )
	
	c_g = tuttle.Graph()
	c_read1 = c_g.createNode("tuttle.checkerboard", size=[50,50])
	c_read2 = c_g.createNode("tuttle.checkerboard", size=[50,49])
	c_merge = c_g.createNode("tuttle.merge")
	c_g.connect(c_read1, c_merge.getClip("B"))
	c_g.connect(c_read2, c_merge.getClip("A"))
	
	# todo
	#assert_not_equal( b_merge.getLocalHashAtTime(1.0), c_merge.getLocalHashAtTime(1.0) )
