from pyTuttle import tuttle

core = tuttle.Core.instance()
core.preload()

pluginCache = core.getPluginCache()
print pluginCache

# TODO
#pluginsPaths = pluginCache.getPluginPath()
#print pluginsPaths

#plugins = pluginCache.getPlugins()
#print plugins
