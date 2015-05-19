#!/usr/bin/env python

import os
import argparse

import argcomplete
from clint.textui import colored, puts

from pySequenceParser import sequenceParser


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


def getSequenceItemFromPath(inputPath, detectNegative):
    """
    Get an Item (which corresponds to a sequence) from a path.
    """
    # get input path and name
    inputSequencePath = os.path.dirname(inputPath)
    if not inputSequencePath:
        inputSequencePath = '.'
    inputSequenceName = os.path.basename(inputPath)

    # sam-mv --detect-negative
    detectionMethod = sequenceParser.eDetectionDefault
    if detectNegative:
        detectionMethod = detectionMethod | sequenceParser.eDetectionNegative

    # get input sequence
    inputItems = sequenceParser.browse(inputSequencePath, detectionMethod, [inputSequenceName])
    if len(inputItems) != 1:
        puts(colored.red('Error: no existing file corresponds to the given input sequence: ' + inputPath))
        exit(-1)

    inputItem = inputItems[0]
    if inputItem.getType() != sequenceParser.eTypeSequence:
        puts(colored.red('Error: first input is not a sequence: ', inputItem.getFilename()))
        exit(-1)

    return inputItem


def addDetectNegativeArgumentToParser(parser):
    """
    Add common a common option to command line of sam tools.
    """
    parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character (False by default)')


def addCommonFilterArgumentsToParser(parser):
    """
    Add common options to command line of sam-ls and sam-rm tools.
    """
    parser.add_argument('-a', '--all', dest='all', action='store_true', help='do not ignore entries starting with .')
    parser.add_argument('-d', '--directories', dest='directories', action='store_true', help='handle directories')
    parser.add_argument('-s', '--sequences', dest='sequences', action='store_true', help='handle sequences')
    parser.add_argument('-f', '--files', dest='files', action='store_true', help='handle files')
    parser.add_argument('-e', '--expression', dest='expression', help='use a specific pattern, ex: *.jpg,*.png').completer = sequenceParserCompleter


def addMvCpArgumentsToParser(parser):
    """
    Add common arguments/options to command line of sam-mv and sam-cp tools.
    """
    # Arguments
    parser.add_argument('inputs', nargs='*', action='store', help='list of input directories').completer = sequenceParserCompleter

    # Options
    parser.add_argument('-o', '--offset', dest='offset', type=int, help='retime the sequence with the given offset')
    parser.add_argument('--input-first', dest='inputFirst', type=int, help='specify the first input image in order to select a sub-range of the input sequence')
    parser.add_argument('--input-last', dest='inputLast', type=int, help='specify the last input image in order to select a sub-range of the input sequence')
    parser.add_argument('--output-first', dest='outputFirst', type=int, help='specify the first output image, in order to select a sub-range of the output sequence')
    parser.add_argument('--output-last', dest='outputLast', type=int, help='specify the last input image in order to select a sub-range of the output sequence')
    parser.add_argument('--remove-holes', dest='removeHoles', action='store_true', help='remove holes of the sequence')
    addDetectNegativeArgumentToParser(parser)


def getMvCpArgumentsFromParser(parser):
    """
    Get list of arguments/options values which corresond to sam-mv and sam-cp tools.
    """
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

    return args


def processSequence(inputItem, outputSequence, outputSequencePath, moveManipulators, func):
    """
    Apply 'func' operation to the sequence contained in inputItem (used by sam-mv and sam-cp).
    Depending on args, update the frame ranges of the output sequence.
    
    :param inputItem: the item which contains the input sequence to process (move, copy...)
    :param outputSequence: the output sequence to write (destination of move, copy...)
    :param outputSequence: the path of the outputSequence.
    :param moveManipulators: dict which contains 
        {
            first time of the inputSequence, 
            last time of the inputSequence, 
            offset used to retime the output sequence, 
            list of holes to remove in the output sequence
        }
    """
    # create output directory if not exists
    try:
        if not os.path.exists(outputSequencePath):
            os.makedirs(outputSequencePath)
    except Exception as e:
        puts(colored.red('Error: cannot create directory tree for "' + outputSequencePath + '": ' + str(e)))
        exit(-1)

    # print brief of the operation
    puts(colored.magenta(os.path.join(inputItem.getFolder(), str(inputItem.getSequence())) + ' -> ' + os.path.join(outputSequencePath, str(outputSequence)), bold=True))

    # get frame ranges
    inputFrameList = list(inputItem.getSequence().getFramesIterable(moveManipulators['first'], moveManipulators['last']))
    outputFrameList = sorted(inputFrameList + moveManipulators['holes'])
    if moveManipulators['offset'] > 0:
        inputFrameList = reversed(inputFrameList)
        outputFrameList = reversed(outputFrameList)

    # for each time of sequence
    for inputTime, outputTime in zip(inputFrameList, outputFrameList):
        inputPath = os.path.join(inputItem.getFolder(), inputItem.getSequence().getFilenameAt(inputTime))
        outputPath = os.path.join(outputSequencePath, outputSequence.getFilenameAt(outputTime + moveManipulators['offset']))

#        print inputPath, '->', outputPath

        # security: check if file already exist
        if os.path.exists(outputPath):
            puts(colored.red('Error: the output path "' + outputPath + '" already exist!'))
            exit(-1)

        # process the image at time
        func(inputPath, outputPath)


def getMvCpSequenceManipulators(inputSequence, args):
    """
    Returns a dict of values to detect how to process move/copy of sequence.
    """
    # --input-first
    first = args.inputFirst if (args.inputFirst is not None and args.inputFirst > inputSequence.getFirstTime()) else inputSequence.getFirstTime()

    # --input-last
    last = args.inputLast if (args.inputLast is not None and args.inputLast < inputSequence.getLastTime()) else inputSequence.getLastTime()

    offset = 0
    # --output-first
    if args.outputFirst is not None:
        offset += args.outputFirst - inputSequence.getFirstTime()
    # --output-last
    if args.outputLast is not None:
        offset += args.outputLast - inputSequence.getLastTime()
    # --offset
    if args.offset:
        offset += args.offset

    # --remove-holes
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
    
    return {'first': first, 'last': last, 'offset': offset, 'holes': holesToRemove}
