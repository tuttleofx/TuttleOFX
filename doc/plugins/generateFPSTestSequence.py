# scons: Text Png FFMpeg Diff
import os

from pyTuttle.tuttle import *
from movie import *

from nose.tools import *

def setUp():
	core().preload( False )

def testFPSGenerator( ):
	generate( "fps_test_", 50 )
	
def encode( outFilename, *orderedParams, **namedParams ):
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_#####.png" ) )
	write = graph.createNode( "tuttle.ffmpegwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", outFilename ), *orderedParams, **namedParams )
	graph.connect( [read, write] )
	graph.compute( write )

def checkVideoProperties( video, fps, frameDuration, pixelAspectRatio ):
	graph = Graph()
	readVideo = graph.createNode( "tuttle.ffmpegreader", filename=os.path.join( os.environ["DOC_DIR"], "videos", video ) ).asImageEffectNode()
	
	graph.setup()
	td = readVideo.getTimeDomain()
	framerate = readVideo.getOutputFrameRate()
	pixelAspectRatio = readVideo.getOutputPixelAspectRatio()

	print "time domain: ", td.min, td.max
	print "framerate: ", framerate
	print "pixel aspect ratio: ", pixelAspectRatio
	
	assert_equal( framerate, fps )
	assert_equal( td.min, 0.0 )
	assert_equal( td.max, frameDuration )
	assert_equal( pixelAspectRatio, pixelAspectRatio )

def check( video, sequence ):
	graph = Graph()
	
	readSequence = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", sequence ) )
	readVideo    = graph.createNode( "tuttle.ffmpegreader", filename=os.path.join( os.environ["DOC_DIR"], "videos", video ) )
	diff         = graph.createNode( "tuttle.diff" ).asImageEffectNode()
	
	graph.connect( readSequence, diff.getClip("SourceA") )
	graph.connect( readVideo, diff.getClip("SourceB") )
	
	graph.setup()
	td = diff.getTimeDomain()

	assert td.min == 0.0
	assert td.max == 49.0

	graph.compute( diff )
	
	param = diff.getParam("quality")
	
	print "diff = ", param.getDoubleValueAtIndex(0), param.getDoubleValueAtIndex(1), param.getDoubleValueAtIndex(2), param.getDoubleValueAtIndex(3)
	
	assert_equal( 0.0, param.getDoubleValueAtIndex(0) )
	assert_equal( 0.0, param.getDoubleValueAtIndex(1) )
	assert_equal( 0.0, param.getDoubleValueAtIndex(2) )
	assert_equal( 0.0, param.getDoubleValueAtIndex(3) )


def testEncodeSequencePng():
	outputFilename = "fps_test_png.avi"
	encode( outputFilename , format="avi", videoCodec="png" )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceMpeg2Intra():
	outputFilename = "MPEG-2_I-frame_only_Highest_Quality_Encoding.m2v"
	encode( outputFilename , format="mpeg2video", videoCodec="mpeg2video", videoPixelFormat="yuv422p", qmin=1, mpeg2video_intra_vlc=1 )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )
	# missing parameter qscale=1

def testEncodeSequenceMpeg2():
	outputFilename = "MPEG-2_Long_GOP_Encoding.m2v"
	encode( outputFilename , format="mpeg2video", videoCodec="mpeg2video", b=5000000 , g=300, bf=2, b_strategy=1 )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )
	
def testEncodeSequenceProres():
	outputFilename = "fps_test_mov.mov"
	encode( outputFilename , format="mov", videoCodec="prores" )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceMJpeg():
	outputFilename = "fps_test_mjpeg_qmin_7_qmax_1.avi"
	encode( outputFilename , format="avi", videoCodec="mjpeg", qmin=7, qmax=1 )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceWebm():
	outputFilename = "fps_test_libvpx_b_1000000.webm"
	encode( outputFilename , format="webm", videoCodec="libvpx", b=1000000 )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )
	
def testEncodeSequenceRawVideo():
	outputFilename = "fps_test_yuvRaw.y4m"
	encode( outputFilename , format="yuv4mpegpipe", videoCodec="rawvideo" )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )

#def testEncodeSequenceH264():
#	encode( "fps_test_x264.mov" , format="mov", videoCodec="libx264" )
#	encode( "fps_test_x264_veryfast.mov" , format="mov", videoCodec="libx264", libx264_preset="veryfast" )
#	encode( "fps_test_x264_slower.mov" , format="mov", videoCodec="libx264", libx264_preset="slower" )
