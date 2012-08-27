
from pyTuttle import tuttle
import numpy

tuttle.Core.instance().preload()
g = tuttle.Graph()

ib = g.createInputBuffer()
# numpy array from an image
img = numpy.asarray(Image.open('data/input.jpg'))
ii.set2DArrayBuffer( img )
# generated numpy array
#x = numpy.array([[.9, .1, .9], [.8, .2, .9]], numpy.float32)
#ib.set2DArrayBuffer( x )

w = g.createNode("tuttle.pngwriter")
w.getParam("filename").setValue("foo.png")

g.connect( ib.node(), w )
g.compute( w )

