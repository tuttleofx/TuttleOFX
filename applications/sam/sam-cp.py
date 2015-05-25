#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
import argparse
import shutil

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
from common import samCmdLines, samUtils


def main(args = None):
    if not args:
        # Create command-line interface
        parser = argparse.ArgumentParser(
                prog='sam-cp',
                description='''
                copy sequence(s) in a directory.
                Copy sequence of image files, and could remove trees (folder, files and sequences).
                ''',
                )

        # Add command line arguments
        samCmdLines.addMvCpArgumentsToParser(parser)

        # Get arguments
        args = samCmdLines.getMvCpArgumentsFromParser(parser)

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
        inputItem = samUtils.getSequenceItemFromPath(input, args.detectNegative)

        if not outputIsSequence:
            outputSequence = sequenceParser.Sequence(inputItem.getSequence())

        # How to process the copy operation
        moveManipulators = samUtils.getMvCpSequenceManipulators(inputItem.getSequence(), args)

        # copy sequence
        samUtils.processSequence(inputItem, outputSequence, outputSequencePath, moveManipulators, shutil.copy2)


if __name__ == '__main__':
    main()
