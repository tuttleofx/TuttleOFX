#!/usr/bin/python
from pyTuttle import tuttle
import sys
import os


def getIOPluginsForExtension(extension, context):
	"""
	Returns the node identifier that has the best evaluation among nodes which supports the given file extension,
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
				evaluation = desc.getProperties().getDoubleProperty( "TuttleOfxImageEffectPropEvaluation" )
				supportedExtensionsProp = desc.getProperties().fetchStringProperty("TuttleOfxImageEffectPropSupportedExtensions")
				supportedExtensionsSize = supportedExtensionsProp.getDimension()
				supportedExtensions = supportedExtensionsProp.getValues()
				if searchExtension in supportedExtensions:
					# print(plugin.getIdentifier(), ext, supportedExtensionsSize)
					results.append((-evaluation, supportedExtensionsSize, plugin.getIdentifier()))
		except Exception:
			# The creation of the node could failed, if not fully supported
			pass
	# sort by the evaluation value.
	results.sort()
	return [v[2] for v in results]


def getBestReader(inputFile):
	"""
	Returns a reader node identifier that supports the given file extension.
	inputFile argument can be a filename or an extension.
	"""
	results = []

	(shortname, extension) = os.path.splitext(inputFile)

	# inputFile is an extension
	if not extension:
		extension = inputFile
		
	results = getIOPluginsForExtension(extension, "OfxImageEffectContextReader")

	if not results:
		raise ValueError("File extension '%s' not found" % extension)
	return results[0]


def getReaders(extension):
	"""
	Returns the reader nodes identifiers that supports the given file extension.
	"""
	return getIOPluginsForExtension(extension, "OfxImageEffectContextReader")


def getReader(filename):
	"""
	Returns the reader node identifier that supports the extension of the given file.
	"""
	(shortname, extension) = os.path.splitext(filename)
	readersId = getReaders(extension)

	for readerId in readersId:
		try:
			graph = tuttle.Graph()
			readerIn = graph.createNode(readerId, filename=filename).asImageEffectNode()
			graph.setup()
			timeRange = readerIn.getTimeDomain() #timeRange contains min and max time
			graph.setupAtTime(timeRange.min)
			readerIn.getRegionOfDefinition(timeRange.min)
			return readerId
		except Exception as exception:
			raise #exception
	
	raise IOError("Can't read image %s" % filename)


def getBestWriter(extension):
	"""
	Returns the writer node identifier that supports the given file extension.
	"""
	results = getIOPluginsForExtension(extension, "OfxImageEffectContextWriter")[0]
	if not results:
		raise ValueError("File extension '%s' not found" % extension)
	return results[0]


def getWriters(extension):
	"""
	Returns the writer nodes identifiers that supports the given file extension.
	"""
	return getIOPluginsForExtension(extension, "OfxImageEffectContextWriter")

