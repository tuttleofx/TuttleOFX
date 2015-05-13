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


def moveSequence(inputSequence, inputSequencePath, first, last, offset, outputSequence, outputSequencePath):
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
    frameRangeList = list(inputSequence.getFramesIterable(first, last))
    if offset > 0:
        frameRangeList = reversed(frameRangeList)

    # for each time of sequence
    for time in frameRangeList:
        inputPath = os.path.join(inputSequencePath, inputSequence.getFilenameAt(time))
        outputTime = time + offset
        outputPath = os.path.join(outputSequencePath, outputSequence.getFilenameAt(outputTime))

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

    # get input sequence
    detectionMethod = sequenceParser.eDetectionDefault | sequenceParser.eDetectionNegative
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
        if args.outputLast < inputSequence.getLastTime():
            puts(colored.red('Error: cannot force last time of output sequence to ' + str(args.outputLast) + ' when the last time of input sequence is ' + str(inputSequence.getLastTime())))
            exit(-1)
        else:
            offset += args.outputLast - inputSequence.getLastTime()
    # sam-mv -o
    if args.offset:
        offset += args.offset

    # move sequence
    moveSequence(inputSequence, inputSequencePath, first, last, offset, outputSequence, outputSequencePath)
