from pyTuttle import tuttle

tuttle.core().preload()

g = tuttle.Graph()
pngRead = g.createNode('tuttle.pngreader')
pngRead.getParam("filename").setValue("data/input-###.png")
blur = g.createNode('tuttle.blur')
pngW = g.createNode('tuttle.pngwriter')
pngW.getParam("filename").setValue("data/output-###.png")
g.connect( [pngRead, blur, pngW] )

env = tuttle.ThreadEnv()
env.compute( g, tuttle.NodeListArg(pngW) )
env.join()
