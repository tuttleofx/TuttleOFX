
from pyTuttle import tuttle
import numpy
import Image

tuttle.Core.instance().preload()
g = tuttle.Graph()

ii = g.createInputBuffer()
img = numpy.asarray(Image.open('data/input.jpg'))
ii.set3DArrayBuffer( img )

ib = g.createInputBuffer()
array = numpy.array([[.9, .1, .9], [.8, .2, .9]], numpy.float32)
ib.set2DArrayBuffer( array )

c = g.createNode("tuttle.component")
c.getParam("to").setValue("rgb")

m = g.createNode("tuttle.merge")
m.getParam("mergingFunction").setValue("average")
m.getParam("rod").setValue("union")

w = g.createNode("tuttle.pngwriter")
w.getParam("filename").setValue("foo.png")

g.connect( ib.getNode(), c )
g.connect( c, m.getAttribute("A") )
g.connect( ii.getNode(), m.getAttribute("B") )
g.connect( m, w )

g.compute( w )

# @todo: fix this example, it doesn't work.
#        Maybe a bug on the host side.
