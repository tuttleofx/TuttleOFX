# scons: Text Png AudioVideo Diff
import os

from pyTuttle.tuttle import *
from movie import *

from nose.tools import *

def setUp():
	core().preload( False )

def testFPSGenerator( ):
	generate( "fps_test_", 50 )
	
def encodePAL( outFilename, *orderedParams, **namedParams ):
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_#####.png" ) )
	write = graph.createNode( "tuttle.avwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", outFilename ), *orderedParams, **namedParams )
	graph.connect( [read, write] )
	graph.compute( write )

def encodeHD( outFilename, *orderedParams, **namedParams ):
	graph = Graph()
	
	read = graph.createNode( "tuttle.pngreader", filename=os.path.join( os.environ["DOC_DIR"], "images", "fps_test_hd_#####.png" ) )
	write = graph.createNode( "tuttle.avwriter", filename=os.path.join( os.environ["DOC_DIR"], "videos", outFilename ), *orderedParams, **namedParams )
	graph.connect( [read, write] )
	graph.compute( write )

def checkVideoProperties( video, fps, frameDuration, pixelAspectRatio ):
	graph = Graph()
	readVideo = graph.createNode( "tuttle.avreader", filename=os.path.join( os.environ["DOC_DIR"], "videos", video ) ).asImageEffectNode()
	
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
	readVideo    = graph.createNode( "tuttle.avreader", filename=os.path.join( os.environ["DOC_DIR"], "videos", video ), colorspace="bt709" )
	diff         = graph.createNode( "tuttle.diff" ).asImageEffectNode()
	
	graph.connect( readSequence, diff.getClip("SourceA") )
	graph.connect( readVideo, diff.getClip("SourceB") )
	
	graph.setup()
	#td = diff.getTimeDomain()

	#assert td.min == 0.0
	#assert td.max == 49.0

	graph.compute( diff )
	
	param = diff.getParam("quality")
	
	print "diff = ", param.getDoubleValueAtIndex(0), param.getDoubleValueAtIndex(1), param.getDoubleValueAtIndex(2), param.getDoubleValueAtIndex(3)
	
	assert_equal( 0.0, param.getDoubleValueAtIndex(0) )
	assert_equal( 0.0, param.getDoubleValueAtIndex(1) )
	assert_equal( 0.0, param.getDoubleValueAtIndex(2) )
	assert_equal( 0.0, param.getDoubleValueAtIndex(3) )


def testEncodeSequencePng():
	outputFilename = "fps_test_png.avi"
	encodePAL( outputFilename , format="avi", videoCodec="png", colorspace="bt709", aspect=[0,1] )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )


def testEncodeSequencePng24Fps():
	outputFilename = "fps_test_png_24fps.avi"
	encodePAL( outputFilename , format="avi", videoCodec="png", colorspace="bt709", aspect=[0,1], useCustomFps=1, customFps=24 )
	checkVideoProperties( outputFilename, 24.0, 99.0, 1.0 )
	# can't check differents fps sequences.
	#check( outputFilename, "fps_test_#####.png" )

def testEncodeSequencePng29_97Fps():
	outputFilename = "fps_test_png_29_97fps.avi"
	encodePAL( outputFilename , format="avi", videoCodec="png", colorspace="bt709", aspect=[0,1], useCustomFps=1, customFps=29.97 )
	checkVideoProperties( outputFilename, 29.97002997002997, 5000.0, 1.0 )
	# can't check differents fps sequences.
	#check( outputFilename, "fps_test_#####.png" )

