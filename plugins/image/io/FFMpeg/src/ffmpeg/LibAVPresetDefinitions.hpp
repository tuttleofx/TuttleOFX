#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESETDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESETDEFINITIONS_HPP_

#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::vector<std::string> > PresetParameters;
typedef std::vector<std::string> PresetsList;

enum EPresetType{
	eMainPresetType = 0,
	eFormatPresetType,
	eVideoPresetType,
	eAudioPresetType,
	eUnknownPresetType
};

static const std::string kPresetExtension = ".avpreset";
static const std::string kPresetFormatExtension = ".avfpreset";
static const std::string kPresetVideoExtension = ".avvpreset";
static const std::string kPresetAudioExtension = ".avapreset";
static const std::string kPresetLabel = "label";

static const std::string kPresetSubDirPath = ".libav";

#ifndef __WINDOWS__
#define DIRLIST_SEP_CHARS ":;"
#else
#define DIRLIST_SEP_CHARS ";"
#endif

#endif
