#!/usr/bin/env python
# coding: utf-8 

import os
import copy

# python module to get colors, indent text...
from clint.textui import colored, progress
from clint import __version__ as clintVersion

# parser of sequence
from pySequenceParser import sequenceParser

# openFX host
from pyTuttle import tuttle

# sam common functions
from common import samUtils


class SplitCmd:
    """
    A dedicated class to expose the given input split command as a list of graph.
    It contains a list of SplitCmdGraph.
    @note resolve given input/output folders.
    """
    def __init__(self, inputCommandLine, noRecursivity):
        self._graph = []
        self._recursive = not noRecursivity

        # check if there is a command
        if not inputCommandLine:
            return

        # Get general graph from the command line
        generalGraph = SplitCmdGraph(inputCommandLine)

        # if the user gives an input directory
        inputIsFolder = False
        if generalGraph.hasReader():
            firstReader = generalGraph.getFirstReader()
            inputFilename = firstReader.getFilename()
            inputIsFolder = sequenceParser.getTypeFromPath(inputFilename) == sequenceParser.eTypeFolder
            if inputIsFolder:
                # create filter
                filters = []
                inputExtName, inputExtValue = firstReader.getArgument('ext')
                if inputExtName:
                    filters.append('*.' + inputExtValue)
                # browse in the given directory
                self._browseFolder(generalGraph, inputFilename, filters)
                # if no graph has been added when browse folder, add the general graph
                # the user could display the help of a plugin
                if not len(self._graph):
                    self._graph.append(generalGraph)
            else:
                self._graph.append(generalGraph)
        else:
            self._graph.append(generalGraph)

        # remove arguments of command line which are not for the plugins
        for graph in self._graph:
            if graph.hasReader():
                graph.getFirstReader().removeArgument('ext')
            if graph.hasWriter():
                graph.getLastWriter().removeArgument('ext')

    def getGraphs(self):
        return self._graph

    def _browseFolder(self, graph, inputFolder, filters):
        """
        Browse filesystem and append a new graph for each files/sequences.
        If the script runs this function, the graph has at least a reader with a folder as filename parameter.
        """
        items = sequenceParser.browse(inputFolder, sequenceParser.eDetectionDefault, filters)
        for item in items:
            itemType = item.getType()
            if itemType == sequenceParser.eTypeFile or itemType == sequenceParser.eTypeSequence or itemType == sequenceParser.eTypeLink:
                # create a new graph
                newGraph = copy.deepcopy(graph)
                newGraph.getFirstReader().setFilename(item.getAbsoluteFilepath())
                if newGraph.hasWriter():
                    lastWriter = newGraph.getLastWriter()
                    lastWriter.setFilename(os.path.join(lastWriter.getFilename(), item.getFilename()))
                    # if the user add a filter of output extensions
                    outputExtName, outputExtValue = graph.getLastWriter().getArgument('ext')
                    if outputExtName:
                        outputFilename = newGraph.getLastWriter().getFilename()
                        newGraph.getLastWriter().setFilename(outputFilename[:outputFilename.rfind('.')+1] + outputExtValue)
                # add the new graph
                self._graph.append(newGraph)
            elif itemType == sequenceParser.eTypeFolder:
                # sam-do --recursive
                if self._recursive:
                    newGraph = copy.deepcopy(graph)
                    newGraph.getFirstReader().setFilename(item.getAbsoluteFilepath())
                    if newGraph.hasWriter():
                        newGraph.getLastWriter().setFilename(os.path.join(graph.getLastWriter().getFilename(), item.getFilename()))
                    self._browseFolder(newGraph, item.getAbsoluteFilepath(), filters)


