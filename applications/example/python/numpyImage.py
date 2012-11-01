
from pyTuttle import tuttle
import numpy
import Image

if __name__ == "__main__":
	tuttle.core().preload()
	g = tuttle.Graph()

	img = numpy.asarray(Image.open('data/input.jpg'))
	ii = g.createInputBuffer()
	ii.set3DArrayBuffer( img )

	array = numpy.array([[.9, .1, .9], [.8, .2, .9]], numpy.float32)
	ib = g.createInputBuffer()
	ib.set2DArrayBuffer( array )

	c = g.createNode("tuttle.component", to="rgb")
	m = g.createNode("tuttle.merge", mergingFunction="average", rod="union")
	w = g.createNode("tuttle.pngwriter", "foo.png")

	g.connect( ib.getNode(), c )
	g.connect( c, m.getAttribute("A") )
	g.connect( ii.getNode(), m.getAttribute("B") )
	g.connect( m, w )

	g.compute( w )

	# @todo: fix this example, it doesn't work.
	#        Maybe a bug on the host side.
