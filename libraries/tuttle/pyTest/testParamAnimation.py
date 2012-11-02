from pyTuttle import tuttle

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

	g.connect( [read,blur,write] )
	g.compute( write, tuttle.ComputeOptions( 9, 9, 3 ) )
