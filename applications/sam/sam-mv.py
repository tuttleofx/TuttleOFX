#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
import argparse

from pySequenceParser import sequenceParser

import shutil

import argcomplete
from clint.textui import colored, puts


def sequenceParserCompleter(prefix, **kwargs):
    """
    Custom Completer to manage auto competion when looking for sequences.
    """
    directory = os.path.dirname(prefix)
    if directory == '':
        directory = '.'
    items = sequenceParser.browse(directory, sequenceParser.eDetectionDefault, [str(prefix+'*')])
    itemsStr = [str(item.getFilename()) for item in items]
    return itemsStr


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
            puts(colored.magenta('Error: the output path "' + outputPath + '" already exist!'))
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

    parser.add_argument('inputs', nargs='*', action='store', help='list of input directories to move').completer = sequenceParserCompleter

    # Options
    parser.add_argument('-o', '--offset', dest='offset', type=int, help='retime the sequence with the given offset')
    parser.add_argument('--input-first', dest='inputFirst', type=int, help='specify the first input image in order to select a sub-range of the input sequence')
    parser.add_argument('--input-last', dest='inputLast', type=int, help='specify the last input image in order to select a sub-range of the input sequence')
    parser.add_argument('--output-first', dest='outputFirst', type=int, help='specify the first output image, in order to select a sub-range of the output sequence')
    parser.add_argument('--output-last', dest='outputLast', type=int, help='specify the last input image in order to select a sub-range of the output sequence')
    parser.add_argument('--remove-holes', dest='removeHoles', action='store_true', help='remove holes of the sequence')
    parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character (False by default)')

    # Activate completion
    argcomplete.autocomplete(parser)

    # Parse command-line
    args = parser.parse_args()

    # check command line
    if len(args.inputs) < 2:
        puts(colored.red('Error: two sequences and/or directories must be specified.'))
        exit(-1)

    if args.offset and (args.outputFirst is not None or args.outputLast is not None):
        puts(colored.red('Error: you cannot cumulate multiple options to modify the time.'))
        exit(-1)

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
