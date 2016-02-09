from pyTuttle import tuttle


def setUp():
	tuttle.core().preload(False)


def testThreadEnv():
	g = tuttle.Graph()
	pngRead = g.createNode("tuttle.checkerboard", size=[50,50])
	blur = g.createNode('tuttle.invert')
	pngW = g.createNode('tuttle.pngwriter', ".tests/output-###.png")
	g.connect( [pngRead, blur, pngW] )

	env = tuttle.ThreadEnv()
	env.compute( g, pngW )
	env.join()
