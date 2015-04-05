# scons: pluginCheckerboard pluginColorTransform pluginInvert

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


def testCoreMemories():

        core = tuttle.core()
        memoryCache = core.getMemoryCache()
        memoryPool = core.getMemoryPool()

        # no cache
        assert_equals( memoryCache.size(), 0 )
        assert( memoryCache.empty() )

        # no memory allocated in pool
        assert_equals( memoryPool.getUsedMemorySize(), 0 )
        assert_equals( memoryPool.getAllocatedAndUnusedMemorySize(), 0 )
        assert_equals( memoryPool.getAllocatedMemorySize(), 0 )
        assert_equals( memoryPool.getWastedMemorySize(), 0 )

        # can allocate available memory size - 1
        allocatedMemory = memoryPool.getAvailableMemorySize() - 1
        memoryPool.allocate( allocatedMemory )

        # can't allocate available memory size + 1
        allocatedMemory = memoryPool.getAvailableMemorySize() + 1
        assert_raises( Exception, memoryPool.allocate, allocatedMemory )
