# scons: pluginCheckerboard pluginMerge pluginTimeShift pluginFade

from pyTuttle import tuttle

from nose.tools import *

def setUp():
	tuttle.core().preload(False)

def testSameGraph():
	'''
	If we create 2 identical graphs, their hashes should be the same!
	'''
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,50])
	a_read2 = a_g.createNode("tuttle.checkerboard", size=[50,49])
	a_merge = a_g.createNode("tuttle.merge")
	a_g.connect(a_read1, a_merge.getClip("A"))
	a_g.connect(a_read2, a_merge.getClip("B"))
	
	b_g = tuttle.Graph()
	b_read1 = b_g.createNode("tuttle.checkerboard", size=[50,50])
	b_read2 = b_g.createNode("tuttle.checkerboard", size=[50,49])
	b_merge = b_g.createNode("tuttle.merge")
	b_g.connect(b_read1, b_merge.getClip("A"))
	b_g.connect(b_read2, b_merge.getClip("B"))

	time = 0.0

	aHashMap = tuttle.NodeHashContainer()
	a_g.computeGlobalHashAtTime(aHashMap, time)
	aHash = aHashMap.getHash(a_merge.getName(), time)

	bHashMap = tuttle.NodeHashContainer()
	b_g.computeGlobalHashAtTime(bHashMap, time)
	bHash = bHashMap.getHash(b_merge.getName(), time)
	
	assert_equal( aHash, bHash )


def testClipOrder():
	'''
	The "merge" node has 2 input clips. The order of these input connections
	should modify the hash.

	"Merge.A -> Read1" and "Merge.B -> Read2"
	is NOT the same than
	"Merge.A -> Read2" and "Merge.B -> Read1"
	'''
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,50])
	a_read2 = a_g.createNode("tuttle.checkerboard", size=[50,49])
	a_merge = a_g.createNode("tuttle.merge")
	a_g.connect(a_read1, a_merge.getClip("A"))
	a_g.connect(a_read2, a_merge.getClip("B"))
	
	c_g = tuttle.Graph()
	c_read1 = c_g.createNode("tuttle.checkerboard", size=[50,50])
	c_read2 = c_g.createNode("tuttle.checkerboard", size=[50,49])
	c_merge = c_g.createNode("tuttle.merge")
	c_g.connect(c_read2, c_merge.getClip("A"))
	c_g.connect(c_read1, c_merge.getClip("B"))

	time = 0.0

	aHashMap = tuttle.NodeHashContainer()
	a_g.computeGlobalHashAtTime(aHashMap, time)
	aHash = aHashMap.getHash(a_merge.getName(), time)
	
	cHashMap = tuttle.NodeHashContainer()
	c_g.computeGlobalHashAtTime(cHashMap, time)
	cHash = cHashMap.getHash(c_merge.getName(), time)

	assert_not_equal( aHash, cHash )

def testClipOrderWithSameInputs():
	'''
	The "merge" node has 2 input clips. The order of these input connections
	modify the hash only if these input are different.

	"Merge.A -> Read1" and "Merge.B -> Read2"
	is the same than
	"Merge.A -> Read2" and "Merge.B -> Read1"
	ONLY IF Read1 is the same than Read2
	'''
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,72])
	a_read2 = a_g.createNode("tuttle.checkerboard", size=[50,72])
	a_merge = a_g.createNode("tuttle.merge")
	a_g.connect(a_read1, a_merge.getClip("A"))
	a_g.connect(a_read2, a_merge.getClip("B"))
	
	b_g = tuttle.Graph()
	b_read1 = b_g.createNode("tuttle.checkerboard", size=[50,72])
	b_read2 = b_g.createNode("tuttle.checkerboard", size=[50,72])
	b_merge = b_g.createNode("tuttle.merge")
	b_g.connect(b_read2, b_merge.getClip("A"))
	b_g.connect(b_read1, b_merge.getClip("B"))

	time = 0.0

	aHashMap = tuttle.NodeHashContainer()
	a_g.computeGlobalHashAtTime(aHashMap, time)
	aHash = aHashMap.getHash(a_merge.getName(), time)

	bHashMap = tuttle.NodeHashContainer()
	b_g.computeGlobalHashAtTime(bHashMap, time)
	bHash = bHashMap.getHash(b_merge.getName(), time)
	
	assert_equal( aHash, bHash )


