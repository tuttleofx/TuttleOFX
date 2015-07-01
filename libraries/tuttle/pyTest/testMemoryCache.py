from pyTuttle import tuttle

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testOutputMemoryCache():

	outputCache = tuttle.MemoryCache()
	tuttle.compute(
		outputCache,
		[
			tuttle.NodeInit( "tuttle.checkerboard", format="PAL", explicitConversion="8i" ),
			tuttle.NodeInit( "tuttle.colortransform", offsetGlobal=.2 ),
			tuttle.NodeInit( "tuttle.invert" ),
		] )


	#print 'invert name:', invert.getName()

	imgRes = outputCache.get(0);

	print('type imgRes:', type( imgRes ))
	print('imgRes:', dir( imgRes ))
	print('FullName:', imgRes.getFullName())
	print('MemorySize:', imgRes.getMemorySize())

	img = imgRes.getNumpyImage()
