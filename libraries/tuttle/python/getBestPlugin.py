#!/usr/bin/python
from pyTuttle import tuttle
import sys


def getIOPluginsForExtension(extension, context):
	"""
	Returns the node identifier that supports the given file extension,
	for a specific plugin context.
	"""
	results = []

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
				supportedExtensions = supportedExtensionsProp.getValues()
				if searchExtension in supportedExtensions:
					# print(plugin.getIdentifier(), ext, supportedExtensionsSize)
					results.append((supportedExtensionsSize, plugin.getIdentifier()))
		except Exception:
			# The creation of the node could failed, if not fully supported
			pass
	# sort by the number of supported extensions.
	results.sort()
	return [v[1] for v in results]


def getBestReader(extension):
	"""
	Returns the reader node identifier that supports the given file extension.
	"""
	results = getIOPluginsForExtension(extension, "OfxImageEffectContextReader")
	if not results:
		raise ValueError("File extension '%s' not found" % extension)
	return results[0]


def getBestReaders(extension):
	"""
	Returns the reader nodes identifiers that supports the given file extension.
	"""
	return getIOPluginsForExtension(extension, "OfxImageEffectContextReader")


def getBestWriter(extension):
	"""
	Returns the writer node identifier that supports the given file extension.
	"""
	results = getIOPluginsForExtension(extension, "OfxImageEffectContextWriter")
	if not results:
		raise ValueError("File extension '%s' not found" % extension)
	return results[0]


def getBestWriters(extension):
	"""
	Returns the writer nodes identifiers that supports the given file extension.
	"""
	return getIOPluginsForExtension(extension, "OfxImageEffectContextWriter")

