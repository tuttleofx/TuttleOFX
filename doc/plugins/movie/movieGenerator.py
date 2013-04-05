import os
from pyTuttle.tuttle import *

def generate( prefix, length ):

	for i in range( length ):
		graph = Graph()
		text = graph.createNode( "tuttle.text", vAlign="center", font="teen", bold=1, color=[1,1,1,1], format="PAL", textSize=50, text=str(i) )
		write = graph.createNode( "tuttle.pngwriter", filename=os.path.join( os.environ["DOC_DIR"], "images", prefix + str(i).zfill(5) + ".png" ) )
		graph.connect( [text, write] )
		graph.compute( write )
