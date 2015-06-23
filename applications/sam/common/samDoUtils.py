#!/usr/bin/env python

import argparse
import copy

# python modules to easily get colors, indent text...
from clint.textui import colored, puts, progress

# parser of sequence
from pySequenceParser import sequenceParser

# openFX host
from pyTuttle import tuttle


class CommandSplit:
    """
    A dedicated class to expose the given input command split as a list of graph.
    It contains a list of CommandSplitGraph.
    @note resolve given input folders.
    """
    def __init__(self, inputCommandLine):
        self._graph = []

        generalGraph = CommandSplitGraph(inputCommandLine)
        # if the user gives an input directory
        inputArgName = generalGraph.getFirstNode().getArguments()[0][1]
        inputIsFolder = sequenceParser.getTypeFromPath(inputArgName) == sequenceParser.eTypeFolder
        # if the user gives an output directory
        outputArgName = generalGraph.getLastNode().getArguments()[0][1]
        outputIsFolder = sequenceParser.getTypeFromPath(outputArgName) == sequenceParser.eTypeFolder

        if inputIsFolder:
            # create a new commandSplitGraph for each files/sequences in directory
            filters = []
            # if the user add a filter of input extensions
            extName, extValue = generalGraph.getFirstNode().getArgument('ext')
            if extName:
                filters.append('*.' + extValue)
                generalGraph.getFirstNode().removeArgument(extName)

            items = sequenceParser.browse(inputArgName, sequenceParser.eDetectionDefault, filters)
            for item in items:
                itemType = item.getType()
                if itemType == sequenceParser.eTypeFile or itemType == sequenceParser.eTypeSequence:
                    newGraph = copy.deepcopy(generalGraph)
                    newGraph.getFirstNode().getArguments()[0] = (None, item.getAbsoluteFilepath())
                    if outputIsFolder:
                        newGraph.getLastNode().getArguments()[0] = (None, newGraph.getLastNode().getArguments()[0][1] + item.getFilename())
                    self._graph.append(newGraph)
        else:
            self._graph.append(generalGraph)

    def getGraphs(self):
        return self._graph


class CommandSplitGraph:
    """
    A dedicated class to expose the given input command split as a graph.
    It contains a list of CommandSplitNode.
    @note resolve full name of plugins.
    """
    def __init__(self, inputCommandLine):
        self._nodes = []

        newPlugin = True
        for input in inputCommandLine:
            # split command line for each new '//'
            if input == '//':
                newPlugin = True
                continue
            # new node in the command
            if newPlugin:
                self.addNode(CommandSplitNode(input))
                newPlugin = False
            else:
                # new argument to the last added node
                paramName = None
                paramValue = ''
                if '=' in input:
                    paramName, paramValue = input.split('=')
                else:
                    paramValue = input
                self.getLastNode().addArgument(paramName, paramValue)

    def __str__(self):
        outputStr = ''
        for i in range(0, len(self._nodes)):
            outputStr += str(i) + ": " + str(self._nodes[i])
        return outputStr

    def addNode(self, commandSplitNode):
        self._nodes.append(commandSplitNode)

    def getNodes(self):
        return self._nodes

    def getNode(self, index):
        return self._nodes[index]

    def getFirstNode(self):
        return self._nodes[0]

    def getLastNode(self):
        return self._nodes[-1]


class CommandSplitNode:
    """
    A dedicated class to expose each part of the split command as a node.
    """
    def __init__(self, pluginId):
        self._pluginId = pluginId
        self._arguments = []

    def __str__(self):
        outputStr = ''
        outputStr += colored.green(self._pluginId) + '\n'
        for i in range(0, len(self._arguments)):
            argName, argValue = self._arguments[i]
            outputStr += '{:} -> {:}\n'.format(argName if argName else 'arg at '+str(i), str(argValue))
        return outputStr

    def getPluginName(self):
        """
        Return complete node name from the pluginId and its arguments.
        Plugin's arguments can be used to get best reader/writer.
        @note Get best reader if the given name is 'r'.
        @note Get best writer if the given name is 'w'.
        """
        pluginsMap = tuttle.core().getImageEffectPluginCache().getPluginsByID()
        if self._pluginId in pluginsMap:
            return self._pluginId
        else:
            if self.isGenericReader() or self.isGenericWriter():
                if len(self._arguments) == 0:
                    puts(colored.red('Cannot guess the best reader/writer node without any filename specified.'))
                    return ''
                # get filename
                filename = self._arguments[0][1]
                # return best reader
                if self.isGenericReader():
                    bestReader = tuttle.getBestReader(filename)
                    puts(colored.green('Use "' + bestReader + '" to read "' + filename + '".'))
                    return bestReader
                # return best writer
                elif self.isGenericWriter():
                    bestWriter = tuttle.getBestWriter(filename)
                    puts(colored.green('Use "' + bestWriter + '" to write "' + filename + '".'))
                    return bestWriter
            return 'tuttle.' + self._pluginId

    def isGenericReader(self):
        """
        Is the plugin corresponds to a generic reader to guess.
        """
        pluginIdLower = self._pluginId.lower()
        if pluginIdLower == 'r' or pluginIdLower == 'reader':
            return True
        return False

    def isGenericWriter(self):
        """
        Is the plugin corresponds to a generic writer to guess.
        """
        pluginIdLower = self._pluginId.lower()
        if pluginIdLower == 'w' or pluginIdLower == 'writer':
            return True
        return False

    def getArguments(self):
        return self._arguments

    def getArgument(self, name):
        """
        If not found, return (None, None)
        """
        for argName, argvalue in self._arguments:
            if argName == name:
                return (argName, argvalue)
        return (None, None)

    def addArgument(self, name, value):
        self._arguments.append((name, value))

    def removeArgument(self, name):
        for argName, argvalue in self._arguments:
            if argName == name:
                self._arguments.remove((argName, argvalue))

    def hasHelp(self):
        argumentValues = [argument[1] for argument in self._arguments]
        if '-h' in argumentValues or '--help' in argumentValues:
            return True
        return False


