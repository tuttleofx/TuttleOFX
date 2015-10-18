#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import os
import argparse

from clint.textui import colored

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
from common import samUtils


class Sam_rm(samUtils.Sam):
    """
    Class which represents the sam_rm operation.
    """

    def __init__(self):
        samUtils.Sam.__init__(self)
        self.command = 'rm'
        self.help = 'to remove sequences (and other files)'
        self.description = str(colored.green('''
            Remove sequences, files and folders.
            '''))

    def fillParser(self, parser):
        # Arguments
        parser.add_argument('inputs', nargs='+', action='store', help='list of input files/sequences/directories to remove').completer = samUtils.sequenceParserCompleter

        # Options
        parser.add_argument('-a', '--all', dest='all', action='store_true', help='do not ignore entries starting with .')
        parser.add_argument('-d', '--directories', dest='directories', action='store_true', help='handle directories')
        parser.add_argument('-s', '--sequences', dest='sequences', action='store_true', help='handle sequences')
        parser.add_argument('-f', '--files', dest='files', action='store_true', help='handle files')
        parser.add_argument('-e', '--expression', dest='expression', help='use a specific pattern, ex: "*.jpg", "*.png"')

        parser.add_argument('-R', '--recursive', dest='recursive', action='store_true', help='handle directories and their content recursively')
        parser.add_argument('--first-image', dest='firstImage', type=int, help='specify the first image of sequence')
        parser.add_argument('--last-image', dest='lastImage', type=int, help='specify the last image of sequence')
        parser.add_argument('--range', dest='range', nargs=2, type=int, help='specify the range of sequence')
        parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character')
        parser.add_argument('-v', '--verbose', dest='verbose', action=samUtils.SamSetVerboseAction, default=2, help='verbose level (0/fatal, 1/error, 2/warn(by default), 3/info, 4(or upper)/debug)')

    def _removeItem(self, item, args):
        """
        Remove the item.
        """
        itemType = item.getType()
        filePath = os.path.abspath(item.getFolder())

        # remove folder
        if itemType == sequenceParser.eTypeFolder:
            try:
                os.rmdir(os.path.join(filePath, (item.getFilename() if item.getFilename() != '.' else '')))
            except OSError:
                self.logger.error('You cannot remove a folder which contains elements like this. If you still want to, see "-R" option.')
                return 1
            return 0

        # remove other things than sequences (file, link...)
        if itemType != sequenceParser.eTypeSequence:
            os.remove(os.path.join(filePath, item.getFilename()))
            return 0

        sequence = item.getSequence()

        # sam-rm --first-image
        first = args.firstImage if (args.firstImage is not None and args.firstImage > sequence.getFirstTime()) else sequence.getFirstTime()

        # sam-rm --last-image
        last = args.lastImage if (args.lastImage is not None and args.lastImage < sequence.getLastTime()) else sequence.getLastTime()

        # sam-rm --range
        if args.range:
            first = args.range[0] if args.range[0] > first else first
            last = args.range[1] if args.range[1] < last else last

        # remove files in sequence
        error = 0
        for time in sequence.getFramesIterable(first, last):
            try:
                filePathInSequence = os.path.join(filePath, sequence.getFilenameAt(time))
                self.logger.info('Remove file "' + filePathInSequence + '".')
                os.remove(os.path.join(filePathInSequence))
            except OSError:
                self.logger.error('Cannot find file "' + filePathInSequence + '" in sequence.')
                error = 1
        return error

    def _removeItems(self, items, args, detectionMethod, filters):
        """
        For each items, check if it should be removed, depending on the command line options.
        """
        folderItems = []
        error = 0

        for item in sorted(items):
            itemType = item.getType()
            toRemove = True

            # sam-rm -d
            if args.directories and itemType != sequenceParser.eTypeFolder:
                toRemove = False

            # sam-rm -f
            if args.files and itemType != sequenceParser.eTypeFile:
                toRemove = False

            # sam-rm -s
            if args.sequences and itemType != sequenceParser.eTypeSequence:
                toRemove = False

            # sam-rm -R
            if args.recursive and itemType == sequenceParser.eTypeFolder:
                newItems = sequenceParser.browse(os.path.join(item.getFolder(), item.getFilename()), detectionMethod, filters)
                self._removeItems(newItems, args, detectionMethod, filters)

            if toRemove:
                # store folder and delete it after all other elements
                if itemType == sequenceParser.eTypeFolder:
                    folderItems.insert(0, item)
                # remove current item
                else:
                    err = self._removeItem(item, args)
                    if err:
                        error = err

        # remove folders (which are empty)
        for folder in folderItems:
            err = self._removeItem(folder, args)
            if err:
                error = err

        return error

    def run(self, parser):
        """
        Process the remove operation.
        """
        # Parse command-line
        args = parser.parse_args()

        # Set sam log level
        self.setLogLevel(args.verbose)

        # check command line
        if args.range and (args.firstImage is not None or args.lastImage is not None):
            self.logger.error('You cannot cumulate multiple options to specify the range of sequence.')
            exit(1)

        if '.' in args.inputs or '..' in args.inputs:
            self.logger.error('You cannot remove folders "." or "..".')
            exit(1)

        # sam-rm -a
        detectionMethod = sequenceParser.eDetectionDefault
        if args.all:
            detectionMethod = sequenceParser.eDetectionDefaultWithDotFile

        # sam-rm --detect-negative
        if args.detectNegative:
            detectionMethod = detectionMethod | sequenceParser.eDetectionNegative

        # sam-rm -e
        filters = []
        if args.expression:
            filters.append(args.expression)

        # get list of items for each inputs
        error = 0
        for input in args.inputs:
            items = []

            # input is a directory
            if not os.path.basename(input):
                items.append(sequenceParser.Item(sequenceParser.eTypeFolder, input))
            # else browse directory with a filter, to find the corresponding Item
            else:
                # get path to browse
                pathToBrowse = os.path.dirname(input)
                if not pathToBrowse:
                    pathToBrowse = '.'
                # get filter
                filterToBrowse = []
                filterToBrowse.extend(filters)
                filterToBrowse.append(os.path.basename(input))
                # browse
                items = sequenceParser.browse(pathToBrowse, detectionMethod, filterToBrowse)

            # print error if no items were found
            if len(items) == 0:
                self.logger.error('No file or folders correspond to "' + input + '".')
                error = 1
                continue

            err = self._removeItems(items, args, detectionMethod, filters)
            if err:
                error = err

        exit(error)


if __name__ == '__main__':
    # Create the tool
    tool = Sam_rm()

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
