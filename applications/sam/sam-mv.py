#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
import argparse
import shutil

# python modules to easily get completion, colors, indent text...
from clint.textui import colored, puts

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
import common

if __name__ == '__main__':

    # Create command-line interface
    parser = argparse.ArgumentParser(
            prog='sam-mv',
            description='''
            Move sequence(s) in a directory.
            Move sequence of image files, and could remove trees (folder, files and sequences).
            ''',
            )

    # Add command line arguments
    common.addMvCpArgumentsToParser(parser)

    # Get arguments
    args = common.getMvCpArgumentsFromParser(parser)

    # Get input
    inputItem = common.getSequenceItemFromPath(args.inputs[0], args.detectNegative)

    # get output path
    outputSequencePath = os.path.dirname(args.inputs[1])
    if not outputSequencePath:
        outputSequencePath = '.'
    outputSequenceName = os.path.basename(args.inputs[1])

    # get output sequence
    outputSequence = sequenceParser.Sequence()
    outputIsSequence = outputSequence.initFromPattern(outputSequenceName, sequenceParser.ePatternDefault)
    if not outputIsSequence:
        outputSequence = sequenceParser.Sequence(inputSequence)

    # How to process the move operation
    moveManipulators = common.getMvCpSequenceManipulators(inputItem.getSequence(), args)

    # move sequence
    common.processSequence(inputItem, outputSequence, outputSequencePath, moveManipulators, shutil.move)
