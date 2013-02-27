# scons: FFMpeg Invert

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)

def testTrancodeToDefaultCodec():
	tuttle.compute(
		[
			tuttle.NodeInit( "tuttle.ffmpegreader", filename="TuttleOFX-data/video/bars_100.avi" ), # video with PAR 16/15=1.066667
			tuttle.NodeInit( "tuttle.invert" ),
			tuttle.NodeInit("tuttle.ffmpegwriter", filename=".tests/testTrancodeToDefaultCodec.mov"),
		], tuttle.ComputeOptions(0,20) )


def testTrancodeToMpegpipe():
	tuttle.compute(
		[
			tuttle.NodeInit( "tuttle.ffmpegreader", filename="TuttleOFX-data/video/flame.avi" ),
			tuttle.NodeInit("tuttle.ffmpegwriter", filename=".tests/testTrancodeToMpegpipe.mov", format="yuv4mpegpipe", videoCodec="rawvideo"),
		], tuttle.ComputeOptions(0,10) )

