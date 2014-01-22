#!/usr/bin/python
""" Sample code that demonstrate progress handle
"""
import os
from sys import argv
# Set Tuttle env
import envTuttle
from pyTuttle import tuttle
import getBestPlugin

# Preload pyTuttle
tuttle.core().preload(False)


def start():
    """A callback function

    """
    print "One, two, three... Go!"


class ProgressHandle(tuttle.IProgressHandle):
    """Progress handle that must inherit IProgressHandle
    beginSequence(), setupAtTime(), processAtTime() and endSequence() have to
    be overwritten
    Do whatever you want do in these functions (print, increment var, call
    another func...)
    See example above

    """
    def __init__(self, callback=None):
        # Required
        super(ProgressHandle, self).__init__()
        # Test counter (optional)
        self.callback = callback
        self.counter = 0

    def beginSequence(self):
        """Called before the beginning of the process

        """
        if self.callback:
            self.callback()
        print "---> beginSequence"

    def setupAtTime(self):
        """Called when setting up an image

        """
        pass

    def processAtTime(self):
        """Called before processing an image

        """
        self.counter += 1
        print "---> processAtTime"

    def endSequence(self):
        """Called at the end of the process

        """
        print "---> endSequence"


if len(argv) != 3:
    print "Script args : seqIn@.ext seqOut@.ext"
else:
    pathIn = argv[1]
    pathOut = argv[2]
    # ComputeOption init
    co = tuttle.ComputeOptions()
    co.setVerboseLevel(tuttle.eVerboseLevelError)
    ## Create handle and set it in ComputeOptions
    handle = ProgressHandle(start)
    co.setProgressHandle(handle)
    # Create nodes
    extIn = os.path.splitext(pathIn)[1]
    extOut = os.path.splitext(pathOut)[1]
    readerInPlug = getBestPlugin.getBestReader(extIn)
    writerOutPlug = getBestPlugin.getBestWriter(extOut)
    if len(readerInPlug) == 0:
        print "ERROR: unsupported input file : " + extIn
        exit()
    if len(writerOutPlug) == 0:
        print "ERROR: unsupported input file : " + extOut
        exit()
    # Create graph
    g = tuttle.Graph()
    # Creade nodes
    print "--- Create read in node : " + pathIn
    readerIn = g.createNode(readerInPlug, filename=pathIn).asImageEffectNode()
    # Time domain setup
    g.setup()
    td = readerIn.getTimeDomain()
    g.setupAtTime(td.min, [readerIn])
    textNode = g.createNode('tuttle.text', text="'frame: {args.time_frame}'.format(args=args)",
                            expression=True, font='liberation_sans', size=22)
    print "--- Create read out node : " + pathOut
    writerOut = g.createNode(writerOutPlug, filename=pathOut)

    print "--- connect"
    g.connect(readerIn, textNode)
    g.connect(textNode, writerOut)
    print "--- compute"
    g.compute(writerOut, co)

    print "Images processed count : " + str(handle.counter)
