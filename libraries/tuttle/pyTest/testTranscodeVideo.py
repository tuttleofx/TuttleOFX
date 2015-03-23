# scons: pluginAudioVideo pluginInvert

from nose.tools import nottest

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testTranscodeToDefaultCodec():
	tuttle.compute(
		[
			tuttle.NodeInit( "tuttle.avreader", filename="TuttleOFX-data/video/bars_100.avi", v_colorspace=2 ), # video with PAR 16/15=1.066667
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit( "tuttle.avwriter", filename=".tests/testTrancodeToDefaultCodec.mov", v_colorspace=2, v_aspect=[0,1] ),
		], tuttle.ComputeOptions(0,20) )


@nottest
def testTranscodeToMpegpipe():
	tuttle.compute(
		[
			tuttle.NodeInit( "tuttle.avreader", filename="TuttleOFX-data/video/flame.avi", v_colorspace=2 ),
			tuttle.NodeInit( "tuttle.avwriter", filename=".tests/testTrancodeToMpegpipe.y4m", f_format="yuv4mpegpipe", v_codec="rawvideo", v_colorspace=2, v_aspect=[0,1] ),
		], tuttle.ComputeOptions(0,10) )
