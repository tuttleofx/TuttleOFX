from pyTuttle import tuttle

if __name__ == "__main__":
	tuttle.core().preload()

	g = tuttle.Graph()
	read = g.createNode( "tuttle.checkerboard", size=[20,20] )
	blur = g.createNode( "tuttle.blur", size=[0.03, 0.05] )
	write = g.createNode( "tuttle.pngwriter", filename="data/output.png" )

	g.connect( [read,blur,write] )
	g.compute( write )
