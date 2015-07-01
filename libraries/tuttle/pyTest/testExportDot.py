from pyTuttle import tuttle
import os

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testExportDot():
	graph = tuttle.Graph()

	read1 = graph.createNode( "tuttle.checkerboard", size=[20,137] ).asImageEffectNode()
	read2 = graph.createNode( "tuttle.checkerboard", size=[234,357] ).asImageEffectNode()
	merge = graph.createNode( "tuttle.merge", rod="union" ).asImageEffectNode()
	graph.connect( read1, merge.getClip("A") )
	graph.connect( read2, merge.getClip("B") )
	
	pref = tuttle.Preferences()
	testPath = pref.buildTuttleTestPathStr()

	filenameSimple = os.path.join(testPath, "testExportDot_simple.dot")
	filenameDetailed = os.path.join(testPath, "testExportDot_detailed.dot")
	graph.exportDot(filenameSimple)
	graph.exportDot(filenameDetailed, tuttle.Graph.eDotExportLevelDetailed)
	
	assert os.path.isfile(filenameSimple)
	assert os.path.isfile(filenameDetailed)
	
	print("filenameSimple:", filenameSimple)
	print("filenameDetailed:", filenameDetailed)
	
	os.remove(filenameSimple)
	os.remove(filenameDetailed)

