from pyTuttle import tuttle

tuttle.core().preload()

g = tuttle.Graph()
read = g.createNode( "tuttle.pngreader" )
blur = g.createNode( "tuttle.blur" )
write = g.createNode( "tuttle.pngwriter" )

read.getParam("filename").setValue( "data/input.png" )
blur.getParam( "size" ).setValueAtTime( 1.0, 80.0, 40.0 )
blur.getParam( "size" ).setValueAtTime( 5.0, 0.0, 0.0 )

s = blur.getParam("size")
for i in xrange(0, 6*5):
    t = i/5.0 # step is 0.2
    print "blur size at ", t, ":", \
        "(", s.getDoubleValueAtTimeAndIndex(t, 0), ", ", \
             s.getDoubleValueAtTimeAndIndex(t, 1), ")"

write.getParam("filename").setValue( "data/output_####.png" )

g.connect( [read,blur,write] )
g.compute( write, tuttle.ComputeOptions( 1, 5 ) )
