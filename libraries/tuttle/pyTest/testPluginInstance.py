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
	
	## Double param
	print "-- Double param --"
	doubleParam = node.getParam("coef1")
	
	print "doubleParam:", doubleParam
	
	# AttributeAccessor
	print "Name:", doubleParam.getName()
	print "ShortLabel:", doubleParam.getShortLabel()
	print "Label:", doubleParam.getLabel()
	print "LongLabel:", doubleParam.getLongLabel()
	
	# ParamAccessor
	print "ParamType:", doubleParam.getParamTypeName()
	print "ParentName:", doubleParam.getParentName()
	print "ScriptName:", doubleParam.getScriptName()
	print "Hint:", doubleParam.getHint()
	print "DoubleType:", doubleParam.getDoubleType()
	print "CacheInvalidation:", doubleParam.getCacheInvalidation()
	print "Enabled:", doubleParam.getEnabled()
	print "CanUndo:", doubleParam.getCanUndo()
	print "Secret:", doubleParam.getSecret()
	print "EvaluateOnChange:", doubleParam.getEvaluateOnChange()
	print "CanAnimate:", doubleParam.getCanAnimate()
	
	print "Value A:", doubleParam.getDoubleValue()
	doubleParam.setValue(0.4)
	print "Value B:", doubleParam.getDoubleValue()
	assert doubleParam.getDoubleValue() == 0.4
	
	# Properties
	print "Default value:", doubleParam.getProperties().getDoubleProperty("OfxParamPropDefault")

	print "dir:", dir(doubleParam)


	## Double2D param
	print "-- Double2D param --"
	double2dParam = node.getParam("gridCenter")
	print double2dParam
	
	print "Value A:", double2dParam.getDoubleValueAtIndex(1)
	double2dParam.setValueAtIndex(1, 0.4)
	print "Value B:", double2dParam.getDoubleValueAtIndex(1)
	assert double2dParam.getDoubleValueAtIndex(1) == 0.4

	print "Default value: [", double2dParam.getProperties().getDoubleProperty("OfxParamPropDefault", 0), ", ", double2dParam.getProperties().getDoubleProperty("OfxParamPropDefault", 1), "]"
	double2dParamDefaultProp = double2dParam.getProperties().fetchDoubleProperty("OfxParamPropDefault")
	print "Default value: [", double2dParamDefaultProp.getValue(0), ", ", double2dParamDefaultProp.getValue(1), "]"


	## Boolean param
	print "-- Boolean param --"
	boolParam = node.getParam( "gridOverlay")
	print boolParam

	print "Value A:", boolParam.getBoolValue()
	boolParam.setValue(True)
	print "Value B:", boolParam.getBoolValue()
	assert boolParam.getBoolValue() == True
	
	print "Default value:", boolParam.getProperties().getIntProperty("OfxParamPropDefault")
	boolParamDefaultProp = boolParam.getProperties().fetchIntProperty("OfxParamPropDefault")
	print "Default value:", boolParamDefaultProp.getValue()

	print "Hint:", boolParam.getHint()
	# low level equivalent function, based on OpenFX properties.
	print "Hint:", boolParam.getProperties().getStringProperty("OfxParamPropHint")

