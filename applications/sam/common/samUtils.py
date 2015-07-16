#!/usr/bin/env python
# coding: utf-8 

import os
import logging
import argparse

# python module to get completions
import argcomplete

# parser of sequence
from pySequenceParser import sequenceParser


class Sam(object):
    """
    Based class for all sam operations.
    """

    def __init__(self):
        self.command = 'command'
        self.help = ''
        self.description = ''
        self.epilog = ''

        # create logger
        self.logger = logging.getLogger('SAM')
        self.logger.setLevel(logging.DEBUG)

        # Add a console handler
        self.addConsoleHandler()

    def fillParser(self, parser):
        """
        To fill the given parser.
        """
        raise NotImplementedError

    def run(self, parser):
        """
        To process the sam operation
        """
        raise NotImplementedError

    def addConsoleHandler(self, tuttleVerboseLevel=None):
        """
        Add handler to log in console.
        """
        # create console handler
        ch = logging.StreamHandler()
        # set a default log level
        ch.setLevel(logging.ERROR)

        # create formatter
        formatter = logging.Formatter("%(asctime)s - %(name)s %(levelname)s - %(message)s")
        # set formatter
        ch.setFormatter(formatter)
        # add console handler to logger
        self.logger.addHandler(ch)

    def setLogLevel(self, tuttleVerboseLevel):
        """
        Set log level of all handlers of the logger.
        """
        if tuttleVerboseLevel is not None:
            for handler in self.logger.handlers:
                handler.setLevel(10*tuttleVerboseLevel) # fit to python logging levels


def completion():
    """
    If the script is executed to generate autocomplete choices.
    """
    return '_ARGCOMPLETE' and 'COMP_LINE' in os.environ


def doCompletion(parser):
    """
    Activate completion by using argcomplete module.
    If the script is running by argcomplete to collect completions, it prints them to the output stream, and exits.
    Else, the function does nothing.
    """
    if completion():
        argcomplete.autocomplete(parser, always_complete_options=False)


def sequenceParserCompleter(prefix, parsed_args, **kwargs):
    """
    Custom Completer to manage auto competion when looking for sequences.
    """
    directory = '.'
    if parsed_args.inputs and len(parsed_args.inputs):
        directory = os.path.abspath(parsed_args.inputs[-1])
    items = sequenceParser.browse(directory, sequenceParser.eDetectionDefault)
    itemsStr = [str(item.getFilename()) for item in items]
    return itemsStr


def getListValues(ofxProperty):
    """
    Get list of string from the given OfxProperty
    """
    elements = []
    for n in range(0, ofxProperty.getDimension()):
        if ofxProperty.getStringValueAt(n):
            elements.append(ofxProperty.getStringValueAt(n))
    return elements


def getReadableSize(size):
    """
    Get a human readable string which represents the given size.
    """
    sizeExtension = ''
    if (size / pow(2, 30)) > 0:
        sizeExtension = 'G'
        size = size / (pow(2, 30) * 1.0)
    elif (size / pow(2, 20)) > 0:
        sizeExtension = 'M'
        size = size / (pow(2, 20) * 1.0)
    elif (size / pow(2, 10)) > 0:
        sizeExtension = 'K'
        size = size / (pow(2, 10) * 1.0)
    return str(round(size, 1)) + sizeExtension


def getSequenceNameWithFormatting(sequence, formatChosen):
    """
    Return the sequence name with a specific formatting (from nuke, rv...).
    """
    sequenceName = sequence.getPrefix()
    if formatChosen == 'rv':
        sequenceName += (str(sequence.getFirstTime()) + '-' + str(sequence.getLastTime()))
        sequenceName += '@' * sequence.getPadding()
        if not sequence.getPadding():
            sequenceName += '@'  # no padding
    elif formatChosen == 'nuke':
        sequenceName += '%0' + str(sequence.getPadding()) + 'd'
    else:  # default formatting
        sequenceName += '#' * sequence.getPadding()
        if not sequence.getPadding():
            sequenceName += '@'  # no padding
    sequenceName += sequence.getSuffix()
    return sequenceName


def getSubParser(parser, subSamCommand):
    """
    Get the subparser which corresponds to the given sam command, from the given parser.
    """
    # retrieve subparsers from parser
    subparsers_actions = [
        action for action in parser._actions
        if isinstance(action, argparse._SubParsersAction)]
    for subparsers_action in subparsers_actions:
        # get all subparsers and print help
        for choice, subparser in subparsers_action.choices.items():
            if choice != subSamCommand:
                continue
            return subparser
    return None


def memoryUsageResource():
    """
    Use resource module, which is part of the standard Python library, to return RAM used by the python script.
    In kilobytes.
    """
    import resource
    import sys
    rusage_denom = 1024.
    if sys.platform == 'darwin':
        # ... it seems that in OSX the output is different units ...
        rusage_denom = rusage_denom * rusage_denom
    mem = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss / rusage_denom
    return mem


def getMaxInt():
    """
    Returns C/C++ max int
    """
    return pow(2, 31)
