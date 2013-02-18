# scons: Checkerboard Merge

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
	g.setupAtTime( time, [read1] )
	rodA = read1.getRegionOfDefinition( time )
	assert rodA.x1 == 0
	assert rodA.y1 == 0
	assert rodA.x2 == 20
	assert rodA.y2 == 137


	merge.getParam("offsetB").setValue([int(rodA.x2), 0])
	
	outputCache = tuttle.MemoryCache()
	g.compute( outputCache, merge )

	# We retrieve the output image buffer, and we could read the image size
	rodMerge = outputCache.get(0).getROD()
	
	print rodMerge.x1, rodMerge.y1, rodMerge.x2, rodMerge.y2
	assert rodMerge.x1 == 0
	assert rodMerge.y1 == 0
	assert rodMerge.x2 == (rodA.x2 + 234)
	assert rodMerge.y2 == 357

