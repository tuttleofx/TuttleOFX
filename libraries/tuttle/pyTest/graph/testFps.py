from pyTuttle import tuttle

from nose.tools import *


def setUp():
    tuttle.core().preload(False)


def testFpsPropagation():
    g = tuttle.Graph()

    reader = g.createNode("tuttle.avreader", filename="TuttleOFX-data/video/morgen-20030714.avi")
    inv1 = g.createNode("tuttle.invert")
    inv2 = g.createNode("tuttle.invert")
    writer = g.createNode("tuttle.avwriter", filename=".tests/output.avi")

    g.connect(reader, inv1, inv2, writer)

    clipReader = reader.getOutputClip()
    clipWriter = writer.getOutputClip()

    assert_equals(clipReader.getFrameRate(), clipWriter.getFrameRate())
