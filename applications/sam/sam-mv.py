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


def moveSequence(inputSequence, inputSequencePath, first, last, offset, outputSequence, outputSequencePath, holesToRemove):
    """
    Move an input sequence from inputSequencePath to output.
    Depending on args, update the frame ranges of the output sequence.
    
    :param inputSequence: the input sequence to move
    :param inputSequencePath: path of the inputSequence
    :param first: first time of the inputSequence
    :param last: last time of the inputSequence
    :param offset: offset used to retime the output sequence
    :param outputSequence: the output sequence to write.
    :param outputSequence: the path of the outputSequence.
    :param holesToRemove: list of holes to remove in the output sequence
    """
    # create output directory if not exists
    try:
        if not os.path.exists(outputSequencePath):
            os.makedirs(outputSequencePath)
    except Exception as e:
        puts(colored.red('Error: cannot create directory tree for "' + outputSequencePath + '": ' + str(e)))
        exit(-1)

    # print brief of the operation
    puts(colored.magenta(os.path.join(inputSequencePath, str(inputSequence)) + ' -> ' + os.path.join(outputSequencePath, str(outputSequence)), bold=True))

    # get frame ranges
    inputFrameList = list(inputSequence.getFramesIterable(first, last))
    outputFrameList = sorted(inputFrameList + holesToRemove)
    if offset > 0:
        inputFrameList = reversed(inputFrameList)
        outputFrameList = reversed(outputFrameList)

    # for each time of sequence
    for inputTime, outputTime in zip(inputFrameList, outputFrameList):
        inputPath = os.path.join(inputSequencePath, inputSequence.getFilenameAt(inputTime))
        outputPath = os.path.join(outputSequencePath, outputSequence.getFilenameAt(outputTime + offset))

#        print inputPath, '->', outputPath

        # security: check if file already exist
        if os.path.exists(outputPath):
            puts(colored.red('Error: the output path "' + outputPath + '" already exist!'))
            exit(-1)

        # move the image at time
        shutil.move(inputPath, outputPath)
#        shutil.copy2(inputPath, outputPath)


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

    inputSequence = inputItem.getSequence()

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
    first = args.inputFirst if (args.inputFirst is not None and args.inputFirst > inputSequence.getFirstTime()) else inputSequence.getFirstTime()

    # sam-mv --input-last
    last = args.inputLast if (args.inputLast is not None and args.inputLast < inputSequence.getLastTime()) else inputSequence.getLastTime()

    offset = 0
    # sam-mv --output-first
    if args.outputFirst is not None:
        offset += args.outputFirst - inputSequence.getFirstTime()
    # sam-mv --output-last
    if args.outputLast is not None:
        offset += args.outputLast - inputSequence.getLastTime()
    # sam-mv -o
    if args.offset:
        offset += args.offset

    # sam-mv --remove-holes
    holesToRemove = []
    if args.removeHoles and inputSequence.hasMissingFile():
        lastest = -1
        for currentRange in inputSequence.getFrameRanges():
            if lastest == -1:
                lastest = currentRange.last
                continue

            gap = currentRange.first - lastest
            for hole in range(1,gap):
                holesToRemove.append(lastest + hole)
            lastest = currentRange.last

    # move sequence
    moveSequence(inputSequence, inputSequencePath, first, last, offset, outputSequence, outputSequencePath, holesToRemove)
