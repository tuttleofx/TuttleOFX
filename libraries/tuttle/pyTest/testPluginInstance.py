# scons: LensDistort

from pyTuttle import tuttle
import os

def setUp():
	tuttle.core().preload(False)

def testNodeInfos():
	graph = tuttle.Graph()

	print "graph:", graph
	print "graph.__str__():", graph.__str__()

	node = graph.createNode( "tuttle.lensdistort" )
	node = node.asImageEffectNode()

	print "plugin: ", node.getName()
	print "version: ", node.getVersion()
	print "nbParams: ", node.getNbParams()
	print "paramSet: ", node.getParamSet()
	print [p for p in node.getParams()]
	print [p.getName() for p in node.getParams()]

	pref = tuttle.Preferences()
	print dir(pref)
	testPath = pref.buildTuttleTestPathStr()

	filenameSimple = os.path.join(testPath, "testNodeInfos_exportDot_simple.dot")
	filenameDetailed = os.path.join(testPath, "testNodeInfos_exportDot_detailed.dot")
	graph.exportDot(filenameSimple)
	graph.exportDot(filenameDetailed, tuttle.Graph.eDotExportLevelDetailed)
	
	assert os.path.isfile(filenameSimple)
	assert os.path.isfile(filenameDetailed)
	
	print "filenameSimple:", filenameSimple
	print "filenameDetailed:", filenameDetailed
	
	os.remove(filenameSimple)
	os.remove(filenameDetailed)


def testParamInfos():
	graph = tuttle.Graph()
	node = graph.createNode( "tuttle.lensdistort" )
	paramCoef1 = node.getParam("coef1")
	
	print "paramCoef1:", paramCoef1
	
	# AttributeAccessor
	print "Name:", paramCoef1.getName()
	print "ShortLabel:", paramCoef1.getShortLabel()
	print "Label:", paramCoef1.getLabel()
	print "LongLabel:", paramCoef1.getLongLabel()
	
	# ParamAccessor
	print "ParamType:", paramCoef1.getParamTypeName()
	print "ParentName:", paramCoef1.getParentName()
	print "ScriptName:", paramCoef1.getScriptName()
	print "Hint:", paramCoef1.getHint()
	print "DoubleType:", paramCoef1.getDoubleType()
	print "CacheInvalidation:", paramCoef1.getCacheInvalidation()
	print "Enabled:", paramCoef1.getEnabled()
	print "CanUndo:", paramCoef1.getCanUndo()
	print "Secret:", paramCoef1.getSecret()
	print "EvaluateOnChange:", paramCoef1.getEvaluateOnChange()
	print "CanAnimate:", paramCoef1.getCanAnimate()
	
	print "Value A:", paramCoef1.getDoubleValue()
	paramCoef1.setValue(0.4)
	print "Value B:", paramCoef1.getDoubleValue()
	assert paramCoef1.getDoubleValue() == 0.4
	
	# Properties
	print "Default value:", paramCoef1.getProperties().getDoubleProperty("OfxParamPropDefault")
	
	print "dir:", dir(paramCoef1)


