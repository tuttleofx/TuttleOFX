#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
import argparse
import shutil

# python modules to easily get completion, colors, indent text...
import argcomplete
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

    # get input path and name
    inputSequencePath = os.path.dirname(args.inputs[0])
    if not inputSequencePath:
        inputSequencePath = '.'
    inputSequenceName = os.path.basename(args.inputs[0])

    # sam-mv --detect-negative
    detectionMethod = sequenceParser.eDetectionDefault
    if args.detectNegative:
        detectionMethod = detectionMethod | sequenceParser.eDetectionNegative

    # get input sequence
    inputItems = sequenceParser.browse(inputSequencePath, detectionMethod, [inputSequenceName])
    if len(inputItems) != 1:
        puts(colored.red('Error: no existing file corresponds to the given input sequence: ' + args.inputs[0]))
        exit(-1)

    inputItem = inputItems[0]
    if inputItem.getType() != sequenceParser.eTypeSequence:
        puts(colored.red('Error: first input is not a sequence: ', inputItem.getFilename()))
        exit(-1)

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

    # sam-mv --input-first
    first = args.inputFirst if (args.inputFirst is not None and args.inputFirst > inputItem.getSequence().getFirstTime()) else inputItem.getSequence().getFirstTime()

    # sam-mv --input-last
    last = args.inputLast if (args.inputLast is not None and args.inputLast < inputItem.getSequence().getLastTime()) else inputItem.getSequence().getLastTime()

    offset = 0
    # sam-mv --output-first
    if args.outputFirst is not None:
        offset += args.outputFirst - inputItem.getSequence().getFirstTime()
    # sam-mv --output-last
    if args.outputLast is not None:
        offset += args.outputLast - inputItem.getSequence().getLastTime()
    # sam-mv -o
    if args.offset:
        offset += args.offset

    # sam-mv --remove-holes
    holesToRemove = []
    if args.removeHoles and inputItem.getSequence().hasMissingFile():
        lastest = -1
        for currentRange in inputItem.getSequence().getFrameRanges():
            if lastest == -1:
                lastest = currentRange.last
                continue

            gap = currentRange.first - lastest
            for hole in range(1,gap):
                holesToRemove.append(lastest + hole)
            lastest = currentRange.last

    # move sequence
    common.processSequence(inputItem, first, last, offset, outputSequence, outputSequencePath, holesToRemove, shutil.move)
