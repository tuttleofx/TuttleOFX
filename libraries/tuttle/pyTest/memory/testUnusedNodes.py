from pyTuttle import tuttle

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testUnusedNodes():
	"""
	It should not have any impact, if there are invalid but
	unused nodes in the graph.
	Nodes could be invalid if they have unconnected mandatory clips.

	And the other hand, if you ask to compute those invalid nodes,
	it should raise an exception.
	"""

	g = tuttle.Graph()
	read1 = g.createNode("tuttle.checkerboard", size=[20,137]).asImageEffectNode()
	read2 = g.createNode("tuttle.checkerboard", size=[234,357]).asImageEffectNode()
	merge = g.createNode("tuttle.merge", rod="union").asImageEffectNode()

	# Unconnected but unused nodes should not impede the computation
	pushpixel_unused = g.createNode("tuttle.pushpixel").asImageEffectNode()
	invert_unused = g.createNode("tuttle.invert").asImageEffectNode()

	g.connect(read1, merge.getClip("A"))
	g.connect(read2, merge.getClip("B"))

	hashMap = tuttle.NodeHashContainer()
	g.computeGlobalHashAtTime(hashMap, 0, [merge])

	## TODO: Should not raise, even if there are invalid nodes.
	# g.computeGlobalHashAtTime(hashMap, 0)

	outputCache = tuttle.MemoryCache()
	g.compute(outputCache, merge)
	
	# Trying to compute all nodes should raise an error
	# on unconnected filters (invert and pushpixel).
	assert_raises(Exception, g.compute, outputCache)

