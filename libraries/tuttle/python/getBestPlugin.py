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


def getFileExtension(filename):
	(shortname, extension) = os.path.splitext(filename)

	if not extension:
		if os.path.sep in filename:
			raise ValueError("Filename has no extension: '%s'." % filename)

		# If filename is not a filename but just an extension...
		return filename
	return extension


def _getReaders(extension):
	"""
	Returns the reader nodes identifiers that supports the given file extension.
	:extension: <str> - argument is an extension.
	"""
	return getIOPluginsForExtension(extension, "OfxImageEffectContextReader")


def getReaders(filename):
	"""
	Returns the reader nodes identifiers that supports the given file extension.
	:filename: <str> - argument can be a filename or an extension.
	"""
	return _getReaders(getFileExtension(filename))


def getBestReader(filename):
	"""
	Returns the reader node identifier that supports the extension of the given file.
	:filename: <str> - argument can be a filename or an extension.
	"""
	extension = getFileExtension(filename)

	if not os.path.exists(filename):
		readersId = _getReaders(extension)
		if not readersId:
			raise ValueError("Unknown file extension '%s'." % extension)
		return readersId[0]

	else:
		readersId = _getReaders(extension)
		for readerId in readersId:
			try:
				graph = tuttle.Graph()
				readerIn = graph.createNode(readerId, filename=filename).asImageEffectNode()
				graph.setup()
				timeRange = readerIn.getTimeDomain()
				graph.setupAtTime(timeRange.min)
				readerIn.getRegionOfDefinition(timeRange.min)
				return readerId
			except Exception as exception:
				# Ignore the exception, if we can't load this file with this reader plugin,
				# we will try with the other ones.
				pass
		raise ValueError(
			("Can't read image '%s'.\n"
			 "Readers tried: %s") % (filename, str(readersId)))

	assert(False)


def _getWriters(extension):
	"""
	Returns the writer nodes identifiers that supports the given file extension.
	:extension: <str> - argument is an extension.
	"""
	return getIOPluginsForExtension(extension, "OfxImageEffectContextWriter")


def getWriters(filename):
	"""
	Returns the writer nodes identifiers that supports the given file extension.
	:filename: <str> - argument can be a filename or an extension.
	"""
	return _getWriters(getFileExtension(filename))


def getBestWriter(filename):
	"""
	Returns the writer node identifier that supports the given file extension.
	:filename: <str> - argument can be a filename or an extension.
	"""
	extension = getFileExtension(filename)

	results = _getWriters(extension)
	if not results:
		raise ValueError("Unknown file extension '%s'." % extension)
	return results[0]

