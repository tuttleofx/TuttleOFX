/*
 * options.hpp
 *
 *  Created on: Apr 16, 2012
 *      Author: mfe
 */

#ifndef OPTIONS_HPP_
#define OPTIONS_HPP_

namespace sam {

//-a, --all
static const std::string kAllOptionLongName("all");
static const std::string kAllOptionString(kAllOptionLongName + ",a");
static const std::string kAllOptionMessage(
		"do not ignore entries starting with .");

//-c, --commands
static const std::string kCommandsOptionLongName("commands");
static const std::string kCommandsOptionString(kCommandsOptionLongName + ",c");
static const std::string kCommandsOptionMessage(
		"show list of all available sam commands");

//-d, --directories
static const std::string kDirectoriesOptionLongName("directories");
static const std::string kDirectoriesOptionString(
		kDirectoriesOptionLongName + ",d");
static const std::string kDirectoriesOptionMessage("handle directories");
//
//-e, --expression
static const std::string kExpressionOptionLongName("expression");
static const std::string kExpressionOptionString(
		kExpressionOptionLongName + ",e");
static const std::string kExpressionOptionMessage(
		"use a specific pattern, ex: *.jpg,*.png");

//-f, --files
static const std::string kFilesOptionLongName("files");
static const std::string kFilesOptionString(kFilesOptionLongName + ",f");
static const std::string kFilesOptionMessage("handle files");

//-F, --filter
static const std::string kFilterOptionLongName("filter");
static const std::string kFilterOptionString(kFilterOptionLongName + ",F");
static const std::string kFilterOptionMessage("filtering the output for research a plugin. ex: -f \"*blur,*tuttle*\"");

//-h, --help
static const std::string kHelpOptionLongName("help");
static const std::string kHelpOptionString(kHelpOptionLongName + ",h");
static const std::string kHelpOptionMessage("display help");

//-i, --input
static const std::string kInputOptionLongName("input");
static const std::string kInputOptionString(kInputOptionLongName + ",i");
static const std::string kInputOptionMessage(
		"input pathname (directory, file or sequence pattern)");
//
//-I, --ignore (was -m, --mask)
static const std::string kIgnoreOptionLongName("ignore");
static const std::string kIgnoreOptionString(kIgnoreOptionLongName + ",I");
static const std::string kIgnoreOptionMessage("ignore the specified sequence");

//-l, --long-listing
static const std::string kLongListingOptionLongName("long-listing");
static const std::string kLongListingOptionString(
		kLongListingOptionLongName + ",l");
static const std::string kLongListingOptionMessage("use a long listing format");

//-n, --nodes
static const std::string kNodesOptionLongName("nodes");
static const std::string kNodesOptionString(kNodesOptionLongName + ",l");
static const std::string kNodesOptionMessage("list all avalaible node");

//-o, --offset
static const std::string kOffsetOptionLongName("offset");
static const std::string kOffsetOptionString(kOffsetOptionLongName + ",o");
static const std::string kOffsetOptionMessage(
		"retime the sequence with the given offset. ex: -o 1, -o");
//
//-p, --path (absolute path)(was --path-root and --absolute-path)
static const std::string kPathOptionLongName("path");
static const std::string kPathOptionString(kPathOptionLongName + ",p");
static const std::string kPathOptionMessage(
		"display the absolute path of each object");

//-Q, --quiet
static const std::string kQuietOptionLongName("quiet");
static const std::string kQuietOptionString(kQuietOptionLongName + ",Q");
static const std::string kQuietOptionMessage("don't print commands");

//-r, --range
static const std::string kRangeOptionLongName("range");
static const std::string kRangeOptionString(kRangeOptionLongName + ",r");
static const std::string kRangeOptionMessage(
		"range (used only if input is a sequence pattern)");

//-R, --recursive
static const std::string kRecursiveOptionLongName("recursive");
static const std::string kRecursiveOptionString(
		kRecursiveOptionLongName + ",R");
static const std::string kRecursiveOptionMessage(
		"handle directories and their content recursively");
//
//-v, --version
static const std::string kVersionOptionLongName("version");
static const std::string kVersionOptionString(kVersionOptionLongName + ",v");
static const std::string kVersionOptionMessage("display version");

//-V, --verbose
static const std::string kVerboseOptionLongName("verbose");
static const std::string kVerboseOptionString(kVerboseOptionLongName + ",V");
static const std::string kVerboseOptionMessage("enable verbose mode");

////////////
//--brief
static const std::string kBriefOptionLongName("brief");
static const std::string &kBriefOptionString = kBriefOptionLongName;
static const std::string kBriefOptionMessage(
		"display a brief summary of the tool");

//--clips
static const std::string kClipsOptionLongName("clips");
static const std::string &kClipsOptionString = kClipsOptionLongName;
static const std::string kClipsOptionMessage(
		"list clips of the OpenFX plugin");

//--color
static const std::string kColorOptionLongName("color");
static const std::string &kColorOptionString = kColorOptionLongName;
static const std::string kColorOptionMessage("display the output with colors");

//--continue-on-error
static const std::string kContinueOnErrorOptionLongName("continue-on-error");
static const std::string &kContinueOnErrorOptionString = kContinueOnErrorOptionLongName;
static const std::string kContinueOnErrorOptionMessage("continue on error");

//--first-image //TODO in sam-info and sam-ls, replace first-image and last-image by intput/output-first/last
static const std::string kFirstImageOptionLongName("first-image");
static const std::string &kFirstImageOptionString = kFirstImageOptionLongName;
static const std::string kFirstImageOptionMessage("specify the first image");

//--full-display
static const std::string kFullDisplayOptionLongName("full-display");
static const std::string &kFullDisplayOptionString = kFullDisplayOptionLongName;
static const std::string kFullDisplayOptionMessage("display everything (directories, files, sequences)");

//--full-rm
static const std::string kFullRMPathOptionLongName("full-rm");
static const std::string &kFullRMPathOptionString = kFullRMPathOptionLongName;
static const std::string kFullRMPathOptionMessage(
		"remove everything (directories, files, sequences)");

//--id
static const std::string kIdOptionLongName("id");
static const std::string &kIdOptionString = kIdOptionLongName;
static const std::string kIdOptionMessage(
		"set a name/id to the node");


//--input-first
static const std::string kInputFirstOptionLongName("input-first");
static const std::string &kInputFirstOptionString = kInputFirstOptionLongName;
static const std::string kInputFirstOptionMessage(
		"specify the first input image in order to select a sub-range of the input sequence");

//--input-last
static const std::string kInputLastOptionLongName("input-last");
static const std::string &kInputLastOptionString = kInputLastOptionLongName;
static const std::string kInputLastOptionMessage(
		"specify the last input image in order to select a sub-range of the input sequence");

//--last-image //TODO in sam-info and sam-ls, replace first-image and last-image by intput/output-first/last
static const std::string kLastImageOptionLongName("last-image");
static const std::string &kLastImageOptionString  = kLastImageOptionLongName;
static const std::string kLastImageOptionMessage("specify the last image");

//--output-first
static const std::string kOutputFirstOptionLongName("output-first");
static const std::string &kOutputFirstOptionString = kOutputFirstOptionLongName;
static const std::string kOutputFirstOptionMessage(
		"specify the first output image, in order to select a sub-range of the output sequence");

//--output-last
static const std::string kOutputLastOptionLongName("output-last");
static const std::string &kOutputLastOptionString =  kOutputLastOptionLongName;
static const std::string kOutputLastOptionMessage(
		"specify the last output image, in order to select a sub-range of the output sequence");

//-parameters
static const std::string kParametersOptionLongName("parameters");
static const std::string &kParametersOptionString =  kParametersOptionLongName;
static const std::string kParametersOptionMessage(
		"list parameters of the OpenFX plugin");

//--properties
static const std::string kPropertiesOptionLongName("properties");
static const std::string &kPropertiesOptionString =  kPropertiesOptionLongName;
static const std::string kPropertiesOptionMessage(
		"list properties of the OpenFX plugin");

//--reader
static const std::string kReaderOptionLongName("reader");
static const std::string &kReaderOptionString = kReaderOptionLongName;
static const std::string kReaderOptionMessage(
		"specify reader node with identifier \"tuttle.XXXreader\"");

//--relative-path
static const std::string kRelativePathOptionLongName("relative-path");
static const std::string &kRelativePathOptionString =
		kRelativePathOptionLongName;
static const std::string kRelativePathOptionMessage(
		"display the relative path of each object");

//--nb-cores
static const std::string kNbCoresOptionLongName("nb-cores");
static const std::string &kNbCoresOptionString = kNbCoresOptionLongName;
static const std::string kNbCoresOptionMessage("set a fix number of CPUs");

//--renderscale
static const std::string kRenderScaleOptionLongName("renderscale");
static const std::string &kRenderScaleOptionString = kRenderScaleOptionLongName;
static const std::string kRenderScaleOptionMessage("specify render scale");

//--script
static const std::string kScriptOptionLongName("script");
static const std::string &kScriptOptionString = kScriptOptionLongName;
static const std::string kScriptOptionMessage(
		"format the output such as it could be dump in a file and be used as a script");

///// hidden option
//--binaries-list
static const std::string kBinariesListOptionLongName("binaries-list");
static const std::string &kBinariesListOptionString = kBinariesListOptionLongName;
static const std::string kBinariesListOptionMessage("show list of all available sam binaries");

//--commands-list
static const std::string kCommandsListOptionLongName("commands-list");
static const std::string &kCommandsListOptionString = kCommandsListOptionLongName;
static const std::string kCommandsListOptionMessage("show list of all available sam commands");

//--enable-color
static const std::string kEnableColorOptionLongName("enable-color");
static const std::string &kEnableColorOptionString = kEnableColorOptionLongName;
static const std::string kEnableColorOptionMessage("enable (or disable) color");

//--input-dir
static const std::string kInputDirOptionLongName("input-dir");
static const std::string &kInputDirOptionString = kInputDirOptionLongName;
static const std::string kInputDirOptionMessage("input directories");




}

#endif /* OPTIONS_HPP_ */
