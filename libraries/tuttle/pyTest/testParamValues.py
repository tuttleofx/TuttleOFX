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
	node = g.createNode( "tuttle.constant", size=[50,50] )

	# set Int2D param values
	s = node.getParam("size")
	s.setValue( [1, 2] )

	# set values at times
	s.setValue( {1.0:[80, 40], 9.0:0} )
	assert_equal( 80, s.getIntValueAtTimeAndIndex(1., 0) )
	assert_equal( 40, s.getIntValueAtTimeAndIndex(1., 1) )
	assert_equal( 0, s.getIntValueAtTimeAndIndex(9., 0) )
	assert_equal( 0, s.getIntValueAtTimeAndIndex(9., 1) )

	# set values at time per item
	s.setValue( [{1.:80, 2.:60}, 40] )
	assert_equal( 80, s.getIntValueAtTimeAndIndex(1., 0) )
	assert_equal( 60, s.getIntValueAtTimeAndIndex(2., 0) )
	#assert_equal( 60, s.getIntValueAtTimeAndIndex(3., 0) )
	assert_equal( 40, s.getIntValueAtIndex(1) )
	assert_equal( 40, s.getIntValueAtTimeAndIndex(1., 1) )
	assert_equal( 40, s.getIntValueAtTimeAndIndex(2., 1) )
	assert_equal( 40, s.getIntValueAtTimeAndIndex(3., 1) )


def testDoubleParam():
	g = tuttle.Graph()
	node = g.createNode( "tuttle.gamma", master=12.34 )

	# set Double2D param values
	s = node.getParam("master")

	# raise, not a multidim param
	#assert_equal( 12.34, s.getDoubleValueAtIndex(0) )

	assert_equal( 12.34, s.getDoubleValue() )

	# Simple set value
	s.setValue( 1.456 )
	assert_equal( 1.456, s.getDoubleValue() )


def testDouble2DParam():
	g = tuttle.Graph()
	node = g.createNode( "tuttle.blur", size=[12.34,0.5] )

	# set Double2D param values
	s = node.getParam("size")

	assert_equal( 12.34, s.getDoubleValueAtIndex(0) )
	assert_equal(   0.5, s.getDoubleValueAtIndex(1) )

	# Simple set value
	s.setValue( [1.5, 25.0] )
	assert_equal( 1.5, s.getDoubleValueAtIndex(0) )
	assert_equal( 25., s.getDoubleValueAtIndex(1) )
	# check return the same value at all times
	assert_equal( 1.5, s.getDoubleValueAtTimeAndIndex(1., 0) )
	assert_equal( 25., s.getDoubleValueAtTimeAndIndex(1., 1) )

	# Set at time
	s.setValue( {1.0:[10.5, 40.], 9.0:0.} )
	assert_equal( 10.5, s.getDoubleValueAtTimeAndIndex(1., 0) )
	assert_equal( 40., s.getDoubleValueAtTimeAndIndex(1., 1) )
	assert_equal( 0., s.getDoubleValueAtTimeAndIndex(9., 0) )
	assert_equal( 0., s.getDoubleValueAtTimeAndIndex(9., 1) )

	# Set integers to double (auto conversion)
	s.setValue( [1, 25] )
	assert_equal(  1., s.getDoubleValueAtIndex(0) )
	assert_equal( 25., s.getDoubleValueAtIndex(1) )
	# check return the same value at all times
	assert_equal(  1., s.getDoubleValueAtTimeAndIndex(1., 0) )
	assert_equal( 25., s.getDoubleValueAtTimeAndIndex(1., 1) )
	
	# mix double and integers
	s.setValue( [1.345, 25] )
	assert_equal(  1.345, s.getDoubleValueAtIndex(0) )
	assert_equal( 25., s.getDoubleValueAtIndex(1) )
	

