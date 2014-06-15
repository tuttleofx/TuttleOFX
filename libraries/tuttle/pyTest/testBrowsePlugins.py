# scons: all

from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testBrowsePlugins():
	pluginCache = tuttle.core().getPluginCache()

	pluginPath = pluginCache.getPluginPath()
	print([p for p in pluginPath])
#	print([p for p in pluginCache.getPluginPath()])  # BUG binding: TODO
	print([p.getIdentifier() for p in pluginCache.getPlugins()])


def testBrowseIEPlugins():
	pluginCache = tuttle.core().getImageEffectPluginCache()

	print([p.getDescriptor().getShortLabel() for p in pluginCache.getPlugins()])
	print([p.getDescriptor().getLabel() for p in pluginCache.getPlugins()])
	print([p.getDescriptor().getLongLabel() for p in pluginCache.getPlugins()])
	print([p.getDescriptor().getPluginGrouping() for p in pluginCache.getPlugins()])
