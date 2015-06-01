#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK

import argparse

# python modules to easily get completion
import argcomplete
from clint.textui import colored, puts, indent

# openFX host
from pyTuttle import tuttle

# sam common functions
from common import samUtils


class SamDo(samUtils.Sam):
    """
    Class which represents the sam_do operation.
    """

    def __init__(self):
        samUtils.Sam.__init__(self)
        self.command = 'do'
        self.help = 'do, to execute a list of OpenFX nodes'

    def fillParser(self, parser):
        # Arguments
        parser.add_argument('inputs', nargs='*', action='store', help='command line to process').completer = samUtils.sequenceParserCompleter

        # Options
        parser.add_argument('-n', '--nodes', dest='nodes', action='store_true', help='list all avalaible nodes')
        parser.add_argument('-v', '--verbose', dest='verbose', type=int, default=2, help='verbose level (trace=0, debug=1, info=2 (by default), warning=3, error=4, fatal=5)')
        # parser.add_argument('-h', '--help', dest='help', action='store_true', help='show this help message and exit')

    def _decomposeCommandLine(self, inputs):
        """
        Split command line in dict with {plugin: relative options}
        """
        pluginsWithOption = []
        newPlugin = True
        for input in inputs:
            # split command line for each new '//'
            if input == '//':
                newPlugin = True
                continue
            if newPlugin:
                pluginsWithOption.append((input, []))
                newPlugin = False
            else:
                pluginsWithOption[-1][1].append(input)
        return pluginsWithOption

    def _getListValues(self, ofxProperty):
        """
        Get list of string from the given property
        """
        elements = []
        for n in range(0, ofxProperty.getDimension()):
            if ofxProperty.getStringValueAt(n):
                elements.append(ofxProperty.getStringValueAt(n))
        return elements

    def _displayNodeHelp(self, nodeFullName, node):
        """
        Display help of a specific node in the command line.
        """
        # NODE
        puts('\n' + colored.blue('NODE', bold=True))
        with indent(4):
            puts(colored.green(nodeFullName + ' ' + node.getVersionStr()))

        # DESCRIPTION
        puts('\n' + colored.blue('DESCRIPTION', bold=True))
        with indent(4):
            puts('type: ' + colored.green(tuttle.mapNodeTypeEnumToString(node.getNodeType())))
            puts('group: ' + colored.green(node.asImageEffectNode().getPluginGrouping()))
        puts('\n')
        puts(node.getProperties().fetchStringProperty('OfxPropPluginDescription').getValue())

        # PARAMETERS
        if node.getParams():
            puts('\n' + colored.blue('PARAMETERS', bold=True))
        for param in node.getParams():
            # Skip secret params
            if param.getSecret():
                continue

            paramType = param.getParamType()
            # Skip Group / PushButton / Page params
            if type == 'OfxParamTypeGroup'  or type == 'OfxParamTypePushButton' or type == 'OfxParamTypePage':
                continue

            # Choice param
            if paramType == 'OfxParamTypeChoice':
                defaultValueIndex = None
                if param.getProperties().hasProperty('OfxParamPropDefault'):
                    propDefault = param.getProperties().fetchProperty('OfxParamPropDefault')
                    defaultValue = self._getListValues(propDefault)
                    if propDefault.getType() == tuttle.ePropTypeInt:
                        defaultValueIndex = param.getProperties().getIntProperty('OfxParamPropDefault', 0)

                choiceValues = []
                choiceLabelValues = []
                hasLabel = False
                if param.getProperties().hasProperty('OfxParamPropChoiceOption'):
                    propChoiceOption = param.getProperties().fetchProperty('OfxParamPropChoiceOption')
                    choiceValues = self._getListValues(propChoiceOption)
                if param.getProperties().hasProperty('OfxParamPropChoiceLabelOption'):
                    propChoiceLabel = param.getProperties().fetchProperty('OfxParamPropChoiceLabelOption')
                    choiceLabelValues = self._getListValues(propChoiceLabel)
                    hasLabel = (len(choiceValues) == len(choiceLabelValues))

                # Print
                with indent(4):
                    puts('{name:50}: {type:10} '.format(name=colored.green(param.getScriptName()), type=param.getParamTypeName()))
                    if len(choiceValues):
                        with indent(40):
                            for choiceValue in choiceValues:
                                if choiceValues.index(choiceValue) == defaultValueIndex:
                                    puts(colored.red(choiceValue))
                                else:
                                    puts(choiceValue)
                    with indent(2):
                        puts(param.getHint())
            # Other param types
            else:
                defaultValue = []
                if param.getProperties().hasProperty('OfxParamPropDefault'):
                    propDefault = param.getProperties().fetchProperty('OfxParamPropDefault')
                    defaultValue = self._getListValues(propDefault)

                minDisplayValue = []
                maxDisplayValue = []
                # min/max values
                if param.getProperties().hasProperty('OfxParamPropDisplayMin'):
                    propDisplayMin = param.getProperties().fetchProperty('OfxParamPropDisplayMin')
                    propDisplayMax = param.getProperties().fetchProperty('OfxParamPropDisplayMax')
                    minDisplayValue = self._getListValues(propDisplayMin)
                    maxDisplayValue = self._getListValues(propDisplayMax)

                # Print
                with indent(4):
                    puts('{paramName:50}: {paramType:10} {default:9}'.format(
                        paramName=colored.green(param.getScriptName()), paramType=param.getParamTypeName(),
                        default=','.join(defaultValue)),
                        newline=False)

                    if len(minDisplayValue) and len(maxDisplayValue):
                        puts('[{minDisplay:5} --> {maxDisplay:5}]'.format(
                            minDisplay=','.join(minDisplayValue),
                            maxDisplay=','.join(maxDisplayValue)))
                    else:
                        puts('\n')

                    with indent(2):
                        puts(param.getHint())

        # CLIPS
        puts('\n' + colored.blue('CLIPS', bold=True))
        for clip in node.getClipImageSet().getClips():
            # Components
            components = clip.getSupportedComponents()
            componentsStr = []
            for component in components:
                componentsStr.append(component[17:]) # remove 'OfxImageComponent'
            # Property
            properties = []
            if clip.getProperties().getIntProperty('OfxImageClipPropOptional'):
                properties.append('optional')
            if clip.isMask():
                properties.append('mask')
            if clip.temporalAccess():
                properties.append('use temporal access')
            # Print
            with indent(4):
                puts('{clipName:10}: {clipComponents} {clipProperties}'.format(clipName=colored.green(clip.getName()), clipComponents=('[' + (', '.join(componentsStr)) + ']'), clipProperties=((', '.join(properties)))))

    def run(self, parser):
        """
        Process the do operation.
        """
        # Activate completion
        argcomplete.autocomplete(parser)

        # Parse command-line
        args, unknown = parser.parse_known_args()

        # sam-do --help
        if len(args.inputs) == 0:
            # if launched from sam, retrieve subparsers from parser
            subparsers_actions = [
                action for action in parser._actions
                if isinstance(action, argparse._SubParsersAction)]
            for subparsers_action in subparsers_actions:
                # get all subparsers and print help
                for choice, subparser in subparsers_action.choices.items():
                    if choice != 'do':
                        continue
                    puts(subparser.format_help())
                    exit(0)
            parser.print_help()
            exit(0)

        # Add unknown options to the command line to process
        args.inputs.extend(unknown)

        # preload OFX plugins
        tuttle.core().preload(False)

        # sam-do --nodes
        if args.nodes:
            for plugin in tuttle.core().getPlugins():
                puts(plugin.getIdentifier().ljust(30) + " (v" + str( plugin.getVersionMajor() ) + "." + str( plugin.getVersionMinor() ) + ")")
            exit(0)

        # Tuttle graph
        graph = tuttle.Graph()
        nodes = []

        # Create nodes from command line
        pluginsWithOption = self._decomposeCommandLine(args.inputs)
        for plugin, options in pluginsWithOption:
            nodeFullName = 'tuttle.' + plugin
            node = graph.createNode(nodeFullName)
            for option in options:
                if '=' in option:
                    optionName, optionValue = option.split('=')
                    param = node.getParam(optionName)
                    param.setValue(optionValue)
                elif option[0] != '-':
                    try:
                        # set parameter at index written in the command line
                        param = node.getParams()[options.index(option)]
                        param.setValue(option)
                    except Exception:
                        # cannot set param of node
                        pass
            # sam-do --help
            if '-h' in options:
                self._displayNodeHelp(nodeFullName, node)
                exit(0)
            nodes.append(node)

        # Options of process
        options = tuttle.ComputeOptions()
        # sam-do --verbose
        if args.verbose:
            options.setVerboseLevel(args.verbose)

        # Connect and compute
        if len(nodes) > 1:
            graph.connect(nodes)
            graph.compute(nodes[-1], options)


if __name__ == '__main__':
    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-do',
        description='''
        A command line to execute a list of OpenFX nodes.
        Use the separator // to pipe images between nodes.
        sam do [options]... [// node [node-options]... [[param=]value]...]... [// [options]...]
        ''',
        formatter_class=argparse.RawTextHelpFormatter,
        add_help=False,
        )

    # Create and run the command
    command = SamDo()
    command.fillParser(parser)
    command.run(parser)
