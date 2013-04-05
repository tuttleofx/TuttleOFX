import os
from pyTuttle.tuttle import *
#os.environ['DOC_DIR']='dist'
#from generators import *
#core().preload( False )
#from pickle import dumps, loads

def generate( pluginName, *orderedParams, **namedParams ):
	filename = pluginName + '_' + '_'.join(['%s_%s' % (key, value) for (key, value) in namedParams.items()]).replace(' ', '_')
	
	cmdLine = pluginName + ' ' + ' '.join(['%s=%s' % (key, value) for (key, value) in namedParams.items()])

	graph = Graph()
	generator = graph.createNode( pluginName, *orderedParams, **namedParams )
	text = graph.createNode( "tuttle.text", vAlign="center", font="teen", bold=1, color=[0.5, 0.5, 0.5, 1.0], text=cmdLine )
	write = graph.createNode( "tuttle.pngwriter", filename=os.path.join( os.environ["DOC_DIR"], "images", filename + ".png" ) )
	graph.connect( [generator, text, write] )
	graph.compute( write )
	#print "######################################################################################"
	#print pluginName + '_'.join(['%s_%s' % (key, value) for (key, value) in namedParams.items()]).replace(' ', '_')
	#print a.join( '_' )
	#print str(*orderedParams)
	#print str(*namedParams)
	#print "######################################################################################"


def generateEXR( pluginName, *orderedParams, **namedParams ):
	filename = pluginName + '_' + '_'.join(['%s_%s' % (key, value) for (key, value) in namedParams.items()]).replace(' ', '_')
	
	cmdLine = pluginName + ' ' + ' '.join(['%s=%s' % (key, value) for (key, value) in namedParams.items()])

	graph = Graph()
	generator = graph.createNode( pluginName, *orderedParams, **namedParams )
	text = graph.createNode( "tuttle.text", vAlign="center", font="teen", bold=1, color=[0.5, 0.5, 0.5, 1.0], text=cmdLine )
	write = graph.createNode( "tuttle.exrwriter", filename=os.path.join( os.environ["DOC_DIR"], "images", filename + ".exr" ) )
	graph.connect( [generator, text, write] )
	graph.compute( write )
	
