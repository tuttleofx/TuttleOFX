# scons: pluginLensDistort pluginAudioVideo

from pyTuttle import tuttle
import os

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testNodeInfos():
	graph = tuttle.Graph()

	print("graph:", graph)
	print("graph.__str__():", graph.__str__())

	node = graph.createNode( "tuttle.lensdistort" )
	node = node.asImageEffectNode()

	print("plugin:", node.getName())
	print("version:", node.getVersion())
	print("nbParams:", node.getNbParams())
	print("paramSet:", node.getParamSet())
	print([p for p in node.getParams()])
	print([p.getName() for p in node.getParams()])

	grouping = node.getProperties().fetchProperty("OfxImageEffectPluginPropGrouping").getStringValue()
	print("group:", grouping.split('/'))


def testParamInfos():
	graph = tuttle.Graph()
	node = graph.createNode( "tuttle.lensdistort" )
	
	## Double param
	print("-- Double param --")
	doubleParam = node.getParam("coef1")
	
	print("doubleParam:", doubleParam)
	
	# AttributeAccessor
	print("Name:", doubleParam.getName())
	print("ShortLabel:", doubleParam.getShortLabel())
	print("Label:", doubleParam.getLabel())
	print("LongLabel:", doubleParam.getLongLabel())
	
	# ParamAccessor
	print("ParamType:", doubleParam.getParamTypeName())
	print("ParentName:", doubleParam.getParentName())
	print("ScriptName:", doubleParam.getScriptName())
	print("Hint:", doubleParam.getHint())
	print("DoubleType:", doubleParam.getDoubleType())
	print("CacheInvalidation:", doubleParam.getCacheInvalidation())
	print("Enabled:", doubleParam.getEnabled())
	print("CanUndo:", doubleParam.getCanUndo())
	print("Secret:", doubleParam.getSecret())
	print("EvaluateOnChange:", doubleParam.getEvaluateOnChange())
	print("CanAnimate:", doubleParam.getCanAnimate())
	
	print("Value A:", doubleParam.getDoubleValue())
	doubleParam.setValue(0.4)
	print("Value B:", doubleParam.getDoubleValue())
	assert doubleParam.getDoubleValue() == 0.4
	
	# Properties
	print("Default value:", doubleParam.getProperties().getDoubleProperty("OfxParamPropDefault"))

	print("dir:", dir(doubleParam))


	## Double2D param
	print("-- Double2D param --")
	double2dParam = node.getParam("gridCenter")
	print(double2dParam)
	
	print("Value A:", double2dParam.getDoubleValueAtIndex(1))
	double2dParam.setValueAtIndex(1, 0.4)
	print("Value B:", double2dParam.getDoubleValueAtIndex(1))
	assert double2dParam.getDoubleValueAtIndex(1) == 0.4

	print("Default value: [", double2dParam.getProperties().getDoubleProperty("OfxParamPropDefault", 0), ", ", double2dParam.getProperties().getDoubleProperty("OfxParamPropDefault", 1), "]")
	double2dParamDefaultProp = double2dParam.getProperties().fetchDoubleProperty("OfxParamPropDefault")
	print("Default value: [", double2dParamDefaultProp.getValue(0), ", ", double2dParamDefaultProp.getValue(1), "]")


	## Boolean param
	print("-- Boolean param --")
	boolParam = node.getParam( "gridOverlay")
	print(boolParam)

	print("Value A:", boolParam.getBoolValue())
	boolParam.setValue(True)
	print("Value B:", boolParam.getBoolValue())
	assert boolParam.getBoolValue() == True
	
	print("Default value:", boolParam.getProperties().getIntProperty("OfxParamPropDefault"))
	boolParamDefaultProp = boolParam.getProperties().fetchIntProperty("OfxParamPropDefault")
	print("Default value:", boolParamDefaultProp.getValue())

	print("Hint:", boolParam.getHint())
	# low level equivalent function, based on OpenFX properties.
	print("Hint:", boolParam.getProperties().getStringProperty("OfxParamPropHint"))

	## Clips
	print("nbClips:", node.getClipImageSet().getNbClips())
	assert node.getClipImageSet().getNbClips() == 3

	clips = node.getClipImageSet().getClips()
	
	print("clips:", clips)
	for clip in clips:
		print("-----")
		print("dir(clip):", dir(clips))
		print("clip name:", clip.getName())

		print("clip isOutput:", clip.isOutput())
		print("clip isOptional:", clip.isOptional())
		print("clip temporalAccess:", clip.temporalAccess())
		print("clip isMask:", clip.isMask())
		print("clip supportsTiles:", clip.supportsTiles())
			
		print("clip getBitDepthString:", clip.getBitDepthString())
		print("clip getBitDepthMemorySize:", clip.getBitDepthMemorySize())
		print("clip getPixelAspectRatio:", clip.getPixelAspectRatio())
		print("clip getComponentsString:", clip.getComponentsString())
		print("clip getNbComponents:", clip.getNbComponents())
		print("clip getFrameRate:", clip.getFrameRate())
		print("clip getFrameRange:", clip.getFrameRange())


def testNodeComputeInfos():
	graph = tuttle.Graph()
	
	node = graph.createNode( "tuttle.avreader", filename="TuttleOFX-data/video/bars_100.avi", colorspace="bt709" ).asImageEffectNode()

	graph.setup()
	td = node.getTimeDomain()
	print("node timeDomain:", td.min, td.max)
	assert td.min == 0.0
	assert td.max == 100.0
	# Duration is 101, the last frame is included
	assert_equal((td.max-td.min)+1, 101.0)
	
	framerate = node.getOutputFrameRate()
	print("framerate:", framerate)
	assert_equal(framerate, 25.0)
	
	pixelAspectRatio = node.getOutputPixelAspectRatio()
	print("pixel aspect ratio:", pixelAspectRatio)
	assert_almost_equal(pixelAspectRatio, 16.0/15.0)
	
	# modify input SAR
	node.getParam("customSAR").setValue(2.0)
	graph.setup()
	
	pixelAspectRatio = node.getOutputPixelAspectRatio()
	print("pixel aspect ratio:", pixelAspectRatio)
	assert_equal(pixelAspectRatio, 2.0)


def testPushButton():
	graph = tuttle.Graph()
	
	node = graph.createNode( "tuttle.avwriter", filename=".tests/plop.avi", colorspace="bt709" )
	node = node.asImageEffectNode()

	render = node.getParam("render")
	# the user click on the PushButton
	render.paramChanged(tuttle.eChangeUserEdited)

