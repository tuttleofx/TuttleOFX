from pyTuttle import tuttle
from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testSimpleProcessGraph():

	graph = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.exrreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = graph.addConnectedNodes(n)

	procOptions = tuttle.ComputeOptions()
	procGraph = tuttle.ProcessGraph(procOptions, graph, [], tuttle.core().getMemoryCache())

	print("before compute")

	outputCache = tuttle.MemoryCache()
	timeRange = tuttle.TimeRange(1, 16, 10)
	print("setup")
	procGraph.setup()
	print("beginSequence")
	procGraph.beginSequence(timeRange)
	for time in range(timeRange._begin, timeRange._end, timeRange._step):
		print("time:", time)
		procGraph.setupAtTime(time)
		procGraph.processAtTime(outputCache, time)
	print("endSequence")
	procGraph.endSequence()

	print("after compute")


def testCustomMemoryCache():

	graph = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.exrreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.oiiowriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = graph.addConnectedNodes(n)

	procOptions = tuttle.ComputeOptions()
	internCache = tuttle.MemoryCache()
	procGraph = tuttle.ProcessGraph(procOptions, graph, [], internCache)

	print("before compute")

	outputCache = tuttle.MemoryCache()
	timeRange = tuttle.TimeRange(1, 16, 10)
	print("setup")
	procGraph.setup()
	print("beginSequence")
	procGraph.beginSequence(timeRange)
	for time in range(timeRange._begin, timeRange._end, timeRange._step):
		print("time:", time)
		procGraph.setupAtTime(time)
		procGraph.processAtTime(outputCache, time)
	print("endSequence")
	procGraph.endSequence()

	print("after compute")

