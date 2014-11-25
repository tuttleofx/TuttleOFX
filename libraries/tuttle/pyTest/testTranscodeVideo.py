# scons: pluginAudioVideo pluginInvert

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testTranscodeToDefaultCodec():
	tuttle.compute(
		[
			tuttle.NodeInit( "tuttle.avreader", filename="TuttleOFX-data/video/bars_100.avi", colorspace=2 ), # video with PAR 16/15=1.066667
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit("tuttle.avwriter", filename=".tests/testTrancodeToDefaultCodec.mov", colorspace=2, aspect=[0,1] ),
		], tuttle.ComputeOptions(0,20) )


def testTranscodeToMpegpipe():
	tuttle.compute(
		[
			tuttle.NodeInit( "tuttle.avreader", filename="TuttleOFX-data/video/flame.avi", colorspace=2 ),
			tuttle.NodeInit("tuttle.avwriter", filename=".tests/testTrancodeToMpegpipe.y4m", format="yuv4mpegpipe", videoCodec="rawvideo", colorspace=2, aspect=[0,1] ),
		], tuttle.ComputeOptions(0,10) )
