from pyTuttle import tuttle

from nose.tools import *
from nose.tools import nottest

def setUp():
	tuttle.core().preload(False)


@nottest
def testGetBestReader():
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader("path/to/image.jpg"))
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader(".jpg"))
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader("jpg"))
	assert_equals("tuttle.turbojpegreader", tuttle.getBestReader("JPG"))
	assert_raises(RuntimeError, tuttle.getBestReader, "/JPG")
	assert_raises(RuntimeError, tuttle.getBestReader, "/non/existing/path")

	allJpegReaders = ('tuttle.turbojpegreader', 'tuttle.jpegreader', 'tuttle.oiioreader', 'tuttle.imagemagickreader')
	foundListReaders = tuttle.getReaders("jpg")
	assert_equals(allJpegReaders, foundListReaders)