def testNotValidGraphRaise():
	'''
	To compute the hash we need to run setupAtTime first (we need to know if
	the node isFrameVaying, etc.).
	"setupAtTime()" raises if the graph is not valid, like a non optional clip
	is unconnected. And there is no sense to compute the hash of a node which
	is not valid.
	
	@todo: The computeGlobalHashAtTime should not throw, and it should compute
	the hash of all valid nodes.
	'''
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,50])
	a_read2 = a_g.createNode("tuttle.checkerboard", size=[50,49])
	a_merge = a_g.createNode("tuttle.merge")
	a_g.connect(a_read1, a_merge.getClip("A"))
	
	b_g = tuttle.Graph()
	b_read1 = b_g.createNode("tuttle.checkerboard", size=[50,50])
	b_read2 = b_g.createNode("tuttle.checkerboard", size=[50,49])
	b_merge = b_g.createNode("tuttle.merge")
	b_g.connect(b_read2, b_merge.getClip("B"))

	time = 0.0

	aHashMap = tuttle.NodeHashContainer()
	assert_raises( Exception, a_g.computeGlobalHashAtTime, aHashMap, time)

	bHashMap = tuttle.NodeHashContainer()
	assert_raises( Exception, b_g.computeGlobalHashAtTime, bHashMap, time)


def testNotTheSameClipIsUnconnected():
	'''
	A "fade" node with 2 optional input clips should have different hashes
	depending on which input is unconnected.
	'''
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,50])
	a_lensdistort = a_g.createNode("tuttle.fade")
	a_g.connect(a_read1, a_lensdistort.getClip("SourceFrom"))
	# a_lensdistort.getClip("SourceTo") is unconnected

	b_g = tuttle.Graph()
	b_read1 = b_g.createNode("tuttle.checkerboard", size=[50,50])
	b_lensdistort = b_g.createNode("tuttle.fade")
	b_g.connect(b_read1, b_lensdistort.getClip("SourceTo"))
	# b_lensdistort.getClip("SourceFrom") is unconnected
	
	time = 0.0

	aHashMap = tuttle.NodeHashContainer()
	a_g.computeGlobalHashAtTime(aHashMap, time)
	aHash = aHashMap.getHash(a_lensdistort.getName(), time)

	bHashMap = tuttle.NodeHashContainer()
	b_g.computeGlobalHashAtTime(bHashMap, time)
	bHash = bHashMap.getHash(b_lensdistort.getName(), time)

	assert_not_equal( aHash, bHash )


def testIdentityNode():
	a_g = tuttle.Graph()
	a_read1 = a_g.createNode("tuttle.checkerboard", size=[50,50])
	a_timeshift = a_g.createNode("tuttle.timeshift", offset=3)
	a_invert = a_g.createNode("tuttle.invert")
	a_g.connect([a_read1, a_timeshift, a_invert])
	
	time = 0.0

	aHashMap = tuttle.NodeHashContainer()
	a_g.computeGlobalHashAtTime(aHashMap, time)
	aHash = aHashMap.getHash(a_invert.getName(), time)


	# The Timshift node itself is an identity node,
	# and it only modify the timing of nodes.
	# So if the input doesn't change over time, it doesn't modify the hash
	a_timeshift.getParam("offset").setValue(0)

	bHashMap = tuttle.NodeHashContainer()
	a_g.computeGlobalHashAtTime(bHashMap, time)
	bHash = bHashMap.getHash(a_invert.getName(), time)

	assert_equal( aHash, bHash )

