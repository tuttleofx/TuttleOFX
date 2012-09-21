from pyTuttle import tuttle

tuttle.core().preload()

g = tuttle.Graph()
read = g.createNode( "tuttle.pngreader", "data/input.png" )
blur = g.createNode( "tuttle.blur", size=[3.0, 5.0] )
write = g.createNode( "tuttle.pngwriter", filename="data/output.png" )

g.connect( [read,blur,write] )
g.compute( write )
