from pyTuttle import tuttle

from nose.tools import *

def setUp():
	tuttle.core().preload()

def testDoubleParamAnimation():
	g = tuttle.Graph()
	read = g.createNode( "tuttle.checkerboard", size=[50,50] )
	blur = g.createNode( "tuttle.blur", size={1.0:[80.0, 40.0], 9.0:0.0} )
	write = g.createNode( "tuttle.pngwriter", filename=".tests/output_####.png" )

	# Read arbitrary blur values, to see the interpolation
	s = blur.getParam("size")
	for i in xrange(0, 10*5):
		t = i/5.0 # step is 0.2
		print "blur size at ", t, ":", \
		    "(", s.getDoubleValueAtTimeAndIndex(t, 0), ", ", \
		         s.getDoubleValueAtTimeAndIndex(t, 1), ")"
		assert( s.getDoubleValueAtTimeAndIndex(t, 0) <= 80. )
		assert( s.getDoubleValueAtTimeAndIndex(t, 0) >=  0. )
		assert( s.getDoubleValueAtTimeAndIndex(t, 1) <= 40. )
		assert( s.getDoubleValueAtTimeAndIndex(t, 1) >=  0. )

	g.connect( [read,blur,write] )
	#g.compute( write, tuttle.ComputeOptions( 9, 9, 3 ) )


def testInt2DParam():
	g = tuttle.Graph()
	constant = g.createNode( "tuttle.constant", size=[50,50] )

	# set Int2D param values
	s = constant.getParam("size")
	s.setValue( [1, 2] )
	s.setValue( {1.0:[80, 40], 9.0:0} )
	assert_equal( 80, s.getIntValueAtTimeAndIndex(1., 0) )
	assert_equal( 40, s.getIntValueAtTimeAndIndex(1., 1) )
	assert_equal( 0, s.getIntValueAtTimeAndIndex(9., 0) )
	assert_equal( 0, s.getIntValueAtTimeAndIndex(9., 1) )

	# not supported:
	#s.setValue( [{1.:80, 2.:60}, 40] )
	#s.setValue( {1.:80, 2.:60}, 40 )

def testRGBAParam():
	g = tuttle.Graph()

	# Set RGB values to an RGBA parameter (alpha set to 1 by default)
	constant = g.createNode( "tuttle.constant", color=[.5, .5, .5] )
	assert_equal( 1., constant.getParam("color").getDoubleValueAtIndex(3) )
	
	# Set RGBA values
	constant = g.createNode( "tuttle.constant",color=[.5, .5, .5, .5] )
	assert_equal( .5, constant.getParam("color").getDoubleValueAtIndex(3) )

	c = constant.getParam("color")
	
	# set Gray
	c.setValue(1.)
	assert_equal( 1., c.getDoubleValueAtIndex(1) )
	# set RGB
	c.setValue([1., 0.5, 0.5])
	assert_equal( .5, c.getDoubleValueAtIndex(1) )
	assert_equal( 1., c.getDoubleValueAtIndex(3) )
	# set RGBA
	c.setValue([1., 0.5, 0.5, .8])
	assert_equal( .5, c.getDoubleValueAtIndex(1) )
	assert_equal( .8, c.getDoubleValueAtIndex(3) )

	# at index
	c.setValueAtIndex(0, .5)
	c.setValueAtIndex(1, .1)
	c.setValueAtIndex(2, .1)
	c.setValueAtIndex(3, 1.)

	# at time
	c.setValueAtTime(0., .5)
	c.setValueAtTime(12., .1)
	c.setValueAtTime(25., .1)
	c.setValueAtTime(13., 1.)

	# at time and index
	c.setValueAtTimeAndIndex( 0., 1, .5)
	c.setValueAtTimeAndIndex(12., 2, .1)
	c.setValueAtTimeAndIndex(25., 1, .1)
	c.setValueAtTimeAndIndex(13., 2, 1.)

	assert_equals( .5, c.getDoubleValueAtTimeAndIndex( 0., 1) )
	assert_equals( .1, c.getDoubleValueAtTimeAndIndex(12., 2) )
