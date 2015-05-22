#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
import argparse

# python modules to easily get completion, colors, indent text...
import argcomplete
from clint.textui import colored, puts, indent

# parser of sequence
from pySequenceParser import sequenceParser

# sam common functions
import common


def removeItem(item, args):
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
            puts(colored.red('Error: you cannot remove a folder which contains elements like this. If you still want to, see "-R" option.'))
        return
    
    # remove other things than sequences (file, link...)
    if itemType != sequenceParser.eTypeSequence:
        os.remove(os.path.join(filePath, item.getFilename()))
        return

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
    for time in range(first, last + 1):
        try:
            filePathInSequence = os.path.join(filePath, sequence.getFilenameAt(time))
            os.remove(os.path.join(filePathInSequence))
        except OSError:
            puts(colored.red('Error: cannot find file "' + filePathInSequence + '" in sequence.'))


def removeItems(items, args, detectionMethod, filters):
    """
    For each items, check if it should be removed, depending on the command line options.
    """
    folderItems = []

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
            removeItems(newItems, args, detectionMethod, filters)

        if toRemove:
            # store folder and delete it after all other elements
            if itemType == sequenceParser.eTypeFolder:
                folderItems.insert(0, item)
            # remove current item
            else:
                removeItem(item, args)

    # remove folders (which are empty)
    for folder in folderItems:
        removeItem(item, args)


if __name__ == '__main__':

    # Create command-line interface
    parser = argparse.ArgumentParser(
            prog='sam-rm',
            description='''
            Remove file sequences.
            Remove sequence of files, and could remove trees (folder, files and sequences).
            ''',
            )

    parser.add_argument('inputs', nargs='+', action='store', help='list of input files/sequences/directories to remove').completer = common.sequenceParserCompleter

    # Options
    common.addCommonFilterArgumentsToParser(parser)
    parser.add_argument('-R', '--recursive', dest='recursive', action='store_true', help='handle directories and their content recursively')
    parser.add_argument('--first-image', dest='firstImage', type=int, help='specify the first image of sequence')
    parser.add_argument('--last-image', dest='lastImage', type=int, help='specify the last image of sequence')
    parser.add_argument('--range', dest='range', nargs=2, type=int, help='specify the range of sequence')
    common.addDetectNegativeArgumentToParser(parser)

    # Activate completion
    argcomplete.autocomplete(parser)

    # Parse command-line
    args = parser.parse_args()

    # check command line
    if args.range and (args.firstImage is not None or args.lastImage is not None):
        puts(colored.red('Error: you cannot cumulate multiple options to specify the range of sequence.'))
        exit(-1)

    if '.' in args.inputs or '..' in args.inputs:
        puts(colored.red('Error: you cannot remove folders "." or "..".'))
        exit(-1)

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

        removeItems(items, args, detectionMethod, filters)