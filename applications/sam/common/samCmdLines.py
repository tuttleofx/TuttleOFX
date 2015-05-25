#!/usr/bin/env python

import os
import argparse

# python modules to easily get completion
import argcomplete


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
    parser.add_argument('-e', '--expression', dest='expression', help='use a specific pattern, ex: "*.jpg", "*.png"').completer = sequenceParserCompleter


def addMvCpArgumentsToParser(parser):
    """
    Add common arguments/options to command line of sam-mv and sam-cp tools.
    """
    # Arguments
    parser.add_argument('inputs', metavar='INPUTS', nargs='+', action='store', help='list of inputs').completer = sequenceParserCompleter
    parser.add_argument('output', metavar='OUTPUT', help='the output to write').completer = sequenceParserCompleter

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

    addDetectNegativeArgumentToParser(parser)


def addLsArgumentsToParser(parser):
    # Arguments
    parser.add_argument('inputs', nargs='*', action='store', help='list of files/sequences/directories to analyse').completer = sequenceParserCompleter

    # Options
    addCommonFilterArgumentsToParser(parser)
    parser.add_argument('-l', '--long-listing', dest='longListing', action='store_true', help='use a long listing format (display in this order: type | permissions | owner | group | last update | minSize | maxSize | totalSize | name)')
    parser.add_argument('--format', dest='format', choices=['default', 'nuke', 'rv'], default='default', help='specify formatting of the sequence padding')
    parser.add_argument('-R', '--recursive', dest='recursive', action='store_true', help='handle directories and their content recursively')
    parser.add_argument('-L', '--level', dest='level', type=int, help='max display depth of the directory tree (without formatting if 0)')
    parser.add_argument('--absolute-path', dest='absolutePath', action='store_true', help='display the absolute path of each object')
    parser.add_argument('--relative-path', dest='relativePath', action='store_true', help='display the relative path of each object')
    parser.add_argument('--color', dest='color', action='store_true', default=True, help='display the output with colors (True by default)')
    addDetectNegativeArgumentToParser(parser)
    #parser.add_argument('--script', dest='script', help='format the output such as it could be dump in a file and be used as a script')


def addRmArgumentsToParser(parser):
    # Arguments
    parser.add_argument('inputs', nargs='+', action='store', help='list of input files/sequences/directories to remove').completer = sequenceParserCompleter

    # Options
    addCommonFilterArgumentsToParser(parser)
    parser.add_argument('-R', '--recursive', dest='recursive', action='store_true', help='handle directories and their content recursively')
    parser.add_argument('--first-image', dest='firstImage', type=int, help='specify the first image of sequence')
    parser.add_argument('--last-image', dest='lastImage', type=int, help='specify the last image of sequence')
    parser.add_argument('--range', dest='range', nargs=2, type=int, help='specify the range of sequence')
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
    if args.offset and (args.outputFirst is not None or args.outputLast is not None):
        puts(colored.red('Error: you cannot cumulate multiple options to modify the time.'))
        exit(-1)

    return args
