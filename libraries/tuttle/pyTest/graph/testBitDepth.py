from pyTuttle import tuttle
import tempfile

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testCompatibleBitDepth():
	filepath = tempfile.NamedTemporaryFile( prefix="bitDepthTest-", suffix=".png" )
	tuttle.compute( [
		tuttle.NodeInit( "tuttle.checkerboard", size=[50,50] ),
		tuttle.NodeInit( "tuttle.swscale", height=500 ), # only supports 8ui
		tuttle.NodeInit( "tuttle.pngwriter", filename=filepath.name ),
		] )


@raises(Exception)
def testIncompatibleBitDepth():
	filepath = tempfile.NamedTemporaryFile( prefix="bitDepthTest-", suffix=".png" )
	tuttle.compute( [
		tuttle.NodeInit( "tuttle.checkerboard", size=[50,50] ),
		tuttle.NodeInit( "tuttle.blur", size=[0.03, 0.05] ), # only supports 32f
		tuttle.NodeInit( "tuttle.swscale", height=500 ), # only supports 8ui
		tuttle.NodeInit( "tuttle.pngwriter", filename=filepath.name ),
		] )