def testEncodeSequencePng12Fps():
	outputFilename = "fps_test_png_12fps.avi"
	encodePAL( outputFilename , format="avi", videoCodec="png", colorspace="bt709", aspect=[0,1], useCustomFps=1, customFps=12 )
	checkVideoProperties( outputFilename, 12.0, 5149.0, 1.0 )
	# can't check differents fps sequences.
	#check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceMpeg2Intra(): 
	outputFilename = "MPEG-2_I-frame_only_Highest_Quality_Encoding.m2v"
	encodePAL( outputFilename , format="mpeg2video", videoCodec="mpeg2video", videoPixelFormat="yuv422p", qmin=1, mpeg2video_intra_vlc=1, colorspace="bt709", aspect=[0,1] )
	checkVideoProperties( outputFilename, 25.0, (1 << 29) - 1, 1.0 )
	#check( outputFilename, "fps_test_#####.png" )
	# missing parameter qscale=1

def testEncodeSequenceMpeg2():
	outputFilename = "MPEG-2_Long_GOP_Encoding.m2v"
	encodePAL( outputFilename , format="mpeg2video", videoCodec="mpeg2video", b=5000000 , g=300, bf=2, b_strategy=1, colorspace="bt709", aspect=[0,1] )
	checkVideoProperties( outputFilename, 25.0, (1 << 29) - 1, 1.0 )
	#check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceDNxHD():
	outputFilename = "fps_test_DNxHD.mov"
	encodeHD( outputFilename , format="mov", videoCodec="dnxhd", b=36000000, colorspace="bt709", aspect=[0,1] )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceProres():
	outputFilename = "fps_test_mov.mov"
	encodePAL( outputFilename , format="mov", videoCodec="prores", colorspace="bt709", profile=1, aspect=[0,1] )
	checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	check( outputFilename, "fps_test_#####.png" )

def testEncodeSequenceMJpeg():
	outputFilename = "fps_test_mjpeg_qmin_7_qmax_1.avi"
	encodePAL( outputFilename , format="avi", videoCodec="mjpeg", qmin=7, qmax=1, colorspace="bt709", aspect=[0,1] )
	#checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
	#check( outputFilename, "fps_test_#####.png" )

#def testEncodeSequenceWebm():
#	outputFilename = "fps_test_libvpx_b_1000000.webm"
#	encodePAL( outputFilename , format="webm", videoCodec="libvpx", b=1000000, colorspace="bt709", aspect=[0,1] )
#	#checkVideoProperties( outputFilename, 25.0, 49.0, 1.0 )
#	check( outputFilename, "fps_test_#####.png" )
	
def testEncodeSequenceRawVideo():
	outputFilename = "fps_test_yuvRaw.y4m"
	encodePAL( outputFilename , format="yuv4mpegpipe", videoCodec="rawvideo", colorspace="bt709", aspect=[0,1] )
	checkVideoProperties( outputFilename, 25.0, (1 << 29) - 1, 1.0 )
	#check( outputFilename, "fps_test_#####.png" )


#def testEncodeSequenceH264():
#	encodePAL( "fps_test_x264.mov" , format="mov", videoCodec="libx264" )
#	encodePAL( "fps_test_x264_veryfast.mov" , format="mov", videoCodec="libx264", libx264_preset="veryfast" )
#	encodePAL( "fps_test_x264_slower.mov" , format="mov", videoCodec="libx264", libx264_preset="slower" )

#def testEncodeSequenceH264():
#	encodePAL( "fps_test_x264.mov" ,
#		format="mov",
#		videoCodec="libx264",
#		colorspace="bt709",
#		aspect=[0,1],
#		profile=1,
#		videoPixelFormat="yuv420p",
#		strict="experimental",
#		maxrate="10000000",
#		bufsize="10000000",
#		b=1200000,
#		libx264_qp=10,
#		libx264_x264-params='crop=0:0:0:0'


#		format="mov",
#		#flags_mov_faststart=True,
#		videoCodec="libx264",
#		colorspace="bt709",
#		aspect=[0,1],
#		profile=1,
#		videoPixelFormat="yuv420p",
#		strict="experimental",
#		maxrate=10000000,
#		bufsize=10000000,
#		b=1200000,
#		libx264_profile="high",
#		libx264_preset="slow"
#		)


