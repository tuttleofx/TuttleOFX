# scons: pluginExr pluginInvert pluginGamma pluginJpeg

from pyTuttle import tuttle
from nose.tools import *

def setUp():
	tuttle.core().preload(False)


def testCompute():

	graph = tuttle.Graph()
	n = [
			tuttle.NodeInit( "tuttle.exrreader", filename="TuttleOFX-data/image/openexr/DisplayWindow/t##.exr" ),
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.gamma", master=.5 ),
			tuttle.NodeInit( "tuttle.jpegwriter", filename=".tests/fromExr/output-####.jpg" ),
		]
	nodes = graph.addConnectedNodes(n)

	procOptions = tuttle.ComputeOptions()
	procGraph = tuttle.ProcessGraph(procOptions, graph, [])

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

