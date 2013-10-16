import os
from pyTuttle.tuttle import *

# generate a sequence test with number_of_frames frames, burned with the frame index
# the sequence will be placed in the documentation directory (DOC_DIR)
# ans the sequence will named: DOC_DIR/images/prefix#####.png
def generate( prefix, number_of_frames ):

	for i in range( number_of_frames ):
		graph = Graph()
		text = graph.createNode( "tuttle.text", vAlign="center", bold=1, color=[1,1,1,1], format="PAL", textSize=50, text=str(i) )
		write = graph.createNode( "tuttle.pngwriter", filename=os.path.join( os.environ["DOC_DIR"], "images", prefix + str(i).zfill(5) + ".png" ) )
		graph.connect( [text, write] )
		graph.compute( write )
		
	for i in range( number_of_frames ):
		graph = Graph()
		text = graph.createNode( "tuttle.text", vAlign="center", bold=1, color=[1,1,1,1], format="HD", textSize=50, text=str(i) )
		write = graph.createNode( "tuttle.pngwriter", filename=os.path.join( os.environ["DOC_DIR"], "images", prefix + "hd_" + str(i).zfill(5) + ".png" ) )
		graph.connect( [text, write] )
		graph.compute( write )
