from pyTuttle import tuttle

if __name__ == "__main__":

	tuttle.core().preload()

	g = tuttle.Graph()
	pngRead = g.createNode('tuttle.pngreader', "data/input-###.png")
	blur = g.createNode('tuttle.blur')
	pngW = g.createNode('tuttle.pngwriter', "data/output-###.png")
	g.connect( [pngRead, blur, pngW] )

	env = tuttle.ThreadEnv()
	env.compute( g, pngW )
	env.join()
