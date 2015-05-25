#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
from datetime import date
import argparse

# python modules to easily get completion, colors, indent text...
import argcomplete
from clint.textui import colored, puts, indent

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
import common


def printItem(item, args, level):
    """
    Print the item depending on the command line options.
    """
    itemType = item.getType()

    filePath = ''
    detailed = ''
    detailedSequence = ''

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
            # [ begin : end ] nbFiles - nbMissingFiles
            sequence = item.getSequence()
            detailedSequence = '[{first}:{last}] {nbFiles} files'.format(first=sequence.getFirstTime(), last=sequence.getLastTime(), nbFiles=sequence.getNbFiles())
        elif itemType == sequenceParser.eTypeLink:
            characterFromType = 'l'
        itemStat = sequenceParser.ItemStat(item)
        lastUpdate = date.fromtimestamp(itemStat.modificationTime).strftime('%d/%m/%y')
        detailed += characterFromType + '  ' + lastUpdate + '  ' + str(itemStat.size)
        detailed += ' \t'

    # sam-ls --absolute-path
    if args.absolutePath:
        filePath += os.path.abspath(item.getFolder()) + '/'

    # sam-ls --relative-path
    if args.relativePath:
        filePath += (item.getFolder() if item.getFolder()[0] != '/' else '.')
        filePath += ('/' if filePath[-1] != '/' else '')

    # filename
    if args.color:
        if itemType == sequenceParser.eTypeUndefined:
            filePath = colored.red(os.path.join(filePath, item.getFilename()))
        elif itemType == sequenceParser.eTypeFolder:
            filePath = colored.blue(os.path.join(filePath + item.getFilename()), bold=True) # blue is not visible without bold
        elif itemType == sequenceParser.eTypeFile:
            filePath = colored.green(os.path.join(filePath + item.getFilename()))
        elif itemType == sequenceParser.eTypeSequence:
            filePath = colored.magenta(os.path.join(filePath + item.getFilename()), bold=True) # magenta is not visible without bold
        elif itemType == sequenceParser.eTypeLink:
            filePath = colored.cyan(os.path.join(filePath + item.getFilename()))
        else:
            filePath += os.path.join(filePath + item.getFilename())
    else:
        filePath += os.path.join(filePath + item.getFilename())
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


def printItems(items, args, detectionMethod, filters, level=0):
    """
    For each items, check if it should be printed, depending on the command line options.
    """
    for item in sorted(items):
        itemType = item.getType()
        toPrint = True

        # sam-ls -d
        if args.directories and itemType != sequenceParser.eTypeFolder:
            toPrint = False

        # sam-ls -f
        if args.files and itemType != sequenceParser.eTypeFile:
            toPrint = False

        # sam-ls -s
        if args.sequences and itemType != sequenceParser.eTypeSequence:
            toPrint = False

        # print current item
        if toPrint:
            printItem(item, args, level)

        # sam-ls -R
        if args.recursive and itemType == sequenceParser.eTypeFolder:

            # sam-ls -L
            if args.level and args.level <= level:
                continue

            level += 1
            newItems = sequenceParser.browse(os.path.join(item.getFolder(), item.getFilename()), detectionMethod, filters)
            printItems(newItems, args, detectionMethod, filters, level)
            level -= 1


def main(args = None):
    if not args:
        # Create command-line interface
        parser = argparse.ArgumentParser(
                prog='sam-ls',
                description='''
                List information about the sequences, files and folders.
                List the current directory by default, and only sequences.
                The script option disable color, disable directory printing (in multi-directory case or recursive) and set relative path by default.
                ''',
                )

        # Add command line arguments
        common.addLsArgumentsToParser(parser)

        # Activate completion
        argcomplete.autocomplete(parser)

        # Parse command-line
        args = parser.parse_args()

    # inputs to scan
    inputs = []
    if args.inputs:
        inputs = args.inputs
    else:
        inputs.append(os.getcwd())

    # sam-ls -a
    detectionMethod = sequenceParser.eDetectionDefault
    if args.all:
        detectionMethod = sequenceParser.eDetectionDefaultWithDotFile

    # sam-ls --detect-negative
    if args.detectNegative:
        detectionMethod = detectionMethod | sequenceParser.eDetectionNegative

    # sam-ls -e
    filters = []
    if args.expression:
        filters.append(args.expression)

    # get list of items for each inputs
    for input in inputs:
        items = []
        try:
            items = sequenceParser.browse(input, detectionMethod, filters)
        except IOError as e:
            # if the given input does not correspond to anything
            if 'No such file or directory' in str(e):
                print e
                continue
            # else it's not a directory: try a new browse with the given input name as filter
            else:
                # new path to browse
                newBrowsePath = os.path.dirname(input)
                if not newBrowsePath:
                    newBrowsePath = '.'
                # new filter
                newFilter = []
                newFilter.extend(filters)
                newFilter.append(os.path.basename(input))
                # new browse
                items += sequenceParser.browse(newBrowsePath, detectionMethod, newFilter)

        printItems(items, args, detectionMethod, filters)


if __name__ == '__main__':
    main()
