
from pyTuttle import tuttle
import numpy
import Image

if __name__ == "__main__":

	tuttle.core().preload()

	g = tuttle.Graph()

	ib = g.createInputBuffer()
	# numpy array from an image
	img = numpy.asarray(Image.open('data/input.jpg'))
	ib.set3DArrayBuffer( img )
	# generated numpy array
	#x = numpy.array([[.9, .1, .9], [.8, .2, .9]], numpy.float32)
	#ib.set2DArrayBuffer( x )

	w = g.createNode("tuttle.pngwriter", filename="foo.png")

	g.connect( ib.getNode(), w )
	g.compute( w )
