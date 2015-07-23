from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testClamp_pngWriterImplicitClamp():

	tempFilename = '.tests/clampedImage.png'

	g = tuttle.Graph()
	read1  = g.createNode( "tuttle.pngreader", "data/input.png", bitDepth="32f" )
	mathOp = g.createNode( "tuttle.mathoperator", master=0.6, operation="plus" )
	write  = g.createNode( "tuttle.pngwriter", filename=tempFilename )

	# @todo: re-read the image and compare to a clamped version of the input image
	#clamp = g.createNode( "tuttle.clamp" )
	#read2 = g.createNode( "tuttle.pngwriter", filename=tempFilename )

	#diff = g.createNode( "tuttle.diff" )

	g.connect( [read1, mathOp, write] )
	#g.connect( [mathOp, clamp] )

	#g.connect( clamp, diff.getAttribute("A") )
	#g.connect( read2, diff.getAttribute("B") )

	#g.compute( write )

