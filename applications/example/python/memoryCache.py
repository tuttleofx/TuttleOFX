from pyTuttle import tuttle

tuttle.core().preload()

g = tuttle.Graph()
read = g.createNode( "tuttle.pngreader", "data/input.png" )
blur = g.createNode( "tuttle.blur", size=[0.003, 0.005] )

g.connect( [read,blur] )

outputCache = tuttle.MemoryCache()
g.compute( outputCache, blur )


print 'blur name:', blur.getName()

imgRes = outputCache.get( blur.getName(), 0 );

print 'imgRes:', dir( imgRes )
print 'MemorySize:', imgRes.get().getMemorySize()

