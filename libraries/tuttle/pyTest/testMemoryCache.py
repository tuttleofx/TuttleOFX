from pyTuttle.tuttle import *

def setUp():
	core().preload()

def testMemoryCache():

	outputCache = MemoryCache()
	compute(
		outputCache,
		[
			NodeInit( "tuttle.checkerboard", format="PAL" ),
			NodeInit( "tuttle.colortransform", offsetGlobal=.2 ),
			NodeInit( "tuttle.invert" ),
		] )


	#print 'invert name:', invert.getName()

	imgRes = outputCache.get(0);

	print 'type imgRes:', type( imgRes )
	print 'imgRes:', dir( imgRes )
	print 'FullName:', imgRes.getFullName()
	print 'MemorySize:', imgRes.getMemorySize()

	img = imgRes.getNumpyImage()
