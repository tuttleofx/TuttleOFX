#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import os
from datetime import date
import argparse

from clint.textui import colored, puts, indent

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
from common import samUtils


class Sam_ls(samUtils.Sam):
    """
    Class which represents the sam_ls operation.
    """

    def __init__(self):
        samUtils.Sam.__init__(self)
        self.command = 'ls'
        self.help = 'to list sequences (and other files)'
        self.description = str(colored.green('''
            List information about sequences, files and folders.
            List the current directory by default.
            '''))
        self._itemPrinted = [] # name of Items already printed
        self._sequenceExploded = [] # name of Sequences already exploded

    def fillParser(self, parser):
        def level_type(level):
            """
            Check constrains of 'level' argument.
            """
            level = int(level)
            if level < 0:
                raise argparse.ArgumentTypeError("Minimum level is 0")
            return level

        # Arguments
        parser.add_argument('inputs', nargs='*', action='store', help='list of files/sequences/directories to analyse').completer = samUtils.sequenceParserCompleter

        # Options
        parser.add_argument('-a', '--all', dest='all', action='store_true', help='do not ignore entries starting with .')
        parser.add_argument('-d', '--directories', dest='directories', action='store_true', help='display only directories')
        parser.add_argument('-s', '--sequences', dest='sequences', action='store_true', help='display only sequences')
        parser.add_argument('-f', '--files', dest='files', action='store_true', help='display only files')
        parser.add_argument('-e', '--expression', dest='expression', action='append', default=[], help='use a specific pattern, ex: "*.jpg", "*.png"')

        parser.add_argument('-l', '--long-listing', dest='longListing', action='store_true', help='use a long listing format (display in this order: type | permissions | owner | group | last update | minSize | maxSize | totalSize | name)')
        parser.add_argument('--format', dest='format', choices=['default', 'nuke', 'rv'], default='default', help='specify formatting of the sequence padding')
        parser.add_argument('-R', '--recursive', dest='recursive', action='store_true', help='handle directories and their content recursively')
        parser.add_argument('-L', '--level', dest='level', type=level_type, help='max display depth of the directory tree (without formatting if 0)')
        parser.add_argument('--absolute-path', dest='absolutePath', action='store_true', help='display the absolute path of each object')
        parser.add_argument('--relative-path', dest='relativePath', action='store_true', help='display the relative path of each object')
        parser.add_argument('--no-color', dest='noColor', action='store_true', default=False, help='display the output without colors (with colors by default)')
        parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character')
        parser.add_argument('--detect-without-holes', dest='detectWithoutHoles', action='store_true', help='detect sequences without holes')
        parser.add_argument('--explode-sequences', dest='explodeSequences', action='store_true', default=False, help='explode sequences as several sequences without holes')
        parser.add_argument('--script', dest='script', action='store_true', default=False, help='Format the output such as it could be dump in a file and be used as a script')
        parser.add_argument('-v', '--verbose', dest='verbose', action=samUtils.SamSetVerboseAction, default=2, help='verbose level (0/fatal, 1/error, 2/warn(by default), 3/info, 4(or upper)/debug)')

    def _isAlreadyPrinted(self, item):
        """
        Return if the given item has already been printed.
        @see _printItem
        """
        if item.getAbsoluteFilepath() in self._itemPrinted:
            return True
        return False

    def _needToPrintCurrentFolder(self, args):
        """
        Return if the current folder expects to be printed before its content.
        See behavior of 'ls' UNIX command with several arguments.
        """
        return len(args.inputs) > 1 and not args.script and not args.recursive

    def _printItem(self, item, args, level):
        """
        Print the item depending on the command line options.
        """
        itemType = item.getType()

        filePath = ''
        detailed = ''
        detailedSequence = ''

        # sam-ls --explode-sequences
        sequenceExploded = False
        if args.explodeSequences and itemType == sequenceParser.eTypeSequence:
            sequence = item.getSequence()
            for frameRange in sequence.getFrameRanges():
                # for each frame range, print a new item as sequence
                subSequence = sequenceParser.Sequence(sequence.getPrefix(), sequence.getPadding(), sequence.getSuffix(), frameRange.first, frameRange.last, frameRange.step)
                if subSequence.__str__() not in self._sequenceExploded:
                    self._sequenceExploded.append(subSequence.__str__())
                    sequenceExploded = True
                    self._printItem(sequenceParser.Item(subSequence, item.getFolder()), args, level)
            # to skip recursivity
            if sequenceExploded:
                return

        # sam-ls -l
        if args.longListing:
            # type - date - size
            characterFromType = 'a'
            if itemType == sequenceParser.eTypeUndefined:
                characterFromType = '?'
            elif itemType == sequenceParser.eTypeFolder:
                characterFromType = 'd'
            elif itemType == sequenceParser.eTypeFile:
                characterFromType = 'f'
            elif itemType == sequenceParser.eTypeSequence:
                characterFromType = 's'
            elif itemType == sequenceParser.eTypeLink:
                characterFromType = 'l'

            # type - permissions - user - group - lastUpdate - size
            itemStat = sequenceParser.ItemStat(item)

            permissions = ''
            permissions += 'r' if itemStat.ownerCanRead else '-'
            permissions += 'w' if itemStat.ownerCanWrite else '-'
            permissions += 'x' if itemStat.ownerCanExecute else '-'
            permissions += 'r' if itemStat.groupCanRead else '-'
            permissions += 'w' if itemStat.groupCanWrite else '-'
            permissions += 'x' if itemStat.groupCanExecute else '-'
            permissions += 'r' if itemStat.otherCanRead else '-'
            permissions += 'w' if itemStat.otherCanWrite else '-'
            permissions += 'x' if itemStat.otherCanExecute else '-'

            lastUpdate = date.fromtimestamp(itemStat.modificationTime).strftime('%d/%m/%y')

            minSize = samUtils.getReadableSize(itemStat.minSize) if itemStat.minSize != itemStat.size else '-'
            maxSize = samUtils.getReadableSize(itemStat.maxSize) if itemStat.maxSize != itemStat.size else '-'

            detailed = '{:1}{:9}'.format(characterFromType, permissions)
            detailed += ' {:8} {:8} {:8}'.format(itemStat.getUserName(), itemStat.getGroupName(), lastUpdate)
            detailed += ' {:6} {:6} {:6}'.format(minSize, maxSize, samUtils.getReadableSize(itemStat.size))
            detailed += '\t'

        # only for sequences: [ begin : end ] nbFiles - nbMissingFiles
        if itemType == sequenceParser.eTypeSequence:
            sequence = item.getSequence()
            detailedSequence = '[{first}:{last}] {nbFiles} files'.format(first=sequence.getFirstTime(), last=sequence.getLastTime(), nbFiles=sequence.getNbFiles())
            nbHoles = (sequence.getLastTime() - sequence.getFirstTime() + 1) - sequence.getNbFiles()
            if nbHoles:
                detailedSequence += ' - {nbHoles} missing files'.format(nbHoles=nbHoles)

        # sam-ls --absolute-path
        if args.absolutePath:
            filePath += os.path.abspath(item.getFolder())

        # sam-ls --relative-path
        if args.relativePath:
            filePath += (item.getFolder() if item.getFolder()[0] != '/' else '.')
            filePath += ('/' if filePath[-1] != '/' else '')

        # filename
        filename = item.getFilename()
        # sam-ls --format
        if itemType == sequenceParser.eTypeSequence:
            filename = samUtils.getSequenceNameWithFormatting(item.getSequence(), args.format)
        # sam-ls --no-color
        if args.noColor:
            filePath = os.path.join(filePath, filename)
        else:
            if itemType == sequenceParser.eTypeFolder:
                # blue is not visible without bold
                filePath = colored.blue(os.path.join(filePath, filename), bold=True)
            elif itemType == sequenceParser.eTypeFile:
                filePath = colored.green(os.path.join(filePath, filename))
            elif itemType == sequenceParser.eTypeSequence:
                # magenta is not visible without bold
                filePath = colored.magenta(os.path.join(filePath, filename), bold=True)
            elif itemType == sequenceParser.eTypeLink:
                filePath = colored.cyan(os.path.join(filePath, filename))
            else:
                filePath = colored.red(os.path.join(filePath, filename))
        filePath += ' \t'

        # sam-ls -R / sam-ls -L
        indentTree = ''
        if args.recursive and args.level != 0:
            indentTree += '|  ' * (level - 1)
            indentTree += '|__ '

        # display
        toPrint = detailed + filePath + detailedSequence
        # if first level or no tree formatting
        if level == 0 or args.level == 0:
            puts(toPrint.format())
        else:
            with indent(level, quote=indentTree):
                puts(toPrint.format())

        self._itemPrinted.append(item.getAbsoluteFilepath())

    def _printItems(self, items, args, detectionMethod, filters, level=0):
        """
        For each items, check if it should be printed, depending on the command line options.
        """
        # sam-ls --script
        if self._needToPrintCurrentFolder(args):
            puts(items[0].getFolder() + ':')

        for item in sorted(items):
            itemType = item.getType()
            toPrint = False

            # sam-ls default case: print all items
            if not args.directories and not args.files and not args.sequences:
                toPrint = True
            else:
                # sam-ls -d
                if args.directories and itemType == sequenceParser.eTypeFolder:
                    toPrint = True
                # sam-ls -f
                elif args.files and itemType == sequenceParser.eTypeFile:
                    toPrint = True
                # sam-ls -s
                elif args.sequences and itemType == sequenceParser.eTypeSequence:
                    toPrint = True

            # skip item already printed
            if toPrint and self._isAlreadyPrinted(item):
                toPrint = False

            # print current item
            if toPrint:
                self._printItem(item, args, level)

            # sam-ls -R
            if args.recursive and itemType == sequenceParser.eTypeFolder:

                # sam-ls -L
                if args.level and args.level <= level:
                    continue

                try:
                    newFolder = os.path.join(item.getFolder(), item.getFilename())
                    self.logger.debug('Browse in "' + newFolder + '" with the following filters: ' + str(filters))
                    newItems = sequenceParser.browse(newFolder, detectionMethod, filters)
                    level += 1
                    self._printItems(newItems, args, detectionMethod, filters, level)
                    level -= 1
                except IOError as e:
                    # Permission denied for example
                    self.logger.warning(e)

        # sam-ls --script
        if self._needToPrintCurrentFolder(args):
            puts(newline=True)

    def run(self, parser):
        """
        Process the list operation.
        """
        # Parse command-line
        args = parser.parse_args()

        # sam-ls --absolute-path --relative-path
        if args.absolutePath and args.relativePath:
            self._displayCommandLineHelp(parser)
            exit(1)

        # Set sam log level
        self.setLogLevel(args.verbose)

        class InputToBrowse(object):
            """
            Represents an input to browse: a path and a list of filters.
            """
            def __init__(self, inputPath):
                self.inputPath = inputPath
                self.filters = []

            def addFilter(self, filterToAdd):
                self.filters.append(filterToAdd)

        # translate user inputs to a list of InputToBrowse
        inputsToBrowse = []
        for inputPath in args.inputs:
            # if the input is a directory, add it and continue
            if os.path.isdir(inputPath):
                inputsToBrowse.append(InputToBrowse(inputPath))
                continue
            # else split the input to a path and a filename
            subPath = os.path.dirname(inputPath)
            if not subPath:
                subPath = '.'
            filename = os.path.basename(inputPath)
            # add the path and the filename as an expression
            inputToBrowse = InputToBrowse(subPath)
            inputToBrowse.addFilter(filename)
            inputsToBrowse.append(inputToBrowse)
        # if no user input, will browse in the current working directory
        if not inputsToBrowse:
            inputsToBrowse.append(InputToBrowse(os.getcwd()))

        # sam-ls -a
        detectionMethod = sequenceParser.eDetectionDefault
        if args.all:
            detectionMethod = sequenceParser.eDetectionDefaultWithDotFile

        # sam-ls --detect-negative
        if args.detectNegative:
            detectionMethod = detectionMethod | sequenceParser.eDetectionNegative

        # sam-ls --detect-without-holes
        if args.detectWithoutHoles:
            detectionMethod = detectionMethod | sequenceParser.eDetectionSequenceWithoutHoles

        # sam-ls -e
        for expression in args.expression:
            for inputToBrowse in inputsToBrowse:
                inputToBrowse.addFilter(expression)

        error = 0
        # for each input to browse, print the finding items
        for inputToBrowse in inputsToBrowse:
            items = []
            inputPath = inputToBrowse.inputPath
            filters = inputToBrowse.filters
            try:
                self.logger.debug('Browse in "' + inputPath + '" with the following filters: ' + str(filters))
                items = sequenceParser.browse(inputPath, detectionMethod, filters)
            except IOError as e:
                self.logger.debug('IOError raised: "' + str(e) + '".')
                # if the given input does not correspond to anything
                if 'No such file or directory' in str(e):
                    # try to create a sequence from the given input
                    sequence = sequenceParser.Sequence()
                    self.logger.debug('BrowseSequence on "' + inputPath + '".')
                    isSequence = sequenceParser.browseSequence(sequence, inputPath)
                    if isSequence:
                        item = sequenceParser.Item(sequence, os.getcwd())
                        # check if the sequence contains at least one element
                        if len(item.explode()):
                            items.append(item)
                    # else error
                    else:
                        self.logger.warning(e)
                        continue
                # else it's not a directory
                else:
                    self.logger.debug('Try a new browse with the given input name as filter.')
                    # new path to browse
                    newBrowsePath = os.path.dirname(inputPath)
                    if not newBrowsePath:
                        newBrowsePath = '.'
                    # new filter
                    newFilter = []
                    newFilter.extend(filters)
                    newFilter.append(os.path.basename(inputPath))
                    # new browse
                    self.logger.debug('Browse in "' + newBrowsePath + '" with the following filters: ' + str(newFilter))
                    items += sequenceParser.browse(newBrowsePath, detectionMethod, newFilter)

            if not len(items):
                self.logger.warning('No items found for input "' + inputPath + '" with the following filters: ' + str(filters))
                error = 1
            else:
                self._printItems(items, args, detectionMethod, filters)

        exit(error)


if __name__ == '__main__':
    # Create the tool
    tool = Sam_ls()

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
