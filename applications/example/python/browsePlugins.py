from pyTuttle import tuttle

tuttle.core().preload()

pluginCache = tuttle.core().getPluginCache()

print [p for p in pluginCache.getPluginPath()]
print [p.getIdentifier() for p in pluginCache.getPlugins()]

