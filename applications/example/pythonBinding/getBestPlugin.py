#!/usr/bin/python
from pyTuttle import tuttle
import sys


def getBestIOPlugin(extension, context):
	result = None
	extSizeForPlugin = sys.maxsize
	test_extension = extension if not extension.startswith(".") else extension[1:]
	tuttle.core().preload(False)
	# print("test_extension: '%s'" % test_extension)
	# print("tuttle.core().getPlugins(): '%s'" % str(tuttle.core().getPlugins()))
	# print("tuttle.core().getPluginCache().getPlugins(): '%s'" % str(tuttle.core().getPluginCache().getPlugins()))
	for p in tuttle.core().getPlugins():
		# print(p.getIdentifier() + " (v" + str( p.getVersionMajor() ) + "." + str( p.getVersionMinor() ) + ")")
		g = tuttle.Graph()
		try:
			n = g.createNode(p.getIdentifier()).asImageEffectNode()

			if n.getContext() == context:
				# print(p.getIdentifier())
				extSize = n.getProperties().getDimension("TuttleOfxImageEffectPropSupportedExtensions")
				if extSize >= extSizeForPlugin:
					break
				for extIndex in range(extSize):
					curExt = n.getProperties().getStringProperty("TuttleOfxImageEffectPropSupportedExtensions", extIndex)
					# print(p.getIdentifier(), curExt, extSize, extSizeForPlugin)
					if test_extension == curExt:
						result = p.getIdentifier()
						extSizeForPlugin = extSize
						break
		except Exception:
			# The creation of the node could failed, if not fully supported
			pass
	if not result:
		raise ValueError("File extension '%s' not found" % extension)
	return result

def getBestReader(extension):
	return getBestIOPlugin(extension, "OfxImageEffectContextReader")

def getBestWriter(extension):
	return getBestIOPlugin(extension, "OfxImageEffectContextWriter")


