# scons: Text Png FFMpeg
import os

from pyTuttle.tuttle import *
from movie import *

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

def testEncodeSequencePng():
	encode( "fps_test_png.avi" , format="avi", videoCodec="png" )

def testEncodeSequenceMpeg2():
	encode( "MPEG-2_I-frame_only_Highest_Quality_Encoding.m2v" , format="mpeg2video", videoCodec="mpeg2video", videoPixelFormat="yuv422p", qmin=1, mpeg2video_intra_vlc=1 )
	# missing parameter qscale=1
	
	encode( "MPEG-2_Long_GOP_Encoding.m2v" , format="mpeg2video", videoCodec="mpeg2video", b=5000000 , g=300, bf=2, b_strategy=1 )
	
def testEncodeSequenceProres():
	encode( "fps_test_mov.mov" , format="mov", videoCodec="prores" )

def testEncodeSequenceMJpeg():
	encode( "fps_test_mjpeg_qmin_7_qmax_1.avi" , format="avi", videoCodec="mjpeg", qmin=7, qmax=1 )

def testEncodeSequenceWebm():
	encode( "fps_test_libvpx_b_1000000.webm" , format="webm", videoCodec="libvpx", b=1000000 )
	
def testEncodeSequenceRawVideo():
	encode( "fps_test_yuvRaw.y4m" , format="yuv4mpegpipe", videoCodec="rawvideo" )

#def testEncodeSequenceH264():
#	encode( "fps_test_x264.mov" , format="mov", videoCodec="libx264" )
#	encode( "fps_test_x264_veryfast.mov" , format="mov", videoCodec="libx264", libx264_preset="veryfast" )
#	encode( "fps_test_x264_slower.mov" , format="mov", videoCodec="libx264", libx264_preset="slower" )
