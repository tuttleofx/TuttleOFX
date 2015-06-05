#!/usr/bin/env python

import os
import argparse

# python modules to easily get colors, indent text...
from clint.textui import colored, puts

# openFX host
from pyTuttle import tuttle

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

def samDoCompleter(prefix, **kwargs):
    """
    Custom Completer to manage auto competion when looking for openFX nodes.
    """    
    tuttle.core().preload(False)
    plugins = tuttle.core().getPlugins()
    pluginsStr = [str(plugin.getIdentifier()) for plugin in plugins]
    return pluginsStr

def retrieveNodeFullName(pluginId):
    """
    Return complete node name from the given id.
    """
    pluginIdLower = pluginId.lower()
    pluginsMap = tuttle.core().getImageEffectPluginCache().getPluginsByID()
    if pluginIdLower in pluginsMap:
        return pluginId
    else:
        return 'tuttle.' + pluginIdLower

def getReadableSize(size):
    """
    Get a human readable string which represents the given size.
    """
    sizeExtension = ''
    if (size / pow(2,30)) > 0:
        sizeExtension = 'G'
        size = size / (pow(2,30) * 1.0)
    elif (size / pow(2,20)) > 0:
        sizeExtension = 'M'
        size = size / (pow(2,20) * 1.0)
    elif (size / pow(2,10)) > 0:
        sizeExtension = 'K'
        size = size / (pow(2,10) * 1.0)
    return str(round(size, 1)) + sizeExtension

def getSequenceNameWithFormatting(sequence, formatChosen):
    """
    Return the sequence name with a specific formatting (from nuke, rv...).
    """
    sequenceName = sequence.getPrefix()
    if formatChosen == 'rv':
        sequenceName += '@' * sequence.getPadding()
    elif formatChosen == 'nuke':
        sequenceName += '%' + str(sequence.getPadding()) + 'd'
    else:  # default formatting
        sequenceName += '#' * sequence.getPadding()
    sequenceName += sequence.getSuffix()
    return sequenceName

def getSequenceItemFromPath(inputPath, detectNegative):
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
    if len(inputItems) != 1:
        puts(colored.red('Error: no existing file corresponds to the given input sequence: ' + inputPath))
        exit(-1)

    inputItem = inputItems[0]
    if inputItem.getType() != sequenceParser.eTypeSequence:
        puts(colored.red('Error: input is not a sequence: ', inputItem.getFilename()))
        exit(-1)

    return inputItem

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
