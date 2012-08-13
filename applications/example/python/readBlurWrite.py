from pyTuttle import tuttle

tuttle.Core.instance().preload()

g = tuttle.Graph()
read = g.createNode( "tuttle.pngreader" )
blur = g.createNode( "tuttle.blur" )
write = g.createNode( "tuttle.pngwriter" )

read.getParam("filename").setValue( "data/input.png" )
blur.getParam( "size" ).setValue( 3.0 )
write.getParam("filename").setValue( "data/output.png" )

g.connect( [read,blur,write] )
g.compute( write )
