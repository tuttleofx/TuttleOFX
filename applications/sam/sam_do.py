#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import argparse
import itertools

# python modules to easily get completion
import argcomplete
from clint.textui import colored, puts, indent

# parser of sequence
from pySequenceParser import sequenceParser

# openFX host
from pyTuttle import tuttle

# sam common functions
from common import samUtils, samDoUtils


class Sam_do(samUtils.Sam):
    """
    Class which represents the sam_do operation.
    """

    def __init__(self):
        samUtils.Sam.__init__(self)
        self.command = 'do'
        self.help = 'to execute a list of OpenFX nodes'
        self.description = str(colored.green('''
            A command line to execute a list of OpenFX nodes.
            Use the separator // to pipe images between nodes.
            sam do [options]... [// node [node-options]... [[param=]value]...]... [options]
            '''))
        self.epilog = '''
    '''+colored.blue('Plugins options', bold=True)+'''
        Plugin list:                       sam do --nodes
        Supported file formats list:       sam do --file-formats
        Plugin help:                       sam do blur -h

    '''+colored.blue('Generators and viewers', bold=True)+'''
        Viewer:                            sam do reader in.@.dpx // viewer
        Print:                             sam do reader in.@.dpx // print color=full16ansi
        Constant generator:                sam do constant                // viewer
        White constant generator:          sam do constant color=1,1,1,1  // viewer
        HD constant generator:             sam do constant size=1920,1080 // viewer
        Checkerboard generator:            sam do checkerboard            // viewer
        Checkerboard generator:            sam do checkerboard width=500  // viewer
        Checkerboard generator:            sam do checkerboard width=1920 ratio=2.35 // viewer
        Text writing:                      sam do constant // text text="hello" size=80 // viewer

    '''+colored.blue('Image sequence conversion and creation', bold=True)+'''
        Convert Image:                     sam do reader in.dpx // writer out.jpg
        Convert Sequence:                  sam do reader in.####.dpx // writer out.####.jpg
        Select a range:                    sam do reader in.####.dpx // writer out.####.jpg --ranges 10 100
        Select several ranges:             sam do reader in.####.dpx // writer out.####.jpg --ranges 10 100 150 200
                                           'r' and 'w' are shortcuts for 'reader' and 'writer'

    '''+colored.blue('Geometry processing during conversion', bold=True)+'''
        Crop:                              sam do reader in.dpx // crop x1=20 x2=1000 y1=10 y2=300 // writer out.jpg
        Fill:                              sam do reader in.dpx // crop y1=10 y2=1060 mode=fill color=0.43,0.67,0.50,1 // writer out.jpg
        Resize:                            sam do reader in.####.dpx // resize size=1920,1080 // writer out.####.jpg
        Upscaling:                         sam do reader in.####.dpx // resize size=1920,1080 filter=lanczos  // writer out.####.jpg
        Downscaling:                       sam do reader in.####.dpx // resize size=720,576   filter=mitchell // writer out.####.jpg

    '''+colored.blue('Color processing during conversion', bold=True)+'''
        Lut :                              sam do reader in.####.dpx // ocio.lut lutFile.3dl // writer out.jpg
        CTL:                               sam do reader in.####.dpx // ctl file=ctlCode.ctl // writer out.####.jpg
        Gamma:                             sam do reader in.####.dpx // gamma master=2.2 // writer out.####.jpg

    '''+colored.blue('Image Sequence Numbering', bold=True)+'''
        Frames with or without padding:    image.@.jpg
        Frames 1 to 100 padding 4:         image.####.jpg -or- image.@.jpg
        Frames 1 to 100 padding 5:         image.#####.jpg
        Printf style padding 4:            image.%04d.jpg

    '''+colored.blue('Processing options', bold=True)+'''
        Range process:                     sam do reader in.@.dpx // writer out.@.exr --ranges 50 100
        Single process:                    sam do reader in.@.dpx // writer out.@.exr --ranges 59
        Continues whatever happens:        sam do reader in.@.dpx // writer out.@.exr --continue-on-error
        '''

        # preload OFX plugins (to have auto completion of plugins name, their parameters...)
        tuttle.core().preload(False)

    def fillParser(self, parser):
        # Arguments
        parser.add_argument('inputs', nargs='*', action='store', help='command line to process').completer = samDoUtils.samDoCompleter

        # Options
        parser.add_argument('-r', '--ranges', dest='ranges', nargs='+', type=int, help='specify the ranges to process (only numbers separate with spaces)')
        parser.add_argument('-n', '--nodes', dest='nodes', action='store_true', help='list all avalaible nodes')
        parser.add_argument('--recursive', dest='recursive', action='store_true', default=False, help='Enable recursivity when using directory as input/output')
        parser.add_argument('--file-formats', dest='fileFormats', action='store_true', help='list all supported file formats (R/W)')
        parser.add_argument('--continue-on-error', dest='continueOnError', action='store_true', default=False, help='continue the process even if errors occured')
        parser.add_argument('--stop-on-missing-files', dest='stopOnMissingFiles', action='store_true', default=False, help='stop the process if missing files')
        parser.add_argument('--no-plugin-cache', dest='noPluginCache', action='store_true', default=False, help='load plugins without using the cache file')
        parser.add_argument('--rebuild-plugin-cache', dest='rebuildPluginCache', action='store_true', default=False, help='load plugins and rebuild the cache file')
        parser.add_argument('-v', '--verbose', dest='verbose', action=samDoUtils.SamDoSetVerboseAction, help='verbose level (0/none(by default), 1/fatal, 2/error, 3/warn, 4/info, 5/debug, 6(or upper)/trace)')
        # parser.add_argument('-h', '--help', dest='help', action='store_true', help='show this help message and exit')

    def _setTimeRanges(self, computeOptions, ranges):
        """
        Set time ranges of the given compute options.
        """
        def grouper(iterable, n, fillValue=None):
            """
            Collect data into fixed-length chunks or blocks.
            """
            args = [iter(iterable)] * n
            return itertools.izip_longest(*args, fillvalue=fillValue)

        for begin, end in grouper(ranges, 2):
            if end is None:
                end = begin
            computeOptions.addTimeRange(begin, end)

    def _getNbBitsFromOfxBitDepth(self, ofxBitDepth):
        """
        Get a label 'nb bits' from 'OfxBitDepth__' name.
        """
        if ofxBitDepth == 'OfxBitDepthByte':
            return '8 bits'
        elif ofxBitDepth == 'OfxBitDepthShort':
            return '16 bits'
        elif ofxBitDepth == 'OfxBitDepthFloat':
            return '32 bits'
        else:
            return 'unknown'

    def _displayTitle(self, title):
        """
        Display the given string with a title format.
        """
        puts('\n' + colored.blue(title, bold=True))

    def _displayPlugins(self):
        """
        Display all available plugins (in alphabetical order), with their versions (v[major].[minor]).
        """
        plugins = []
        for plugin in tuttle.core().getPlugins():
            plugins.append(plugin.getIdentifier().ljust(30) + ' (v' + str(plugin.getVersionMajor()) + '.' + str(plugin.getVersionMinor()) + ')')
        for plugin in sorted(plugins):
            puts(plugin)

    def _displayFileFormats(self):
        """
        Display all supported input/output file formats.
        """
        def getListOfSupportedExtension(ofxhImageEffectNodeDescriptor):
            """
            Return list of supported extension from a given plugin descriptor.
            """
            propSupportedExtension = ofxhImageEffectNodeDescriptor.getParamSetProps().fetchProperty('TuttleOfxImageEffectPropSupportedExtensions')
            return samUtils.getListValues(propSupportedExtension)

        supportedExtensions = {'r': [], 'w': []}
        for plugin in tuttle.core().getImageEffectPluginCache().getPlugins():
            plugin.loadAndDescribeActions()
            if plugin.supportsContext('OfxImageEffectContextReader'):
                pluginDescriptor = plugin.getDescriptorInContext('OfxImageEffectContextReader')
                if pluginDescriptor.getParamSetProps().hasProperty('TuttleOfxImageEffectPropSupportedExtensions'):
                    supportedExtensions['r'].extend(getListOfSupportedExtension(pluginDescriptor))
            elif plugin.supportsContext('OfxImageEffectContextWriter'):
                pluginDescriptor = plugin.getDescriptorInContext('OfxImageEffectContextWriter')
                if pluginDescriptor.getParamSetProps().hasProperty('TuttleOfxImageEffectPropSupportedExtensions'):
                    supportedExtensions['w'].extend(getListOfSupportedExtension(pluginDescriptor))
        for key, extensions in supportedExtensions.items():
            if key == 'r':
                self._displayTitle('SUPPORTED INPUT FILE FORMATS')
            elif key == 'w':
                self._displayTitle('SUPPORTED OUTPUT FILE FORMATS')
            puts(', '.join(sorted(extensions)))

    def _displayParamHelp(self, param):
        """
        Display help of the given OFXParameter.
        """
        # Choice param
        if param.getParamType() == 'OfxParamTypeChoice':
            defaultValueIndex = None
            if param.getProperties().hasProperty('OfxParamPropDefault'):
                propDefault = param.getProperties().fetchProperty('OfxParamPropDefault')
                defaultValue = samUtils.getListValues(propDefault)
                if propDefault.getType() == tuttle.ePropTypeInt:
                    defaultValueIndex = param.getProperties().getIntProperty('OfxParamPropDefault', 0)

            choiceValues = []
            choiceLabelValues = []
            hasLabel = False
            if param.getProperties().hasProperty('OfxParamPropChoiceOption'):
                propChoiceOption = param.getProperties().fetchProperty('OfxParamPropChoiceOption')
                choiceValues = samUtils.getListValues(propChoiceOption)
            if param.getProperties().hasProperty('OfxParamPropChoiceLabelOption'):
                propChoiceLabel = param.getProperties().fetchProperty('OfxParamPropChoiceLabelOption')
                choiceLabelValues = samUtils.getListValues(propChoiceLabel)
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
                defaultValue = samUtils.getListValues(propDefault)

            minDisplayValues = []
            maxDisplayValues = []
            # min/max values
            if param.getProperties().hasProperty('OfxParamPropDisplayMin'):
                propDisplayMin = param.getProperties().fetchProperty('OfxParamPropDisplayMin')
                propDisplayMax = param.getProperties().fetchProperty('OfxParamPropDisplayMax')
                minDisplayValues = samUtils.getListValues(propDisplayMin)
                maxDisplayValues = samUtils.getListValues(propDisplayMax)
                # check inf
                for i in range(0, len(maxDisplayValues)):
                    if propDisplayMax.getType() == tuttle.ePropTypeInt:
                        if int(maxDisplayValues[i]) >= samUtils.getMaxInt():
                            maxDisplayValues[i] = 'inf'
                    elif propDisplayMax.getType() == tuttle.ePropTypeDouble:
                        if float(maxDisplayValues[i]) >= samUtils.getMaxInt():
                            maxDisplayValues[i] = 'inf'
                # check -inf
                for i in range(0, len(minDisplayValues)):
                    if propDisplayMax.getType() == tuttle.ePropTypeInt:
                        if int(minDisplayValues[i]) <= -samUtils.getMaxInt()-1:
                            minDisplayValues[i] = '-inf'
                    elif propDisplayMax.getType() == tuttle.ePropTypeDouble:
                        if float(minDisplayValues[i]) <= -samUtils.getMaxInt()-1:
                            minDisplayValues[i] = '-inf'
            hasMinMaxValues = len(minDisplayValues) > 0 and len(minDisplayValues) == len(maxDisplayValues)

            # Print
            with indent(4):
                puts('{paramName:50}: {paramType:10} {default:9}'.format(
                    paramName=colored.green(param.getScriptName()), paramType=param.getParamTypeName(),
                    default=colored.yellow(','.join(defaultValue))),
                    newline=(False if hasMinMaxValues else True))

                if hasMinMaxValues:
                    puts('[{minDisplay:5} --> {maxDisplay:5}]'.format(
                        minDisplay=','.join(minDisplayValues),
                        maxDisplay=','.join(maxDisplayValues)))

                with indent(2):
                    puts(param.getHint())

    def _displayClipHelp(self, clip):
        """
        Display help of the given OFXClip.
        """
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
        self._displayTitle('NODE')
        with indent(4):
            puts(colored.green(nodeFullName + ' ' + node.getVersionStr()))

        # DESCRIPTION
        self._displayTitle('DESCRIPTION')
        with indent(4):
            puts('type: ' + colored.green(tuttle.mapNodeTypeEnumToString(node.getNodeType())))
            puts('group: ' + colored.green(node.asImageEffectNode().getPluginGrouping()))
        puts('\n')
        puts(node.getProperties().fetchStringProperty('OfxPropPluginDescription').getValue())

        # PARAMETERS
        if node.getParams():
            self._displayTitle('PARAMETERS')
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
        self._displayTitle('CLIPS')
        for clip in node.getClipImageSet().getClips():
            self._displayClipHelp(clip)

        # SUPPORTED BIT DEPTH
        if node.getProperties().hasProperty('OfxImageEffectPropSupportedPixelDepths'):
            self._displayTitle('SUPPORTED BIT DEPTH')
            propBitDepth = node.getProperties().fetchProperty('OfxImageEffectPropSupportedPixelDepths')
            bitDepthValues = samUtils.getListValues(propBitDepth)
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

    def _getTuttleGraph(self, splitCmd):
        """
        Return the tuple (tuttle graph, its nodes) which corresponds to the given split command.
        """
        graph = tuttle.Graph()
        nodes = []
        connections = []

        for splitCmdNode in splitCmd.getNodes():
            # Create a tuttle node
            nodeFullName = splitCmdNode.getPluginName()
            node = None
            try:
                node = graph.createNode(nodeFullName)
            except Exception as e:
                # Plugin not found
                puts(colored.red('Cannot create node "' + nodeFullName + '": the node will be skipped from the command line.'))
                print e
                continue
            # sam-do node --help
            if splitCmdNode.hasHelp():
                self._displayNodeHelp(nodeFullName, node)
                exit(0)
            # Set its parameters
            for argName, argValue in splitCmdNode.getArguments():
                try:
                    param = None
                    if argName:
                        param = node.getParam(argName)
                    else:
                        param = node.getParams()[splitCmdNode.getArguments().index((argName, argValue))]
                    param.setValueFromExpression(argValue)
                except Exception as e:
                    # if id for connection
                    if argName == 'id':
                        pass
                    # if clip name, create connection
                    elif argName in [clip.getName() for clip in node.getClipImageSet().getClips()]:
                        try:
                            graph.connect(nodes[connections.index(argValue)], node.getAttribute(argName))
                        except Exception as e:
                            # Cannot connect attribute of node
                            puts(colored.red('Cannot connect attribute "' 
                                + argName + '" of node "' + nodeFullName 
                                + '" to id "' + argValue 
                                + '": the connection will be skipped from the command line.'))
                            print e
                    else:
                        # Cannot set param of node
                        puts(colored.red('Cannot set '
                            + (('parameter "' + argName + '" of ') if argName else '')
                            + 'node "' + nodeFullName + '" ' 
                            + (('to value "' + argValue + '"') if argValue else '') 
                            + ': the parameter will be skipped from the command line.'))
                        print e
            nodes.append(node)
            connections.append(splitCmdNode.getArgument('id')[1])

        # Create in line connections
        for i in range(0, len(connections)):
            if connections[i] is None and i+1 < len(connections):
                graph.connect(nodes[i], nodes[i+1])

        # return tuple (graph, nodes)
        return (graph, nodes)

    def run(self, parser):
        """
        Process the do operation.
        """
        # Parse command-line
        args, unknown = parser.parse_known_args()

        # Add sam logger
        self.addConsoleHandler(args.verbose)

        # Clear plugin cache
        if args.rebuildPluginCache:
            tuttle.core().getPluginCache().clearPluginFiles()

        # preload OFX plugins
        if args.noPluginCache:
            tuttle.core().preload(False)
        else:
            tuttle.core().preload(True)

        # sam-do --nodes
        if args.nodes:
            self._displayPlugins()
            exit(0)

        # sam-do --file-formats
        if args.fileFormats:
            self._displayFileFormats()
            exit(0)

        # sam-do --help
        if len(args.inputs) == 0:
            subparser = samUtils.getSubParser(parser, 'do')
            if subparser is not None:
                puts(subparser.format_help())
                exit(0)
            parser.print_help()
            exit(0)

        # Add unknown options to the command line to process
        args.inputs.extend(unknown)

        # Split command line
        splitCmd = samDoUtils.SplitCmd(args.inputs, args.recursive)
        graphsWithNodes = [self._getTuttleGraph(splitCmdGraph) for splitCmdGraph in splitCmd.getGraphs()]

        if not graphsWithNodes:
            puts(colored.red('Error: nothing to compute.'))

        # Compute the corresponding tuttle graphs
        for graph, nodes in graphsWithNodes:
            # Options of process
            options = tuttle.ComputeOptions()
            # sam-do --verbose
            if args.verbose is not None:
                options.setVerboseLevel(args.verbose)
            # sam-do --ranges
            if args.ranges is not None:
                self._setTimeRanges(options, args.ranges)

            # sam-do --continue-on-error
            options.setContinueOnError(args.continueOnError)
            # sam-do --stop-on-missing-files
            options.setContinueOnMissingFile(not args.stopOnMissingFiles)
            # Set progress handle
            ranges = options.getTimeRanges()
            if not len(ranges):
                # get time domaine
                try:
                    timeDomain = nodes[0].asImageEffectNode().computeTimeDomain()
                    ranges = []
                    ranges.append(tuttle.TimeRange(int(timeDomain.min), int(timeDomain.max), 1))
                except Exception as e:
                    # the first added node has no filename set
                    pass
            progress = samDoUtils.ProgressHandle(ranges)
            options.setProgressHandle(progress)

            # Connect and compute
            if len(nodes) > 1:
                graph.compute(nodes[-1], options)
                self.logger.info('Memory usage: ' + str(int(samUtils.memoryUsageResource())) + 'KB')


if __name__ == '__main__':
    # Create the tool
    tool = Sam_do()

    # Create command-line interface
    parser = argparse.ArgumentParser(
        prog='sam-'+tool.command,
        description=tool.description,
        epilog=tool.epilog,
        formatter_class=argparse.RawTextHelpFormatter,
        add_help=False,
        )
    tool.fillParser(parser)

    # Activate completion
    argcomplete.autocomplete(parser)

    # Run the command
    tool.run(parser)
