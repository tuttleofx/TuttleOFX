# scons: all

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testBrowsePlugins():
	pluginCache = tuttle.core().getPluginCache()

	print [p for p in pluginCache.getPluginPath()]
	print [p.getIdentifier() for p in pluginCache.getPlugins()]
