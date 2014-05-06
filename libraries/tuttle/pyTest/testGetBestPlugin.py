# scons: pluginTurboJpeg pluginJpeg pluginOpenImageIO pluginImageMagick

from pyTuttle import tuttle

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testGetBestReader():
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader("../eia1956-small.jpg"))
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader(".jpg"))
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader("jpg"))
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader("JPG"))
	assert_raises(RuntimeError, tuttle.getBestReader, "/JPG")
	assert_raises(RuntimeError, tuttle.getBestReader, "/non/existing/path")

	allJpegReaders = ['tuttle.turbojpegreader', 'tuttle.jpegreader', 'tuttle.oiioreader', 'tuttle.imagemagickreader']
	foundListReaders = tuttle.getReaders("jpg")
	for reader in foundListReaders:
		if not reader in allJpegReaders:
			assert( false )

