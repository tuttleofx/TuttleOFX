from pyTuttle import tuttle

core = tuttle.Core.instance()
core.preload()

print [p for p in core.getPluginCache().getPluginPath()]
print [p.getIdentifier() for p in core.getPluginCache().getPlugins()]
