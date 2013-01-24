# scons: all

from pyTuttle import tuttle
import os

def setUp():
	tuttle.core().preload(False)

def testPluginInfos():
	g = tuttle.Graph()

	print "graph:", g
	print "graph.__str__():", g.__str__()

	p = g.createNode( "tuttle.lensdistort" )
	node = p.asImageEffectNode()

	print "plugin: ", node.getName()
	print "version: ", node.getVersion()
	print "nbParams: ", node.getNbParams()
	print "paramSet: ", node.getParamSet()
	print [p for p in node.getParams()]
	print [p.getName() for p in node.getParams()]

	pref = tuttle.Preferences()
	print dir(pref)
	testPath = pref.buildTuttleTestPathStr()

	filenameSimple = os.path.join(testPath, "testPluginInfos_exportDot_simple.dot")
	filenameDetailed = os.path.join(testPath, "testPluginInfos_exportDot_detailed.dot")
	g.exportDot(filenameSimple)
	g.exportDot(filenameDetailed, tuttle.Graph.eDotExportLevelDetailed)
	
	assert os.path.isfile(filenameSimple)
	assert os.path.isfile(filenameDetailed)
	
	print "filenameSimple:", filenameSimple
	print "filenameDetailed:", filenameDetailed
	
	os.remove(filenameSimple)
	os.remove(filenameDetailed)

