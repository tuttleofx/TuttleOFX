from pyTuttle import tuttle

tuttle.core().preload()

g = tuttle.Graph()
read = g.createNode( "tuttle.pngreader", "data/input.png" )
invert = g.createNode( "tuttle.invert" )

g.connect( [read,invert] )

outputCache = tuttle.MemoryCache()
g.compute( outputCache, invert )


print 'blur name:', invert.getName()

imgRes = outputCache.get( invert.getName(), 0 );

print 'type imgRes:', type( imgRes )
print 'imgRes:', dir( imgRes )
print 'MemorySize:', imgRes.getMemorySize()
