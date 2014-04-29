# scons: pluginTurboJpeg pluginJpeg pluginOpenImageIO pluginImageMagick
from pyTuttle import tuttle
import getBestPlugin

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testGetBestReader():
	assert_equals("tuttle.turbojpegreader", getBestPlugin.getBestReader("../eia1956-small.jpg"))
	assert_equals("tuttle.turbojpegreader", getBestPlugin.getBestReader(".jpg"))
	assert_equals("tuttle.turbojpegreader", getBestPlugin.getBestReader("jpg"))
	assert_equals("tuttle.turbojpegreader", getBestPlugin.getBestReader("JPG"))
	assert_raises(ValueError, getBestPlugin.getBestReader, "/JPG")
	assert_raises(ValueError, getBestPlugin.getBestReader, "/non/existing/path")

	allJpegReaders = ['tuttle.turbojpegreader', 'tuttle.jpegreader', 'tuttle.oiioreader', 'tuttle.imagemagickreader']
	assert_equals(allJpegReaders, getBestPlugin.getReaders("jpg"))

