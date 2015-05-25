#!/usr/bin/env python

import os

# python modules to easily get colors, indent text...
from clint.textui import colored, puts

# parser of sequence
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
        puts(colored.red('Error: input is not a sequence: ', inputItem.getFilename()))
        exit(-1)

    return inputItem


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
