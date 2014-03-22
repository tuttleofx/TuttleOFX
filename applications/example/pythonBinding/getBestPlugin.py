#!/usr/bin/python
from pyTuttle import tuttle
import sys


def getBestIOPlugin(extension, context):
	"""
	Returns the node identifier that supports the given file extension,
	for a specific plugin context.
	"""
	result = None
	# If multiple plugins support the same extension,
	# we keep the plugin which declares less supported extensions.
	# So we suppose that this plugin is more specialized...
	# This is a simple but non optimal trick.
	minExtensionsOfPlugin = sys.maxsize
	
	# Normalize the input extension, so the searchExtension should be something like "jpg".
	searchExtension = extension if not extension.startswith(".") else extension[1:]
	searchExtension = searchExtension.lower()
	
	for plugin in tuttle.core().getImageEffectPluginCache().getPlugins():
		try:
			plugin.loadAndDescribeActions()
			desc = plugin.getDescriptor()
			if plugin.supportsContext(context):
				# print(plugin.getIdentifier() + " (v" + str( plugin.getVersionMajor() ) + "." + str( plugin.getVersionMinor() ) + ")")
				supportedExtensionsProp = desc.getProperties().fetchStringProperty("TuttleOfxImageEffectPropSupportedExtensions")
				supportedExtensionsSize = supportedExtensionsProp.getDimension()
				if supportedExtensionsSize >= minExtensionsOfPlugin:
					break
				supportedExtensions = supportedExtensionsProp.getValues()
				for ext in supportedExtensions:
					# print(plugin.getIdentifier(), ext, supportedExtensionsSize, minExtensionsOfPlugin)
					if ext == searchExtension:
						result = plugin.getIdentifier()
						minExtensionsOfPlugin = supportedExtensionsSize
						break
		except Exception:
			# The creation of the node could failed, if not fully supported
			pass
	if not result:
		raise ValueError("File extension '%s' not found" % extension)
	return result

def getBestReader(extension):
	"""
	Returns the reader node identifier that supports the given file extension.
	"""
	return getBestIOPlugin(extension, "OfxImageEffectContextReader")

def getBestWriter(extension):
	"""
	Returns the writer node identifier that supports the given file extension.
	"""
	return getBestIOPlugin(extension, "OfxImageEffectContextWriter")


