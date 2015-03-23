# scons: pluginCheckerboard

from pyTuttle import tuttle

from nose.tools import *

def setUp():
	tuttle.core().preload(False)


def testDoubleParamProps():
	print("testDoubleParamProps")
	g = tuttle.Graph()
	print("--")
	checkerNode = g.createNode( "tuttle.checkerboard", size=[50,50] )
	sizeParam = checkerNode.getParam("size")
	ratioParam = checkerNode.getParam("ratio")
	print("--")
	print("-- Node props:", checkerNode.getProperties().getSize())
	print("-- Node props len:", len(checkerNode.getProperties()))
	print("-- Node props:", checkerNode.getProperties().getLocalSize())
	assert_less(checkerNode.getProperties().getLocalSize(), checkerNode.getProperties().getSize())
	for i, p in enumerate(checkerNode.getProperties()):
		v = p.getStringValues()
		for vv in v:
			print(vv)
		print(i, p.getName(), p.getType())
		print(i, p.getName(), p.getType(), [str(vv) for vv in v])
		print(i, p.getName(), p.getType(), p.getStringValueAt(0) if p.getDimension() else None)
	
	print("--")
	print("-- Param props:")
	s = checkerNode.getParam("size")
	for p in s.getProperties():
		v = p.getStringValues()
		print(p.getName(), p.getType(), [str(vv) for vv in v])
		print(p.getName(), p.getType(), p.getStringValues())
	print("--")


def testParamGroups():
	print("testParamGroups")
	g = tuttle.Graph()
	print("--")
	blurNode = g.createNode("tuttle.blur")
	assert_equals(blurNode.getNbParams(), 5)
	assert_equals(blurNode.getNbChildParams(), 3)
	assert_equals(len(blurNode.getParams()), 5)
	assert_equals(len(blurNode.getChildParams()), 3)
	print("blurNode:", blurNode)
	groupParam = blurNode.getParam("advanced")
	print("childrens:", groupParam.getChildParams())
	assert_equals(groupParam.getNbChildParams(), 2)
	assert_equals(groupParam.getNbParams(), 0)  # the group doesn't constains parameters
	
	checkerNode = g.createNode("tuttle.checkerboard")
	assert_equals(checkerNode.getNbChildParams(), 13)
	assert_equals(checkerNode.getNbParams(), 13)
	print("--")


# setUp()
# testDoubleParamProps()
# testParamGroups()

