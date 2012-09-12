from pyTuttle import tuttle

tuttle.core().preload()

print [p for p in tuttle.core().getPluginCache().getPluginPath()]
print [p.getIdentifier() for p in tuttle.core().getPluginCache().getPlugins()]

