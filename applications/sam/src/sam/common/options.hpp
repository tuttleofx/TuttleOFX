#ifndef _SAM_OPTIONS_HPP_
#define _SAM_OPTIONS_HPP_

namespace sam {

#define SAM_HELP_MARGING 35
#define SAM_EXAMPLE_TITLE_COUT( ... )   TUTTLE_COUT( std::endl << color->_yellow << "  " << __VA_ARGS__ << color->_std );
#define SAM_EXAMPLE_LINE_COUT( x, ... ) TUTTLE_COUT( color->_green << "    " << std::setw(SAM_HELP_MARGING) << x << __VA_ARGS__ << color->_std );

static const char* const kUrlTuttleofxProject = "http://www.tuttleofx.org";

/// options with short-cut

//-a, --all
static const char* const kAllOptionLongName = "all";
static const char* const kAllOptionString = "all,a";
static const char* const kAllOptionMessage = "do not ignore entries starting with .";

//-A, --attributes
static const char* const kAttributesOptionLongName = "attributes";
static const char* const kAttributesOptionString = "attributes,A";
static const char* const kAttributesOptionMessage = "show all attributes: parameters+clips";

//-c, --commands
static const char* const kCommandsOptionLongName = "commands";
static const char* const kCommandsOptionString = "commands,c";
static const char* const kCommandsOptionMessage = "show list of all available sam commands";

//-d, --directories
static const char* const kDirectoriesOptionLongName = "directories";
static const char* const kDirectoriesOptionString = "directories,d";
static const char* const kDirectoriesOptionMessage = "handle directories";

//-e, --expression
static const char* const kExpressionOptionLongName = "expression";
static const char* const kExpressionOptionString = "expression,e";
static const char* const kExpressionOptionMessage = "use a specific pattern, ex: *.jpg,*.png";

//-f, --files
static const char* const kFilesOptionLongName = "files";
static const char* const kFilesOptionString = "files,f";
static const char* const kFilesOptionMessage = "handle files";

//-F, --filter
static const char* const kFilterOptionLongName = "filter";
static const char* const kFilterOptionString = "filter,F";
static const char* const kFilterOptionMessage = "filtering the output for research a plugin. ex: -f \"*blur,*tuttle*\"";

//-h, --help
static const char* const kHelpOptionLongName = "help";
static const char* const kHelpOptionString = "help,h";
static const char* const kHelpOptionMessage = "display help";

//-i, --input
static const char* const kInputOptionLongName = "input";
static const char* const kInputOptionString = "input,i";
static const char* const kInputOptionMessage = "input pathname: directory, file or sequence pattern";
//
//-I, --ignore =was -m, --mask
static const char* const kIgnoreOptionLongName = "ignore";
static const char* const kIgnoreOptionString = "ignore,I";
static const char* const kIgnoreOptionMessage = "ignore the specified sequence";

//-l, --long-listing
static const char* const kLongListingOptionLongName = "long-listing";
static const char* const kLongListingOptionString = "long-listing,l";
static const char* const kLongListingOptionMessage = "use a long listing format";

//-n, --nodes
static const char* const kNodesOptionLongName = "nodes";
static const char* const kNodesOptionString = "nodes,n";
static const char* const kNodesOptionMessage = "list all avalaible node";

//-o, --offset
static const char* const kOffsetOptionLongName = "offset";
static const char* const kOffsetOptionString = "offset,o";
static const char* const kOffsetOptionMessage = "retime the sequence with the given offset. ex: -o 1, -o";
//
//-p, --path =absolute path=was --path-root and --absolute-path
static const char* const kPathOptionLongName = "path";
static const char* const kPathOptionString = "path,p";
static const char* const kPathOptionMessage = "display the absolute path of each object";

//-Q, --quiet
static const char* const kQuietOptionLongName = "quiet";
static const char* const kQuietOptionString = "quiet,Q";
static const char* const kQuietOptionMessage = "don't print commands";

//-r, --range
static const char* const kRangeOptionLongName = "range";
static const char* const kRangeOptionString = "range,r";
static const char* const kRangeOptionMessage = "processing range (used only if input is a sequence pattern)";

//-R, --recursive
static const char* const kRecursiveOptionLongName = "recursive";
static const char* const kRecursiveOptionString = "recursive,R";
static const char* const kRecursiveOptionMessage = "handle directories and their content recursively";

//-s, --sequences
static const char* const kSequencesOptionLongName = "sequences";
static const char* const kSequencesOptionString = "sequences,s";
static const char* const kSequencesOptionMessage = "handle sequences";

//-v, --version
static const char* const kVersionOptionLongName = "version";
static const char* const kVersionOptionString =  "version,v";
static const char* const kVersionOptionMessage = "display version";

//-V, --verbose
static const char* const kVerboseOptionLongName = "verbose";
static const char* const kVerboseOptionString =  "verbose,V";
static const char* const kVerboseOptionMessage = "verbose level (trace=0, debug=1, info=2, warning=3, error=4, fatal=5)";
static const char* const kVerboseOptionDefaultValue = "warning";

/// options without short-cut

//--binaries-list
static const char* const kBinariesListOptionLongName = "binaries-list";
static const char* const kBinariesListOptionString = kBinariesListOptionLongName;
static const char* const kBinariesListOptionMessage = "show list of all available sam binaries";

//--brief
static const char* const kBriefOptionLongName = "brief";
static const char* const kBriefOptionString = kBriefOptionLongName;
static const char* const kBriefOptionMessage = "display a brief summary of the tool";

//--clip
static const char* const kClipOptionLongName = "clip";
static const char* const kClipOptionString = kClipOptionLongName;
static const char* const kClipOptionMessage = "display clip informations";

//--clips
static const char* const kClipsOptionLongName = "clips";
static const char* const kClipsOptionString = kClipsOptionLongName;
static const char* const kClipsOptionMessage = "list clips of the OpenFX plugin";

//--color
static const char* const kColorOptionLongName = "color";
static const char* const kColorOptionString = kColorOptionLongName;
static const char* const kColorOptionMessage = "display the output with colors";

//--commands-list
static const char* const kCommandsListOptionLongName = "commands-list";
static const char* const kCommandsListOptionString = kCommandsListOptionLongName;
static const char* const kCommandsListOptionMessage = "show list of all available sam commands";

//--continue-on-error
static const char* const kContinueOnErrorOptionLongName = "continue-on-error";
static const char* const kContinueOnErrorOptionString = kContinueOnErrorOptionLongName;
static const char* const kContinueOnErrorOptionMessage = "continue on error";

//--disable-process
static const char* const kDisableProcessOptionLongName = "disable-process";
static const char* const kDisableProcessOptionString = kDisableProcessOptionLongName;
static const char* const kDisableProcessOptionMessage = "run the command line without computing images (verbose enable).";

//--force-identity-nodes-process
static const char* const kForceIdentityNodesProcessOptionLongName = "force-identity-nodes-process";
static const char* const kForceIdentityNodesProcessOptionString = kForceIdentityNodesProcessOptionLongName;
static const char* const kForceIdentityNodesProcessOptionMessage = "force identity nodes process (debug option for plugin developers only)";

//--enable-color
static const char* const kEnableColorOptionLongName = "enable-color";
static const char* const kEnableColorOptionString = kEnableColorOptionLongName;
static const char* const kEnableColorOptionMessage = "enable or disable color";

//--expert
static const char* const kExpertOptionLongName = "expert";
static const char* const kExpertOptionString = kExpertOptionLongName;
static const char* const kExpertOptionMessage = "show expert user options";

//--first-image //TODO in sam-info and sam-ls, replace first-image and last-image by intput/output-first/last
static const char* const kFirstImageOptionLongName = "first-image";
static const char* const kFirstImageOptionString = kFirstImageOptionLongName;
static const char* const kFirstImageOptionMessage = "specify the first image";

//--format
static const char* const kFormatOptionLongName = "format";
static const char* const kFormatOptionString = kFormatOptionLongName;
static const char* const kFormatOptionMessage = "display supported formats";

//--full-display
static const char* const kFullDisplayOptionLongName = "full-display";
static const char* const kFullDisplayOptionString = kFullDisplayOptionLongName;
static const char* const kFullDisplayOptionMessage = "display everything =directories, files, sequences";

//--full-rm
static const char* const kFullRMPathOptionLongName = "full-rm";
static const char* const kFullRMPathOptionString = kFullRMPathOptionLongName;
static const char* const kFullRMPathOptionMessage = "remove everything =directories, files, sequences";

//--generator-args
static const char* const kGeneratorArgsOptionLongName = "generator-args";
static const char* const kGeneratorArgsOptionString = kGeneratorArgsOptionLongName;
static const char* const kGeneratorArgsOptionMessage = "properties of generator plugin";

//--id
static const char* const kIdOptionLongName = "id";
static const char* const kIdOptionString = kIdOptionLongName;
static const char* const kIdOptionMessage = "set a name/id to the node";

//--input-dir
static const char* const kInputDirOptionLongName = "input-dir";
static const char* const kInputDirOptionString = kInputDirOptionLongName;
static const char* const kInputDirOptionMessage = "input directories";

//--input-first
static const char* const kInputFirstOptionLongName = "input-first";
static const char* const kInputFirstOptionString = kInputFirstOptionLongName;
static const char* const kInputFirstOptionMessage = "specify the first input image in order to select a sub-range of the input sequence";

//--input-last
static const char* const kInputLastOptionLongName = "input-last";
static const char* const kInputLastOptionString = kInputLastOptionLongName;
static const char* const kInputLastOptionMessage = "specify the last input image in order to select a sub-range of the input sequence";

//--last-image //TODO in sam-info and sam-ls, replace first-image and last-image by intput/output-first/last
static const char* const kLastImageOptionLongName = "last-image";
static const char* const kLastImageOptionString = kLastImageOptionLongName;
static const char* const kLastImageOptionMessage = "specify the last image";

//--nodes-list
static const char* const kNodesListOptionLongName = "nodes-script";
static const char* const kNodesListOptionString = kNodesListOptionLongName;
static const char* const kNodesListOptionMessage = "show list of all available nodes (formatted for scripting)";

//--output-first
static const char* const kOutputFirstOptionLongName = "output-first";
static const char* const kOutputFirstOptionString = kOutputFirstOptionLongName;
static const char* const kOutputFirstOptionMessage = "specify the first output image, in order to select a sub-range of the output sequence";

//--output-last
static const char* const kOutputLastOptionLongName = "output-last";
static const char* const kOutputLastOptionString = kOutputLastOptionLongName;
static const char* const kOutputLastOptionMessage = "specify the last output image, in order to select a sub-range of the output sequence";

//--parameters-list
static const char* const kParametersOptionLongName = "parameters-list";
static const char* const kParametersOptionString = kParametersOptionLongName;
static const char* const kParametersOptionMessage = "list parameters of the OpenFX plugin";

//--parameters-redux-list
static const char* const kParametersReduxOptionLongName = "parameters-redux-list";
static const char* const kParametersReduxOptionString = kParametersReduxOptionLongName;
static const char* const kParametersReduxOptionMessage = "list parameters of the OpenFX plugin (redux informations)";

//--param-default
static const char* const kParamDefaultOptionLongName = "param-default";
static const char* const kParamDefaultOptionString = kParamDefaultOptionLongName;
static const char* const kParamDefaultOptionMessage = "parameter default value";

//--param-group
static const char* const kParamGroupOptionLongName = "param-group";
static const char* const kParamGroupOptionString = kParamGroupOptionLongName;
static const char* const kParamGroupOptionMessage = "group of the OpenFX plugin";

//--param-infos
static const char* const kParamInfosOptionLongName = "param-infos";
static const char* const kParamInfosOptionString = kParamInfosOptionLongName;
static const char* const kParamInfosOptionMessage = "display parameter informations";

//--param-possible-values
static const char* const kParamPossibleValuesOptionLongName = "param-possible-values";
static const char* const kParamPossibleValuesOptionString = kParamPossibleValuesOptionLongName;
static const char* const kParamPossibleValuesOptionMessage = "possible parameter values";

//--param-type
static const char* const kParamTypeOptionLongName = "param-type";
static const char* const kParamTypeOptionString = kParamTypeOptionLongName;
static const char* const kParamTypeOptionMessage = "parameter type";

//--param-values
static const char* const kParamValuesOptionLongName = "param-values";
static const char* const kParamValuesOptionString = kParamValuesOptionLongName;
static const char* const kParamValuesOptionMessage = "display parameter values";

//--properties
static const char* const kPropertiesOptionLongName = "properties";
static const char* const kPropertiesOptionString = kPropertiesOptionLongName;
static const char* const kPropertiesOptionMessage = "list properties of the OpenFX plugin";

//--plugins
static const char* const kPluginsOptionLongName = "plugins";
static const char* const kPluginsOptionString = kPropertiesOptionLongName;
static const char* const kPluginsOptionMessage = "list of associated plugins for this dummy node";

//--reader
static const char* const kReaderOptionLongName = "reader";
static const char* const kReaderOptionString = kReaderOptionLongName;
static const char* const kReaderOptionMessage = "specify reader node with identifier \"tuttle.XXXreader\"";

//--relative-path
static const char* const kRelativePathOptionLongName = "relative-path";
static const char* const kRelativePathOptionString = kRelativePathOptionLongName;
static const char* const kRelativePathOptionMessage = "display the relative path of each object";

//--stop-on-missing-file
static const char* const kStopOnMissingFileOptionLongName = "stop-on-missing-file";
static const char* const kStopOnMissingFileOptionString = kStopOnMissingFileOptionLongName;
static const char* const kStopOnMissingFileOptionMessage = "stop on missing file";

//--nb-cores
static const char* const kNbCoresOptionLongName = "nb-cores";
static const char* const kNbCoresOptionString = kNbCoresOptionLongName;
static const char* const kNbCoresOptionMessage = "set a fix number of CPUs";

//--renderscale
static const char* const kRenderScaleOptionLongName = "renderscale";
static const char* const kRenderScaleOptionString = kRenderScaleOptionLongName;
static const char* const kRenderScaleOptionMessage = "specify render scale";

//--script
static const char* const kScriptOptionLongName = "script";
static const char* const kScriptOptionString = kScriptOptionLongName;
static const char* const kScriptOptionMessage = "format the output such as it could be dump in a file and be used as a script";

}

#endif
