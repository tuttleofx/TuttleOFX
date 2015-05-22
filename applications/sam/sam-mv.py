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

    # Get output path
    outputSequencePath = os.path.dirname(args.output)
    if not outputSequencePath:
        outputSequencePath = '.'

    # Get output sequence
    outputSequence = sequenceParser.Sequence()
    outputSequenceName = os.path.basename(args.output)
    outputIsSequence = outputSequence.initFromPattern(outputSequenceName, sequenceParser.ePatternDefault)

    # For each input
    for input in args.inputs:
        inputItem = common.getSequenceItemFromPath(input, args.detectNegative)

        if not outputIsSequence:
            outputSequence = sequenceParser.Sequence(inputItem.getSequence())

        # How to process the move operation
        moveManipulators = common.getMvCpSequenceManipulators(inputItem.getSequence(), args)

        # move sequence
        common.processSequence(inputItem, outputSequence, outputSequencePath, moveManipulators, shutil.move)
