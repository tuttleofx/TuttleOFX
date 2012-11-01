from pyTuttle import tuttle

if __name__ == "__main__":
	tuttle.core().preload()

	pluginCache = tuttle.core().getPluginCache()

	print [p for p in pluginCache.getPluginPath()]
	print [p.getIdentifier() for p in pluginCache.getPlugins()]
