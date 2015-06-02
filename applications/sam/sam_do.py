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
        self.description = '''
            A command line to execute a list of OpenFX nodes.
            Use the separator // to pipe images between nodes.
            sam do [options]... [// node [node-options]... [[param=]value]...]... [// [options]...]
            '''
        self.epilog = '''
    Plugins options
        Plugin list:                       sam do --nodes
        Plugin help:                       sam do blur -h

    Generators and viewers
        Viewer:                            sam do reader in.@.dpx // viewer
        Print:                             sam do reader in.@.dpx // print color=full16ansi
        Constant generator:                sam do constant                // viewer
        White constant generator:          sam do constant color=1,1,1,1  // viewer
        HD constant generator:             sam do constant size=1920,1080 // viewer
        Checkerboard generator:            sam do checkerboard            // viewer
        Checkerboard generator:            sam do checkerboard width=500  // viewer
        Checkerboard generator:            sam do checkerboard width=1920 ratio=2.35 // viewer
        Text writing:                      sam do constant // text text="hello" size=80 // viewer

    Image sequence conversion and creation
        Convert Image:                     sam do reader in.dpx // writer out.jpg
        Convert Sequence:                  sam do reader in.####.dpx // writer out.####.jpg
        Select a range:                    sam do reader in.####.dpx // writer out.####.jpg // --range=10,100
                                           r and w are shortcuts for reader and writer

    Geometry processing during conversion
        Crop:                              sam do reader in.####.dpx // crop x1=20 x2=1000 y1=10 y2=300 // writer out.jpg
        Fill:                              sam do reader in.####.dpx // crop y1=10 y2=1060 mode=fill color=0.43,0.67,0.50 // writer out.jpg
        Resize:                            sam do reader in.####.dpx // resize size=1920,1080 // writer out.####.jpg
        Upscaling:                         sam do reader in.####.dpx // resize size=1920,1080 filter=lanczos  // writer out.####.jpg
        Downscaling:                       sam do reader in.####.dpx // resize size=720,576   filter=mitchell // writer out.####.jpg

    Color processing during conversion
        Lut :                              sam do reader in.####.dpx // lut lutFile.3dl // writer out.jpg
        CTL:                               sam do reader in.####.dpx // ctl file=ctlCode.ctl // writer out.####.jpg
        Gamma:                             sam do reader in.####.dpx // gamma master=2.2 // writer out.####.jpg

    Image Sequence Numbering
        Frames with or without padding:    image.@.jpg
        Frames 1 to 100 padding 4:         image.####.jpg -or- image.@.jpg
        Frames 1 to 100 padding 5:         image.#####.jpg
        Printf style padding 4:            image.%04d.jpg
        All Frames in Directory:           /path/to/directory

    Processing options
        Range process:                     sam do reader in.@.dpx // writer out.@.exr // --range 50,100
        Single process:                    sam do reader in.@.dpx // writer out.@.exr // --range 59
        Multiple CPUs:                     sam do reader in.@.dpx // writer out.@.exr // --nb-cores 4
        Continues whatever happens:        sam do reader in.@.dpx // writer out.@.exr // --continueOnError
        '''

    def fillParser(self, parser):
        # Arguments
        parser.add_argument('inputs', nargs='*', action='store', help='command line to process').completer = samUtils.sequenceParserCompleter

        # Options
        parser.add_argument('-r', '--range', dest='range', nargs=2, type=int, help='specify the range to process')
        parser.add_argument('-n', '--nodes', dest='nodes', action='store_true', help='list all avalaible nodes')
        parser.add_argument('--continue-on-error', dest='continueOnError', action='store_true', default=False, help='continue the process even if errors occured')
        parser.add_argument('--stop-on-missing-files', dest='stopOnMissingFiles', action='store_true', default=False, help='stop the process if missing files')
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

    def _getNbBitsFromOfxBitDepth(self, ofxBitDepth):
        if ofxBitDepth == 'OfxBitDepthByte':
            return '8 bits'
        elif ofxBitDepth == 'OfxBitDepthShort':
            return '16 bits'
        elif ofxBitDepth == 'OfxBitDepthFloat':
            return '32 bits'
        else:
            return 'unknown'

    def _displayParamHelp(self, param):
        # Choice param
        if param.getParamType() == 'OfxParamTypeChoice':
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
                            puts('{value:50} {label}'.format(
                                value=(colored.yellow(choiceValue) if choiceValues.index(choiceValue) == defaultValueIndex else colored.red(choiceValue)),
                                label=(choiceLabelValues[choiceValues.index(choiceValue)] if hasLabel else '')))
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
                    default=colored.yellow(','.join(defaultValue))),
                    newline=False)

                if len(minDisplayValue) and len(maxDisplayValue):
                    puts('[{minDisplay:5} --> {maxDisplay:5}]'.format(
                        minDisplay=','.join(minDisplayValue),
                        maxDisplay=','.join(maxDisplayValue)))
                else:
                    puts('\n')

                with indent(2):
                    puts(param.getHint())

    def _displayClipHelp(self, clip):
        # Components
        components = clip.getSupportedComponents()
        componentsStr = []
        for component in components:
            componentsStr.append(component[17:])  # remove 'OfxImageComponent'
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
            puts('{clipName:10}: {clipComponents} {clipProperties}'.format(
                clipName=colored.green(clip.getName()),
                clipComponents=('[' + (', '.join(componentsStr)) + ']'),
                clipProperties=(', '.join(properties))))

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
            if paramType == 'OfxParamTypeGroup' or paramType == 'OfxParamTypePushButton' or paramType == 'OfxParamTypePage':
                continue
            self._displayParamHelp(param)

        # CLIPS
        puts('\n' + colored.blue('CLIPS', bold=True))
        for clip in node.getClipImageSet().getClips():
            self._displayClipHelp(clip)

        # SUPPORTED BIT DEPTH
        if node.getProperties().hasProperty('OfxImageEffectPropSupportedPixelDepths'):
            puts('\n' + colored.blue('SUPPORTED BIT DEPTH', bold=True))
            propBitDepth = node.getProperties().fetchProperty('OfxImageEffectPropSupportedPixelDepths')
            bitDepthValues = self._getListValues(propBitDepth)
            bitDepthSourceStr = []
            bitDepthOutputStr = []
            for bitDepthValue in bitDepthValues[:len(bitDepthValues)/2]:
                bitDepthSourceStr.append(self._getNbBitsFromOfxBitDepth(bitDepthValue))
            for bitDepthValue in bitDepthValues[len(bitDepthValues)/2:]:
                bitDepthOutputStr.append(self._getNbBitsFromOfxBitDepth(bitDepthValue))
            # Print
            with indent(4):
                puts('{name:10}: [{bitdepth}]'.format(
                    name=colored.green('Source'),
                    bitdepth=', '.join(bitDepthSourceStr)))
                puts('{name:10}: [{bitdepth}]'.format(
                    name=colored.green('Output'),
                    bitdepth=', '.join(bitDepthOutputStr)))

    def run(self, parser):
        """
        Process the do operation.
        """
        # Activate completion
        argcomplete.autocomplete(parser)

        # Parse command-line
        args, unknown = parser.parse_known_args()

        # preload OFX plugins
        tuttle.core().preload(False)

        # sam-do --nodes
        if args.nodes:
            for plugin in tuttle.core().getPlugins():
                puts(plugin.getIdentifier().ljust(30) + ' (v' + str(plugin.getVersionMajor()) + '.' + str(plugin.getVersionMinor()) + ')')
            exit(0)

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
                    param.setValueFromExpression(optionValue)
                elif option[0] != '-':
                    try:
                        # set parameter at index written in the command line
                        param = node.getParams()[options.index(option)]
                        param.setValueFromExpression(option)
                    except Exception:
                        # cannot set param of node
                        pass
            # sam-do node --help
            if '-h' in options or '--help' in options:
                self._displayNodeHelp(nodeFullName, node)
                exit(0)
            nodes.append(node)

        # Options of process
        options = tuttle.ComputeOptions()
        # sam-do --verbose
        options.setVerboseLevel(args.verbose)
        # sam-do --range
        if args.range:
            options.setBegin(args.range[0])
            options.setEnd(args.range[1])
        # sam-do --continue-on-error
        options.setContinueOnError(args.continueOnError)
        # sam-do --stop-on-missing-files
        options.setContinueOnMissingFile(not args.stopOnMissingFiles)

        # Connect and compute
        if len(nodes) > 1:
            graph.connect(nodes)
            graph.compute(nodes[-1], options)


if __name__ == '__main__':
    # Create the tool
    tool = SamDo()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        epilog=tool.epilog,
        formatter_class=argparse.RawTextHelpFormatter,
        add_help=False,
        )

    # Run the command
    tool.fillParser(parser)
    tool.run(parser)
