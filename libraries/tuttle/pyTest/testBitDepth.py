# scons: Checkerboard Blur AudioVideo Png

from pyTuttle.tuttle import *
from nose.tools import *

def setUp():
	core().preload(False)


@raises(Exception)
def testIncompatibleBitDepth():
	filepath = NamedTemporaryFile( preffix="bitDepthTest-", suffix=".png" )
	compute( [
		NodeInit( "tuttle.checkerboard", size=[50,50] ),
		NodeInit( "tuttle.blur", size=[0.03, 0.05] ), # only supports 32f
		NodeInit( "tuttle.swscale", height=500 ), # only supports 8ui
		NodeInit( "tuttle.pngwriter", filename=filepath.name ),
		] )
