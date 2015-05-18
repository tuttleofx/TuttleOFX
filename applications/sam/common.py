#!/usr/bin/env python

import os
import argparse

import argcomplete

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


def addMvCpArgumentsToParser(parser):
    """
    Create common arguments/options to sam-mv and sam-cp tools.
    """
    # Arguments
    parser.add_argument('inputs', nargs='*', action='store', help='list of input directories to move').completer = sequenceParserCompleter

    # Options
    parser.add_argument('-o', '--offset', dest='offset', type=int, help='retime the sequence with the given offset')
    parser.add_argument('--input-first', dest='inputFirst', type=int, help='specify the first input image in order to select a sub-range of the input sequence')
    parser.add_argument('--input-last', dest='inputLast', type=int, help='specify the last input image in order to select a sub-range of the input sequence')
    parser.add_argument('--output-first', dest='outputFirst', type=int, help='specify the first output image, in order to select a sub-range of the output sequence')
    parser.add_argument('--output-last', dest='outputLast', type=int, help='specify the last input image in order to select a sub-range of the output sequence')
    parser.add_argument('--remove-holes', dest='removeHoles', action='store_true', help='remove holes of the sequence')
    parser.add_argument('--detect-negative', dest='detectNegative', action='store_true', help='detect negative numbers instead of detecting "-" as a non-digit character (False by default)')


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