class SplitCmdGraph:
    """
    A dedicated class to expose the given input split command as a graph.
    It contains a list of SplitCmdNode.
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
                self.addNode(SplitCmdNode(input))
                newPlugin = False
            else:
                # new argument to the last added node
                paramName = None
                paramValue = ''
                if '=' in input:
                    paramName, paramValue = input.split('=')
                else:
                    paramValue = input
                self._nodes[-1].addArgument(paramName, paramValue)

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

    def getFirstReader(self):
        """
        Get the first reader node.
        Return None if there is no reader.
        """
        for node in self._nodes:
            if node.isReader():
                return node
        return None

    def getLastWriter(self):
        """
        Get the last writer node.
        Return None if there is no writer.
        """
        for node in reversed(self._nodes):
            if node.isWriter():
                return node
        return None

    def hasReader(self):
        """
        True if there is at least one reader in the graph.
        """
        if self.getFirstReader():
            return True
        return False

    def hasWriter(self):
        """
        True if there is at least one writer in the graph.
        """
        if self.getLastWriter():
            return True
        return False


class SplitCmdNode:
    """
    A dedicated class to expose each part of the command split as a node.
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

    def getPluginName(self, logger):
        """
        Return complete node name from the pluginId and its arguments.
        Plugin's arguments can be used to get best reader/writer.
        @note Get best reader if the given name is 'r'.
        @note Get best writer if the given name is 'w'.
        @exception if cannot find the plugin name which corresponds to the plugin id
        """
        if self.isGenericReader() or self.isGenericWriter():
            if len(self._arguments) == 0:
                logger.warning('Cannot guess the best reader/writer node without any filename specified.')
            # get filename
            filename = self._arguments[0][1]
            # return best reader
            if self.isGenericReader():
                bestReader = tuttle.getBestReader(filename)
                logger.info('Use "' + bestReader + '" to read "' + filename + '".')
                return bestReader
            # return best writer
            elif self.isGenericWriter():
                bestWriter = tuttle.getBestWriter(filename)
                logger.info('Use "' + bestWriter + '" to write "' + filename + '".')
                return bestWriter
        else:
            pluginsMap = tuttle.core().getImageEffectPluginCache().getPluginsByID()
            # get plugins name which match with the given id
            pluginNames = []
            for pluginName in pluginsMap:
                if self._pluginId in pluginName:
                    pluginNames.append(pluginName)
            # one plugin matches
            if len(pluginNames) == 1:
                return pluginNames[0]
            # more than one matches
            elif len(pluginNames) > 1:
                logger.warning('Cannot guess the best match for plugin name "' + self._pluginId +'".')
                logger.warning('Possible choices are: "' + ', '.join(pluginNames) +'".')
        # cannot find a best reader/writer or no plugin name matches with the id
        raise RuntimeError('Plugin with id "' + self._pluginId + '" not found.')

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

    def isReader(self):
        """
        From the plugin name, guess if it's a reader.
        """
        if self.isGenericReader():
            return True
        if 'reader' in self._pluginId:
            return True
        return False

    def isWriter(self):
        """
        From the plugin name, guess if it's a writer.
        """
        if self.isGenericWriter():
            return True
        if 'writer' in self._pluginId:
            return True
        return False

    def getArguments(self):
        return self._arguments

    def getArgument(self, name):
        """
        If not found, returns (None, None)
        """
        for argName, argvalue in self._arguments:
            if argName == name:
                return (argName, argvalue)
        return (None, None)

    def getFilename(self):
        """
        If not found, return an empty string.
        Used for reader/writer nodes.
        """
        for argName, argvalue in self._arguments:
            if argName == 'filename':
                return argvalue
        # get first arg if it has no name
        if len(self._arguments):
            return self._arguments[0][1]
        return ''

    def setFilename(self, value):
        """
        If not found, do nothing.
        Used for reader/writer nodes.
        """
        for i in range(0, len(self._arguments)):
            argName = self._arguments[i][0]
            if argName == 'filename':
                self._arguments[i] = ('filename', value)
        # set first arg if it has no name
        if self._arguments[0][0] is None:
            self._arguments[0] = ('filename', value)

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


class ProgressHandle(tuttle.IProgressHandle):
    """
    Progress handle to get progress of process.
    """
    def __init__(self, ranges):
        super(ProgressHandle, self).__init__()

        # Get number of frames to compute
        nbFramesToCompute = 0
        for timeRange in ranges:
            nbFramesToCompute += (timeRange._end - timeRange._begin + 1) / timeRange._step
        expectedSize = (nbFramesToCompute if nbFramesToCompute < samUtils.getMaxInt() else 1)

        # Create progress bar
        if clintVersion >= '0.3.5':
            self._progress = progress.Bar(expected_size=expectedSize)
            self._counter = 1
        else:
            self._it = list(range(0, expectedSize+1))
            self._progress = progress.bar(self._it, expected_size=expectedSize)
            self._progress.next()

    def processAtTime(self):
        if clintVersion >= '0.3.5':
            self._progress.show(self._counter)
            self._counter += 1
        else:
            self._progress.next()

    def endSequence(self):
        if clintVersion >= '0.3.5':
            self._progress.done()


def samDoCompleter(prefix, parsed_args, **kwargs):
    """
    Custom Completer to manage auto competion when looking for openFX nodes.
    @warning The autocompletion works only for TuttleOFX plugins.
    """
    # preload OFX plugins (to have auto completion of plugins name, their parameters...)
    tuttle.core().preload(True)

    # get plugins
    pluginsId = tuttle.core().getImageEffectPluginCache().getPluginsByID()
    pluginsStr = [str(id).replace('tuttle.', '') for id in pluginsId]

    # check last input in command line
    if len(parsed_args.inputs):
        lastInput = parsed_args.inputs[-1]
        # if last input is a plugin, return its parameters
        if lastInput in pluginsStr:
            graph = tuttle.Graph()
            node = graph.createNode('tuttle.'+lastInput)
            params = node.getParams()
            paramsStr = [str(param.getScriptName()) for param in params]
            return paramsStr
        elif lastInput == '//':
            return pluginsStr
        else:
            for input in reversed(parsed_args.inputs):
                # if an input is a plugin, get its parameters
                if input in pluginsStr:
                    graph = tuttle.Graph()
                    node = graph.createNode('tuttle.'+input)
                    params = node.getParams()
                    paramsStr = [str(param.getScriptName()) for param in params]
                    # if last input is one of its parameters, return its choices
                    if lastInput in paramsStr:
                        param = node.getParam(lastInput)
                        if param.getProperties().hasProperty('OfxParamPropChoiceOption'):
                            propChoiceOption = param.getProperties().fetchProperty('OfxParamPropChoiceOption')
                            choicesStr = samUtils.getListValues(propChoiceOption)
                            return choicesStr
                    # else, return its parameters
                    else:
                        return paramsStr
    # else return available plugins
    return pluginsStr
