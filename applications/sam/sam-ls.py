#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import os
import sys
from datetime import date
import argparse

from pySequenceParser import sequenceParser

import argcomplete
from clint.textui import colored, puts, indent


# sam-ls -R
levelPadding = 2


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


def printItem(item, directory, args, level):
    """
    Print the item depending on the command line options.
    """
    itemType = item.getType()

    filename = ''
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
        elif itemType == sequenceParser.eTypeLink:
            characterFromType = 'l'
        itemStat = sequenceParser.ItemStat(item)
        lastUpdate = date.fromtimestamp(itemStat.modificationTime).strftime('%d/%m/%y')
        detailed += characterFromType + '  ' + lastUpdate + '  ' + str(itemStat.size)
        detailed += ' \t'

    # sam-ls -s
    if args.sequences:
        sequence = item.getSequence()
        # [ begin : end ] nbFiles - nbMissingFiles
        detailedSequence += '[' + str(sequence.getFirstTime()) + ':' + str(sequence.getLastTime()) + '] '
        detailedSequence += str(sequence.getNbFiles()) + ' files'
        detailedSequence += ( ', ' + str(sequence.getNbMissingFiles() ) + ' missing files') if sequence.hasMissingFile() else ''
        detailedSequence += ' \t'

    # sam-ls --absolute-path
    if args.absolutePath:
        filename += os.path.abspath(directory) + '/'

    # sam-ls --relative-path
    if args.relativePath:
        filename += (item.getFolder() if item.getFolder()[0] != '/' else '.')
        filename += ('/' if filename[-1] != '/' else '')

    # filename
    if args.color:
        if itemType == sequenceParser.eTypeUndefined:
            filename = colored.red(filename + item.getFilename())
        elif itemType == sequenceParser.eTypeFolder:
            filename = colored.blue(filename + item.getFilename(), bold=True) # blue is not visible without bold
        elif itemType == sequenceParser.eTypeFile:
            filename = colored.green(filename + item.getFilename())
        elif itemType == sequenceParser.eTypeSequence:
            filename = colored.magenta(filename + item.getFilename(), bold=True) # magenta is not visible without bold
        elif itemType == sequenceParser.eTypeLink:
            filename = colored.cyan(filename + item.getFilename())
        else:
            filename += item.getFilename()
    else:
        filename += item.getFilename()
    filename += ' \t'

    # sam-ls -R
    indentTree = ''
    if args.recursive:
        indentTree += '|  ' * (level - levelPadding)
        indentTree += '|__ '

    # display
    toPrint = detailed + filename + detailedSequence
    # cannot use indent method with an empty quote
    if level == 0:
        puts(toPrint.format())
    else:
        with indent(level, quote=indentTree):
            puts(toPrint.format())


def manageItems(items, directory, args, level=0, detectionMethod=sequenceParser.eDetectionDefault, filters=[]):
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
            printItem(item, directory, args, level)

        # sam-ls -R
        if args.recursive and itemType == sequenceParser.eTypeFolder:
            level += levelPadding
            manageItems(sequenceParser.browse(item.getFolder() + '/' + item.getFilename(), detectionMethod, filters), directory, args, level)
            level -= levelPadding


if __name__ == '__main__':

    # Create command-line interface
    parser = argparse.ArgumentParser(
            prog='sam-ls',
            description='''
            List information about the sequences, files and folders.
            List the current directory by default, and only sequences.
            The script option disable color, disable directory printing (in multi-directory case or recursive) and set relative path by default.
            ''',
            )

    parser.add_argument('inputDirectories', nargs='*', action='store', help='list of input directories to analyse').completer = sequenceParserCompleter

    # Options
    parser.add_argument('-a', '--all', dest='all', action='store_true', help='do not ignore entries starting with .')
    parser.add_argument('-d', '--directories', dest='directories', action='store_true', help='handle directories')
    parser.add_argument('-s', '--sequences', dest='sequences', action='store_true', help='handle sequences')
    parser.add_argument('-f', '--files', dest='files', action='store_true', help='handle files')
    parser.add_argument('-l', '--long-listing', dest='longListing', action='store_true', help='use a long listing format')
    parser.add_argument('-e', '--expression', dest='expression', help='use a specific pattern, ex: *.jpg,*.png').completer = sequenceParserCompleter
    parser.add_argument('-R', '--recursive', dest='recursive', action='store_true', help='handle directories and their content recursively')
    parser.add_argument('--absolute-path', dest='absolutePath', action='store_true', help='display the absolute path of each object')
    parser.add_argument('--relative-path', dest='relativePath', action='store_true', help='display the relative path of each object')
    parser.add_argument('--color', dest='color', action='store_true', default=True, help='display the output with colors (True by default)')
    #parser.add_argument('--script', dest='script', help='format the output such as it could be dump in a file and be used as a script')

    # Activate completion
    argcomplete.autocomplete(parser)

    # Parse command-line
    args = parser.parse_args()

    # directories to scan
    inputDirectories = []
    if len( args.inputDirectories ) == 0:
        inputDirectories.append(os.getcwd())
    else:
        inputDirectories = args.inputDirectories

    # sam-ls -a
    detectionMethod = sequenceParser.eDetectionDefault
    if args.all:
        detectionMethod = sequenceParser.eDetectionDefaultWithDotFile

    # sam-ls -e
    filters = []
    if args.expression:
        filters.append(args.expression)

    # get list of items for each directory
    for inputDirectory in inputDirectories:
        items = []
        try:
            items += sequenceParser.browse(inputDirectory, detectionMethod, filters)
        except IOError as e:
            # if the given input does not correspond to anything
            if 'No such file or directory' in str(e):
                print e
                continue
            # else try a new browse on the current directory, with the given name as filter
            else:
                # create item from the given name
                fileItem = sequenceParser.Item(sequenceParser.eTypeFile, inputDirectory)
                # new browse
                filters.append(fileItem.getFilename())
                folderName = fileItem.getFolder()
                if len(folderName) == 0:
                    folderName = os.getcwd()
                items += sequenceParser.browse(folderName, detectionMethod, filters)

        manageItems(items, inputDirectory, args)