def testDouble3DParam():
	g = tuttle.Graph()
	node = g.createNode( "tuttle.imagestatistics", outputAverageHSL=[1.1,2.2,3.3] )

	# set Double3D param values
	s = node.getParam("outputAverageHSL")

	assert_equal( 1.1, s.getDoubleValueAtIndex(0) )
	assert_equal( 2.2, s.getDoubleValueAtIndex(1) )
	assert_equal( 3.3, s.getDoubleValueAtIndex(2) )

	# Simple set value
	s.setValue( [1.23, 4.56, 7.89] )
	assert_equal( 1.23, s.getDoubleValueAtIndex(0) )
	assert_equal( 4.56, s.getDoubleValueAtIndex(1) )
	assert_equal( 7.89, s.getDoubleValueAtIndex(2) )
	# check return the same value at all times
	assert_equal( 1.23, s.getDoubleValueAtTimeAndIndex(1., 0) )
	assert_equal( 4.56, s.getDoubleValueAtTimeAndIndex(123., 1) )
	assert_equal( 7.89, s.getDoubleValueAtTimeAndIndex(0., 2) )

	# Set at time
	s.setValue( {1.0:[10.5, 40., 23.4], 9.0:0.} )
	assert_equal( 10.5, s.getDoubleValueAtTimeAndIndex(1., 0) )
	assert_equal( 40.0, s.getDoubleValueAtTimeAndIndex(1., 1) )
	assert_equal( 23.4, s.getDoubleValueAtTimeAndIndex(1., 2) )
	assert_equal(  0.0, s.getDoubleValueAtTimeAndIndex(9., 0) )
	assert_equal(  0.0, s.getDoubleValueAtTimeAndIndex(9., 1) )
	assert_equal(  0.0, s.getDoubleValueAtTimeAndIndex(9., 2) )

	# Set integers to double (auto conversion)
	s.setValue( [1, 25, 3] )
	assert_equal(  1., s.getDoubleValueAtIndex(0) )
	assert_equal( 25., s.getDoubleValueAtIndex(1) )
	assert_equal(  3., s.getDoubleValueAtIndex(2) )

	# Set integer or double to double (auto conversion)
	s.setValue( [1.123, 25, 3.58] )
	assert_equal(  1.123, s.getDoubleValueAtIndex(0) )
	assert_equal( 25., s.getDoubleValueAtIndex(1) )
	assert_equal(  3.58, s.getDoubleValueAtIndex(2) )
	# check return the same value at all times
	assert_equal(  1.123, s.getDoubleValueAtTimeAndIndex(1., 0) )
	assert_equal( 25., s.getDoubleValueAtTimeAndIndex(1., 1) )
	assert_equal(  3.58, s.getDoubleValueAtTimeAndIndex(1., 2) )

def testRGBParam():
	g = tuttle.Graph()

	# Set RGB values to an RGBA parameter (alpha set to 1 by default)
	node = g.createNode( "tuttle.pinning", overlayInColor=[.5, .5, .5] )
	assert_equal( .5, node.getParam("overlayInColor").getDoubleValueAtIndex(1) )
	
#	@raises(Exception)
#	def test_setRGBA_on_RGB_param():
#		# Set RGBA values on an RGB parameter
#		node = g.createNode( "tuttle.pinning", overlayInColor=[.5, .5, .5, .5] )

	c = node.getParam("overlayInColor")
	
	# set same value to all channels
	c.setValue(1.)
	assert_equal( 1., c.getDoubleValueAtIndex(1) )
	# set RGB
	c.setValue([1., 0.5, 0.5])
	assert_equal( 1., c.getDoubleValueAtIndex(0) )
	assert_equal( .5, c.getDoubleValueAtIndex(1) )
	assert_equal( .5, c.getDoubleValueAtIndex(2) )

	# at index
	c.setValueAtIndex(0, .5)
	c.setValueAtIndex(1, .1)
	c.setValueAtIndex(2, .1)

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


def testRGBAParam():
	g = tuttle.Graph()

	# Set RGBA param values
	constant = g.createNode( "tuttle.constant", color=[.5, .5, .5, 1] )
	assert_equal( .5, constant.getParam("color").getDoubleValueAtIndex(0) )
	assert_equal( 1., constant.getParam("color").getDoubleValueAtIndex(3) )
	
	# Set RGBA values
	constant = g.createNode( "tuttle.constant", color=[.5, .5, .5, .5] )
	assert_equal( .5, constant.getParam("color").getDoubleValueAtIndex(3) )

	c = constant.getParam("color")
	
	# set same value to all channels
	c.setValue(1.)
	assert_equal( 1., c.getDoubleValueAtIndex(1) )
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

