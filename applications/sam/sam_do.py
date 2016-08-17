#!/usr/bin/env python
# coding: utf-8 
# PYTHON_ARGCOMPLETE_OK

import sys
import argparse
import itertools

from clint.textui import colored, puts, indent
from clint import __version__ as clintVersion

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

        self._pluginOption = colored.blue('Plugins options')
        self._generatorsAndViewers = colored.blue('Generators and viewers')
        self._imgSeqConversion = colored.blue('Image sequence conversion and creation')
        self._imgSeqNumbering = colored.blue('Image Sequence Numbering')
        self._geometryPorcessing = colored.blue('Geometry processing during conversion')
        self._colorProcessing = colored.blue('Color processing during conversion')
        self._processingOptions = colored.blue('Processing options')
        self._tuttleVersion = colored.green('TuttleOFX project [v' + str(tuttle.TUTTLE_HOST_VERSION_MAJOR)+'.'+str(tuttle.TUTTLE_HOST_VERSION_MINOR)+'.'+str(tuttle.TUTTLE_HOST_VERSION_MICRO)+']')
        if clintVersion >= '0.3.3':
            self._pluginOption.bold=True
            self._generatorsAndViewers.bold=True
            self._imgSeqConversion.bold=True
            self._imgSeqNumbering.bold=True
            self._geometryPorcessing.bold=True
            self._colorProcessing.bold=True
            self._processingOptions.bold=True
            self._tuttleVersion.bold=True

        self.epilog = '''
    ''' + self._pluginOption + '''
        Plugin list:                       sam do --nodes
        Supported file formats list:       sam do --file-formats
        Plugin help:                       sam do blur -h

    ''' + self._generatorsAndViewers + '''
        Viewer:                            sam do reader in.@.dpx // viewer
        Print:                             sam do reader in.@.dpx // print color=full16ansi
        Constant generator:                sam do constant                // viewer
        White constant generator:          sam do constant color=1,1,1,1  // viewer
        HD constant generator:             sam do constant size=1920,1080 // viewer
        Checkerboard generator:            sam do checkerboard            // viewer
        Checkerboard generator:            sam do checkerboard width=500  // viewer
        Checkerboard generator:            sam do checkerboard width=1920 ratio=2.35 // viewer
        Text writing:                      sam do constant // text text="hello" size=80 // viewer

    ''' + self._imgSeqConversion + '''
        Convert Image:                     sam do reader in.dpx // writer out.jpg
        Convert Sequence:                  sam do reader in.####.dpx // writer out.####.jpg
        Select a frame:                    sam do reader in.####.dpx // writer out.####.jpg --ranges 10
        Select a range:                    sam do reader in.####.dpx // writer out.####.jpg --ranges 10 100
        Select several ranges:             sam do reader in.####.dpx // writer out.####.jpg --ranges 10 100 150 200
        Convert files in directory:        sam do reader inDir // writer outDir ext=jpg
        Convert some files in directory:   sam do reader inDir ext=dpx // writer outDir ext=jpg
        Notes:                             'reader' / 'r' could be use to automatically choose the best reader.
                                           'writer' / 'w' could be use to automatically choose the best writer.
                                           Convert files in a directory also copy the input tree structure to the output.

    ''' + self._imgSeqNumbering + '''
        Frames with or without padding:    image.@.jpg
        Frames 1 to 100 padding 4:         image.####.jpg -or- image.@.jpg
        Frames 1 to 100 padding 5:         image.#####.jpg
        Printf style padding 4:            image.%04d.jpg

    ''' + self._geometryPorcessing + '''
        Crop:                              sam do reader in.dpx // crop x1=20 x2=1000 y1=10 y2=300 // writer out.jpg
        Fill:                              sam do reader in.dpx // crop y1=10 y2=1060 mode=fill color=0.43,0.67,0.50,1 // writer out.jpg
        Resize:                            sam do reader in.####.dpx // resize size=1920,1080 // writer out.####.jpg
        Upscaling:                         sam do reader in.####.dpx // resize width=1920 filter=lanczos // writer out.####.jpg
        Downscaling:                       sam do reader in.####.dpx // resize width=720 filter=mitchell // writer out.####.jpg

    ''' + self._colorProcessing + '''
        Lut :                              sam do reader in.####.dpx // ocio.lut lutFile.3dl // writer out.jpg
        CTL:                               sam do reader in.####.dpx // ctl file=ctlCode.ctl // writer out.####.jpg
        Gamma:                             sam do reader in.####.dpx // gamma master=2.2 // writer out.####.jpg
        Color Gradation:                   sam do reader in.####.dpx // colorgradation in=Linear out=Rec709 // writer out.####.jpg

    ''' + self._processingOptions + '''
        Range process:                     sam do reader in.@.dpx // writer out.@.exr --ranges 50 100
        Single process:                    sam do reader in.@.dpx // writer out.@.exr --ranges 59
        Continues whatever happens:        sam do reader in.@.dpx // writer out.@.exr --continue-on-error
        Disable recursivity with folders:  sam do reader inputFolder // writer outputFolder --no-recursivity
        
    ''' + self._tuttleVersion + '''            ''' + self.tuttleWebSiteUserDoc

    def fillParser(self, parser):
        # Arguments
        parser.add_argument('inputs', nargs='*', action='store', help='command line to process').completer = samDoUtils.samDoCompleter

        # Options
        parser.add_argument('-r', '--ranges', dest='ranges', nargs='+', type=int, help='specify the ranges to process (only numbers separate with spaces)')
        parser.add_argument('-n', '--nodes', dest='nodes', action='store_true', help='list all avalaible nodes')
        parser.add_argument('--file-formats', dest='fileFormats', action='store_true', help='list all supported file formats (R/W)')
        parser.add_argument('--no-recursivity', dest='noRecursivity', action='store_true', default=False, help='Disable recursivity when using directory as input/output')
        parser.add_argument('--continue-on-error', dest='continueOnError', action='store_true', default=False, help='continue the process even if errors occured')
        parser.add_argument('--stop-on-missing-files', dest='stopOnMissingFiles', action='store_true', default=False, help='stop the process if missing files')
        parser.add_argument('--no-plugin-cache', dest='noPluginCache', action='store_true', default=False, help='load plugins without using the cache file')
        parser.add_argument('--rebuild-plugin-cache', dest='rebuildPluginCache', action='store_true', default=False, help='load plugins and rebuild the cache file')
        parser.add_argument('-v', '--verbose', dest='verbose', action=samUtils.SamSetVerboseAction, default=2, help='verbose level (0/fatal, 1/error, 2/warn(by default), 3/info, 4/debug, 5(or upper)/trace) of tuttle host and sam application')
        # parser.add_argument('-h', '--help', dest='help', action='store_true', help='show this help message and exit')

    def _isCommandLineInvalid(self, inputsToProcess, inputsUnknown):
        """
        Returns if the given sam do inputs to process is not well written.
        @param inputsToProcess the 'inputs' arguments of the command line.
        @param inputsUnknown the 'inputs' arguments of the command line which are not recognized by argparse.
        """
        # check if there is no arguments
        if len(inputsToProcess) == 0:
            self.logger.warn('No input to process: the command is invalid.')
            return True
        # else check if last input is the separator
        elif inputsToProcess[-1] == '//':
            self.logger.warn('The given inputs to process are invalid: ' + str(inputsToProcess))
            return True
        return False

    def _isCommandLineAskForHelp(self, inputsToProcess, inputsUnknown):
        """
        Returns if the given sam do command expects to print its help.
        """
        # check if there is no arguments
        if len(inputsToProcess) == 0:
            if len(inputsUnknown) == 0 or '-h' in inputsUnknown or '--help' in inputsUnknown:
                return True
        # else check if help is asked before any input to process
        else:
            firstOption = sys.argv[1]
            # manage if the command is called from 'sam do' or 'sam-do'
            if firstOption == self.command and len(sys.argv) > 2:
                firstOption = sys.argv[2]
            if ('-h' == firstOption or '--help' == firstOption):
                return True
        return False

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
        blueTitle = colored.blue(title)
        if clintVersion >= '0.3.3':
            blueTitle.bold=True
        puts('\n' + blueTitle)

    def _displayPlugins(self):
        """
        Display all available plugins (in alphabetical order), with their versions (v[major].[minor]).
        """
        plugins = []
        for plugin in tuttle.core().getPlugins():
            plugins.append(plugin.getIdentifier().ljust(30) + ' (v' + str(plugin.getVersionMajor()) + '.' + str(plugin.getVersionMinor()) + ')')
        for plugin in sorted(set(plugins)):
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
            try:
                plugin.loadAndDescribeActions()
                if plugin.supportsContext('OfxImageEffectContextReader'):
                    pluginDescriptor = plugin.getDescriptorInContext('OfxImageEffectContextReader')
                    if pluginDescriptor.getParamSetProps().hasProperty('TuttleOfxImageEffectPropSupportedExtensions'):
                        supportedExtensions['r'].extend(getListOfSupportedExtension(pluginDescriptor))
                elif plugin.supportsContext('OfxImageEffectContextWriter'):
                    pluginDescriptor = plugin.getDescriptorInContext('OfxImageEffectContextWriter')
                    if pluginDescriptor.getParamSetProps().hasProperty('TuttleOfxImageEffectPropSupportedExtensions'):
                        supportedExtensions['w'].extend(getListOfSupportedExtension(pluginDescriptor))
            except Exception as e:
                self.logger.warning('Cannot load and describe plugin "' + plugin.getIdentifier() + '".')
                self.logger.debug(e)
        for key, extensions in supportedExtensions.items():
            if key == 'r':
                self._displayTitle('SUPPORTED INPUT FILE FORMATS')
            elif key == 'w':
                self._displayTitle('SUPPORTED OUTPUT FILE FORMATS')
            puts(', '.join(sorted(set(extensions))))

    def _displayParamHelp(self, param):
        """
        Display help of the given OFXParameter.
        """
        paramName = colored.green(param.getScriptName())
        if param.getEnabled() and not param.getSecret() and clintVersion >= '0.3.3':
            paramName.bold = True
        paramType = param.getParamTypeName()
        paramHint = param.getHint()
        paramDefaultValue = None
        paramChoiceValues = []
        paramChoiceLabel = []
        paramMinDisplayValue = []
        paramMaxDisplayValue = []
        paramHasMinMaxValues = False
        paramIsChoice = False

        props = param.getProperties()

        # Choice param
        if param.getParamType() == 'OfxParamTypeChoice':
            paramIsChoice = True
            # Get default choice value
            if props.hasProperty('OfxParamPropDefault'):
                propDefault = props.fetchProperty('OfxParamPropDefault')
                defaultValue = samUtils.getListValues(propDefault)
                if propDefault.getType() == tuttle.ePropTypeInt:
                    paramDefaultValue = props.getIntProperty('OfxParamPropDefault', 0)
            # Get choice values
            if props.hasProperty('OfxParamPropChoiceOption'):
                propChoiceOption = props.fetchProperty('OfxParamPropChoiceOption')
                paramChoiceValues = samUtils.getListValues(propChoiceOption)
            # Get label values
            if props.hasProperty('OfxParamPropChoiceLabelOption'):
                propChoiceLabel = props.fetchProperty('OfxParamPropChoiceLabelOption')
                paramChoiceLabel = samUtils.getListValues(propChoiceLabel)
                hasLabel = (len(paramChoiceValues) == len(paramChoiceLabel))

        # Other param types
        else:
            # Get default value
            if props.hasProperty('OfxParamPropDefault'):
                propDefault = props.fetchProperty('OfxParamPropDefault')
                paramDefaultValue = samUtils.getListValues(propDefault)
            # Get min/max values
            if props.hasProperty('OfxParamPropDisplayMin'):
                propDisplayMin = props.fetchProperty('OfxParamPropDisplayMin')
                propDisplayMax = props.fetchProperty('OfxParamPropDisplayMax')
                paramMinDisplayValue = samUtils.getListValues(propDisplayMin)
                paramMaxDisplayValue = samUtils.getListValues(propDisplayMax)
                # check +inf
                for i in range(0, len(paramMaxDisplayValue)):
                    if propDisplayMax.getType() == tuttle.ePropTypeInt:
                        if int(paramMaxDisplayValue[i]) >= samUtils.getMaxInt():
                            paramMaxDisplayValue[i] = 'inf'
                    elif propDisplayMax.getType() == tuttle.ePropTypeDouble:
                        if float(paramMaxDisplayValue[i]) >= samUtils.getMaxInt():
                            paramMaxDisplayValue[i] = 'inf'
                # check -inf
                for i in range(0, len(paramMinDisplayValue)):
                    if propDisplayMax.getType() == tuttle.ePropTypeInt:
                        if int(paramMinDisplayValue[i]) <= -samUtils.getMaxInt()-1:
                            paramMinDisplayValue[i] = '-inf'
                    elif propDisplayMax.getType() == tuttle.ePropTypeDouble:
                        if float(paramMinDisplayValue[i]) <= -samUtils.getMaxInt()-1:
                            paramMinDisplayValue[i] = '-inf'
                paramHasMinMaxValues = len(paramMinDisplayValue) > 0 and len(paramMinDisplayValue) == len(paramMaxDisplayValue)

        # Print
        with indent(4):
            puts('{paramName!s:50}: {paramType!s:10}'.format(
                paramName=paramName,
                paramType=paramType),
                newline=paramIsChoice)

            if paramIsChoice:
                with indent(40):
                    for choiceValue in paramChoiceValues:
                        puts('{choiceValue!s:50} {label}'.format(
                            choiceValue=(colored.yellow(choiceValue + ' (default)') if paramChoiceValues.index(choiceValue) == paramDefaultValue else colored.red(choiceValue)),
                            label=(paramChoiceLabel[paramChoiceValues.index(choiceValue)] if hasLabel else '')))
            else:
                puts('{defaultValue!s:9}'.format(
                    defaultValue=colored.yellow(','.join(paramDefaultValue))),
                    newline=(not paramHasMinMaxValues))

                if paramHasMinMaxValues:
                    puts('[{minDisplayValue:5} --> {maxDisplayValue:5}]'.format(
                        minDisplayValue=','.join(paramMinDisplayValue),
                        maxDisplayValue=','.join(paramMaxDisplayValue)))

            with indent(2):
                puts(paramHint)

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
            puts('{clipName!s:10}: {clipComponents} {clipProperties}'.format(
                clipName=colored.green(clip.getName()),
                clipComponents=('[' + (', '.join(componentsStr)) + ']'),
                clipProperties=(', '.join(properties))))

    def _displayNodeHelp(self, nodeFullName, node):
        """
        Display help of a specific node in the command line.
        """
        if not node:
            self.logger.error('Cannot print help of unknown plugin "' + nodeFullName + '".')
            exit(1)
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
            for bitDepthValue in bitDepthValues[:int(len(bitDepthValues)/2)]:
                bitDepthSourceStr.append(self._getNbBitsFromOfxBitDepth(bitDepthValue))
            for bitDepthValue in bitDepthValues[int(len(bitDepthValues)/2):]:
                bitDepthOutputStr.append(self._getNbBitsFromOfxBitDepth(bitDepthValue))
            # Print
            with indent(4):
                puts('{name!s:10}: [{bitdepth}]'.format(
                    name=colored.green('Source'),
                    bitdepth=', '.join(bitDepthSourceStr)))
                puts('{name!s:10}: [{bitdepth}]'.format(
                    name=colored.green('Output'),
                    bitdepth=', '.join(bitDepthOutputStr)))

    def _getTuttleGraph(self, splitCmdGraph):
        """
        Return the tuple (tuttle graph, its nodes) which corresponds to the given split command.
        """
        graph = tuttle.Graph()
        nodes = []
        connections = []
        error = 0

        for splitCmdNode in splitCmdGraph.getNodes():
            # Create a tuttle node
            nodeFullName = ''
            node = None
            try:
                nodeFullName = splitCmdNode.getPluginName(self.logger)
                node = graph.createNode(nodeFullName)
            except Exception as e:
                # Plugin not found
                if not nodeFullName:
                    nodeFullName = splitCmdNode._pluginId
                self.logger.error('Cannot create node "' + nodeFullName + '".')
                self.logger.debug(e)
                error = 1
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
                            self.logger.error('Cannot connect attribute "'
                                + argName + '" of node "' + nodeFullName
                                + '" to id "' + argValue)
                            self.logger.debug(e)
                            error = 1
                    else:
                        # Cannot set param of node
                        self.logger.error('Cannot set '
                            + (('parameter "' + argName + '" of ') if argName else '')
                            + 'node "' + nodeFullName + '" '
                            + (('to value "' + argValue + '"') if argValue else ''))
                        self.logger.debug(e)
                        error = 1
            nodes.append(node)
            connections.append(splitCmdNode.getArgument('id')[1])

        # exit the process if there is an error in the command line
        if error:
            exit(error)

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

        # Set sam log level
        self.setLogLevel(args.verbose)
        # set tuttle host log level
        tuttle.core().getFormatter().setLogLevel(args.verbose)

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
        if self._isCommandLineAskForHelp(args.inputs, unknown):
            self._displayCommandLineHelp(parser)
            exit(0)

        # Check sam-do command line
        if self._isCommandLineInvalid(args.inputs, unknown):
            self._displayCommandLineHelp(parser)
            exit(1)

        # Add unknown options to the command line to process
        args.inputs.extend(unknown)

        # Split command line
        splitCmd = samDoUtils.SplitCmd(args.inputs, args.noRecursivity)
        graphsWithNodes = []
        for splitCmdGraph in splitCmd.getGraphs():
            self.logger.debug('Create the following tuttle graph: \n' + str(splitCmdGraph))
            try:
                graphsWithNodes.append(self._getTuttleGraph(splitCmdGraph))
            except Exception as e:
                self.logger.error('Cannot create tuttle graph')
                self.logger.debug('\n' + str(splitCmdGraph))
                self.logger.debug(e)

        if not graphsWithNodes:
            exit(1)

        error = 0
        # Compute the corresponding tuttle graphs
        for graph, nodes in graphsWithNodes:
            # Options of process
            options = tuttle.ComputeOptions()
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

            if not nodes:
                self.logger.warning('No tuttle nodes to compute')
                continue

            # Connect and compute
            try:
                graph.compute(nodes[-1], options)
            except Exception as e:
                self.logger.error('Tuttle graph computation has failed.')
                self.logger.debug(e)
                error = 1
            self.logger.info('Memory usage: ' + str(int(samUtils.memoryUsageResource())) + 'KB')

        exit(error)

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
    samUtils.doCompletion(parser)

    # Run the command
    tool.run(parser)
