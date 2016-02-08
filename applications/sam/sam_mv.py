#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import os
import argparse
import shutil

from clint.textui import colored

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
from common import samUtils


class Sam_mv(samUtils.Sam):
    """
    Class which represents the sam_mv operation.
    """

    def __init__(self):
        samUtils.Sam.__init__(self)
        self.command = 'mv'
        self.help = 'to move sequences'
        self.description = str(colored.green('''
            Move sequences in a directory.
            Rename sequences if the output directory is the same as the input.
            '''))
        self._operation = shutil.move

    def fillParser(self, parser):
        parser.add_argument('inputs', metavar='INPUTS', nargs='+', action='store', help='list of inputs').completer = samUtils.sequenceParserCompleter
        parser.add_argument('output', metavar='OUTPUT', help='the output to write').completer = samUtils.sequenceParserCompleter

        # Options
        parser.add_argument('-o', '--offset', dest='offset', type=int, help='retime the sequence with the given offset')
        parser.add_argument('--remove-holes', dest='removeHoles', action='store_true', help='remove holes of the sequence')
        # input group
        inputGroup = parser.add_argument_group('select range of input')
        inputGroup.add_argument('--input-first', dest='inputFirst', type=int, help='specify the first input image in order to select a sub-range of the input sequence')
        inputGroup.add_argument('--input-last', dest='inputLast', type=int, help='specify the last input image in order to select a sub-range of the input sequence')
        # output group
        outputGroup = parser.add_argument_group('select range of output')
        outputGroup.add_argument('--output-first', dest='outputFirst', type=int, help='specify the first output image, in order to select a sub-range of the output sequence')
        outputGroup.add_argument('--output-last', dest='outputLast', type=int, help='specify the last input image in order to select a sub-range of the output sequence')

        parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character')
        parser.add_argument('-v', '--verbose', dest='verbose', action=samUtils.SamSetVerboseAction, default=2, help='verbose level (0/fatal, 1/error, 2/warn(by default), 3/info, 4(or upper)/debug)')

    def _getSequenceManipulators(self, inputSequence, args):
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
                for hole in range(1, gap):
                    holesToRemove.append(lastest + hole)
                lastest = currentRange.last

        return {'first': first, 'last': last, 'offset': offset, 'holes': holesToRemove}

    def _processSequence(self, inputItem, outputSequence, outputSequencePath, moveManipulators):
        """
        Apply operation to the sequence contained in inputItem (used by sam-mv and sam-cp).
        Depending on args, update the frame ranges of the output sequence.
        Return if the operation was a success or not.

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
            self.logger.error('Cannot create directory tree for "' + outputSequencePath + '": ' + str(e))
            return 1

        # print brief of the operation
        self.logger.info(os.path.join(inputItem.getFolder(), str(inputItem.getSequence())) + ' -> ' + os.path.join(outputSequencePath, str(outputSequence)))

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

            # security: check if file already exist
            if os.path.exists(outputPath):
                self.logger.error('The output path "' + outputPath + '" already exist!')
                return 1

            # process the image at time
            self._operation(inputPath, outputPath)

    def _getSequenceItemFromPath(self, inputPath, detectNegative):
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
        if not len(inputItems):
            self.logger.error('No existing file corresponds to the given input sequence: ' + inputPath)
            exit(-1)
        if len(inputItems) > 1:
            self.logger.error('Several items ' + str([item.getFilename() for item in inputItems]) + ' correspond to the given input sequence: ' + inputPath)
            exit(-1)

        # check if the item is a sequence
        inputItem = inputItems[0]
        if inputItem.getType() != sequenceParser.eTypeSequence:
            self.logger.error('Input is not a sequence: ' + inputItem.getFilename())
            exit(-1)

        return inputItem

    def run(self, parser):
        """
        Process the move operation.
        """
        # Parse command-line
        args = parser.parse_args()

        # Set sam log level
        self.setLogLevel(args.verbose)

        # check command line
        if args.offset and (args.outputFirst is not None or args.outputLast is not None):
            self.logger.error('You cannot cumulate multiple options to modify the time.')
            exit(-1)

        # Get output path
        outputSequencePath = os.path.dirname(args.output)
        if not outputSequencePath:
            outputSequencePath = '.'

        # Get output sequence
        outputSequence = sequenceParser.Sequence()
        outputSequenceName = os.path.basename(args.output)
        outputIsSequence = outputSequence.initFromPattern(outputSequenceName, sequenceParser.ePatternDefault)

        # For each input
        error = 0
        for input in args.inputs:
            inputItem = self._getSequenceItemFromPath(input, args.detectNegative)

            if not outputIsSequence:
                outputSequence = sequenceParser.Sequence(inputItem.getSequence())

            # How to process the move operation
            moveManipulators = self._getSequenceManipulators(inputItem.getSequence(), args)

            # move sequence
            err = self._processSequence(inputItem, outputSequence, outputSequencePath, moveManipulators)
            if err:
                error = err

        exit(error)

if __name__ == '__main__':
    # Create the tool
    tool = Sam_mv()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        formatter_class=argparse.RawTextHelpFormatter,
        )
    tool.fillParser(parser)

    # Activate completion
    samUtils.doCompletion(parser)

    # Run the command
    tool.run(parser)
