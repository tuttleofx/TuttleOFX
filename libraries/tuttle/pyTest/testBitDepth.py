from pyTuttle import tuttle
import tempfile

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testIncompatibleBitDepth():
	filepath = tempfile.NamedTemporaryFile( prefix="bitDepthTest-", suffix=".png" )
	tuttle.compute( [
		NodeInit( "tuttle.checkerboard", size=[50,50] ),
		NodeInit( "tuttle.swscale", height=500 ), # only supports 8ui
		NodeInit( "tuttle.pngwriter", filename=filepath.name ),
		] )


@raises(Exception)
def testIncompatibleBitDepth():
	filepath = tempfile.NamedTemporaryFile( prefix="bitDepthTest-", suffix=".png" )
	tuttle.compute( [
		NodeInit( "tuttle.checkerboard", size=[50,50] ),
		NodeInit( "tuttle.blur", size=[0.03, 0.05] ), # only supports 32f
		NodeInit( "tuttle.swscale", height=500 ), # only supports 8ui
		NodeInit( "tuttle.pngwriter", filename=filepath.name ),
		] )