class SamDoSetVerboseAction(argparse.Action):
    """
    Class to get the corresponding tuttle verbose level from the user input.
    The user input can be a number or a string.
    """
    def __init__(self, option_strings, dest, nargs=None, **kwargs):
        if nargs is not None:
            raise ValueError("nargs not allowed")
        super(SamDoSetVerboseAction, self).__init__(option_strings, dest, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        # if the given number is upper than the max tuttle verbose level, set verbose to trace
        try:
            if int(values) > 6:
                setattr(namespace, self.dest, 0)
                return
        except Exception:
            pass

        if values == '0' or values.lower() == 'none':
            setattr(namespace, self.dest, None)
        elif values == '1' or values.lower() == 'fatal':
            setattr(namespace, self.dest, 5)
        elif values == '2' or values.lower() == 'error':
            setattr(namespace, self.dest, 4)
        elif values == '3' or values.lower() == 'warn':
            setattr(namespace, self.dest, 3)
        elif values == '4' or values.lower() == 'info':
            setattr(namespace, self.dest, 2)
        elif values == '5' or values.lower() == 'debug':
            setattr(namespace, self.dest, 1)
        elif values == '6' or values.lower() == 'trace':
            setattr(namespace, self.dest, 0)
        # if the level is not recognized, no verbosity
        else:
            setattr(namespace, self.dest, None)


class ProgressHandle(tuttle.IProgressHandle):
    """
    Progress handle to get progress of process.
    """
    def __init__(self, ranges):
        super(ProgressHandle, self).__init__()
        nbFramesToCompute = 0
        for range in ranges:
            nbFramesToCompute += (range._end - range._begin + 1) / range._step
        self._progress = progress.Bar(expected_size=(nbFramesToCompute if (nbFramesToCompute and nbFramesToCompute <= getMaxInt()) else 1))
        self._counter = 1

    def processAtTime(self):
        self._progress.show(self._counter)
        self._counter += 1

    def endSequence(self):
        self._progress.done()


def samDoCompleter(prefix, parsed_args, **kwargs):
    """
    Custom Completer to manage auto competion when looking for openFX nodes.
    """
    # get plugins
    pluginsId = tuttle.core().getImageEffectPluginCache().getPluginsByID()
    pluginsStr = [str(id) for id in pluginsId]

    # check last input in command line
    if len(parsed_args.inputs):
        lastInput = parsed_args.inputs[-1]
        # if last input is a plugin, return its parameters
        if lastInput in pluginsStr:
            graph = tuttle.Graph()
            node = graph.createNode(lastInput)
            params = node.getParams()
            paramsStr = [str(param.getScriptName()) for param in params]
            return paramsStr
        else:
            for input in reversed(parsed_args.inputs):
                # if an input is a plugin, get its parameters
                if input in pluginsStr:
                    graph = tuttle.Graph()
                    node = graph.createNode(input)
                    params = node.getParams()
                    paramsStr = [str(param.getScriptName()) for param in params]
                    # if last input is one of its parameters, return its choices
                    if lastInput in paramsStr:
                        param = node.getParam(lastInput)
                        if param.getProperties().hasProperty('OfxParamPropChoiceOption'):
                            propChoiceOption = param.getProperties().fetchProperty('OfxParamPropChoiceOption')
                            choicesStr = getListValues(propChoiceOption)
                            return choicesStr
                    # else, return its parameters
                    else:
                        return paramsStr
    # else return available plugins
    return pluginsStr


def getMaxInt():
    """
    Returns C/C++ max int
    """
    return pow(2, 31)-1
