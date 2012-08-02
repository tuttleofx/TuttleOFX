
from pyTuttle import tuttle
import numpy

tuttle.Core.instance().preload()
g = tuttle.Graph()

ib = g.createInputBuffer()
x = numpy.array([[.9, .1, .9], [.8, .2, .9]], numpy.float32)
ib.setGrayImage( x )

w = g.createNode("tuttle.pngwriter")
w.getParam("filename").setValue("foo.png")

g.connect( ib, w )
g.compute( w )









#a = [[1,2,3,4],[1,2,3,4]]
#ib.setRGBAImage( a )
#a = numpy.array([1,2,3],'f')
#ib.setGrayImage( a )

