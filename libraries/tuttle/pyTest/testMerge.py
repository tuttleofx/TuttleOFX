# scons: Checkerboard Invert Merge FFMpeg

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testMergeWithDynamicOffset():

	g = tuttle.Graph()
	read1 = g.createNode( "tuttle.checkerboard", size=[20,137] ).asImageEffectNode()
	read2 = g.createNode( "tuttle.checkerboard", size=[234,357] ).asImageEffectNode()
	merge = g.createNode( "tuttle.merge", rod="union" ).asImageEffectNode()

	g.connect( read1, merge.getClip("A") )
	g.connect( read2, merge.getClip("B") )
	
	time = 1.0

	# After the setup at time we could read the image size on the node
	g.setupAtTime( time, [read1, read2] )
	rodA = read1.getRegionOfDefinition( time )
	rodB = read2.getRegionOfDefinition( time )
	assert rodA.x1 == 0
	assert rodA.y1 == 0
	assert rodA.x2 == 20
	assert rodA.y2 == 137
	
	assert rodB.x1 == 0
	assert rodB.y1 == 0
	assert rodB.x2 == 234
	assert rodB.y2 == 357


	merge.getParam("offsetB").setValue([int(rodA.x2), 0])
	
	outputCache = tuttle.MemoryCache()
	g.compute( outputCache, merge )

	# We retrieve the output image buffer, and we could read the image size
	rodMerge = outputCache.get(0).getROD()
	
	print rodMerge.x1, rodMerge.y1, rodMerge.x2, rodMerge.y2
	assert rodMerge.x1 == 0
	assert rodMerge.y1 == 0
	assert rodMerge.x2 == ( rodA.x2 + rodB.x2 )
	assert rodMerge.y2 == rodB.y2


def testMergeWithSameChildrens():

	g = tuttle.Graph()

	read = g.createNode("tuttle.checkerboard", size=[100,200], explicitConversion="8i")
	scale = g.createNode("tuttle.swscale", width=150)
	effect = g.createNode("tuttle.invert")
	merge = g.createNode("tuttle.merge", offsetA=[150, 0], mergingFunction="copy", rod="union")
	write = g.createNode("tuttle.invert")

	# read -> scale -> effect
	#               \         \
	#                -----------> merge -> write

	g.connect([read, scale, effect])
	g.connect(scale, merge.getAttribute("B"))
	g.connect(effect, merge.getAttribute("A"))
	g.connect(merge, write)
	
	outputCache = tuttle.MemoryCache()
	g.compute( outputCache, write, tuttle.ComputeOptions(5))
	
	# We retrieve the output image buffer, and we could read the image size
	rodMerge = outputCache.get(0).getROD()
	
	print rodMerge.x1, rodMerge.y1, rodMerge.x2, rodMerge.y2
	assert rodMerge.x1 == 0
	assert rodMerge.y1 == 0
	assert rodMerge.x2 == 300
	assert rodMerge.y2 == 300

