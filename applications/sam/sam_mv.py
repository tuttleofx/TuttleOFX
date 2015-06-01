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
from common import samUtils


class SamMv(samUtils.Sam):
    """
    Class which represents the sam_mv operation.
    """

    def __init__(self):
        samUtils.Sam.__init__(self)
        self.command = 'mv'
        self.help = 'mv, to move sequences'
        self.description = '''
            Move sequence(s) in a directory.
            Move sequence of image files, and could remove trees (folder, files and sequences).
            '''
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

        parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character (False by default)')

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
                for hole in range(1,gap):
                    holesToRemove.append(lastest + hole)
                lastest = currentRange.last

        return {'first': first, 'last': last, 'offset': offset, 'holes': holesToRemove}

    def _processSequence(self, inputItem, outputSequence, outputSequencePath, moveManipulators):
        """
        Apply operation to the sequence contained in inputItem (used by sam-mv and sam-cp).
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

            # security: check if file already exist
            if os.path.exists(outputPath):
                puts(colored.red('Error: the output path "' + outputPath + '" already exist!'))
                exit(-1)

            # process the image at time
            self._operation(inputPath, outputPath)

    def run(self, parser):
        """
        Process the move operation.
        """
        # Activate completion
        argcomplete.autocomplete(parser)

        # Parse command-line
        args = parser.parse_args()

        # check command line
        if args.offset and (args.outputFirst is not None or args.outputLast is not None):
            puts(colored.red('Error: you cannot cumulate multiple options to modify the time.'))
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
        for input in args.inputs:
            inputItem = samUtils.getSequenceItemFromPath(input, args.detectNegative)

            if not outputIsSequence:
                outputSequence = sequenceParser.Sequence(inputItem.getSequence())

            # How to process the move operation
            moveManipulators = self._getSequenceManipulators(inputItem.getSequence(), args)

            # move sequence
            self._processSequence(inputItem, outputSequence, outputSequencePath, moveManipulators)


if __name__ == '__main__':
    # Create the tool
    tool = SamMv()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        )

    # Run the command
    tool.fillParser(parser)
    tool.run(parser)
