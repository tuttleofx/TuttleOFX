# scons: Text Png FFMpeg
import os

from pyTuttle.tuttle import *
from movie import *

def setUp():
	core().preload( False )

def testFPSGenerator():
	generate( "fps_test_", 50 )
	
def testEncodeSequencePng():
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_#####.png" ) )
	write = graph.createNode( "tuttle.ffmpegwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", "fps_test_png.avi" ), format="avi", videoCodec="png" )
	graph.connect( [read, write] )
	graph.compute( write )

def testEncodeSequenceProres():
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_#####.png" ) )
	write = graph.createNode( "tuttle.ffmpegwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", "fps_test.mov" ), format="mov", videoCodec="prores" )
	graph.connect( [read, write] )
	graph.compute( write )

def testEncodeSequenceMJpeg():
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_#####.png" ) )
	write = graph.createNode( "tuttle.ffmpegwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", "fps_test_mjpeg.avi" ), format="avi", videoCodec="mjpeg", qmin=7, qmax=1 )
	graph.connect( [read, write] )
	graph.compute( write )
	
def testEncodeSequenceWebm():
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_#####.png" ) )
	write = graph.createNode( "tuttle.ffmpegwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", "fps_test_mjpeg.webm" ), format="webm", videoCodec="libvpx", b=1000000 )
	graph.connect( [read, write] )
	graph.compute( write )
