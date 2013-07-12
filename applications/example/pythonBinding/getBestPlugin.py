#!/usr/bin/python
from pyTuttle.tuttle import *

def setUp():
	core().preload(False)

def getBestIOPlugin( extension, context ):
	pluginCache = core().getPluginCache()
	result = ""
	extSizeForPlugin = 9999
	for p in pluginCache.getPlugins():
		#print p.getIdentifier() + " (v" + str( p.getVersionMajor() ) + "." + str( p.getVersionMinor() ) + ")"
		g = Graph()
		n = g.createNode( p.getIdentifier() ).asImageEffectNode()

		if n.getContext() == context:
			#print p.getIdentifier()
			extSize = n.getProperties().getDimension( "TuttleOfxImageEffectPropSupportedExtensions" )
			for extIndex in range( extSize ):
				curExt = n.getProperties().getStringProperty( "TuttleOfxImageEffectPropSupportedExtensions", extIndex )
				if extension == curExt or extension == "." + curExt :
					#print p.getIdentifier(), extSize, extSizeForPlugin
					if extSize < extSizeForPlugin:
						result = p.getIdentifier()
						extSizeForPlugin = extSize
					break;
	#print result
	return result

def getBestReader( extension ):
	return getBestIOPlugin( extension, "OfxImageEffectContextReader" )

def getBestWriter( extension ):
	return getBestIOPlugin( extension, "OfxImageEffectContextWriter" )
