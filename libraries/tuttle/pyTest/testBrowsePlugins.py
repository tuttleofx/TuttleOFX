# scons: all

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testBrowsePlugins():
	pluginCache = tuttle.core().getPluginCache()

	print [p for p in pluginCache.getPluginPath()]
	print [p.getIdentifier() for p in pluginCache.getPlugins()]

def testBrowseIEPlugins():
	pluginCache = tuttle.core().getImageEffectPluginCache()

	print [p.getDescriptor().getShortLabel() for p in pluginCache.getPlugins()]
	print [p.getDescriptor().getLabel() for p in pluginCache.getPlugins()]
	print [p.getDescriptor().getLongLabel() for p in pluginCache.getPlugins()]


