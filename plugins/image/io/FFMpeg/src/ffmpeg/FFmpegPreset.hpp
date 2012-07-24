#ifndef __FFMPEG_PRESET_HPP__
#define __FFMPEG_PRESET_HPP__

#include "FFmpeg.hpp"
#include <tuttle/plugin/global.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace bfs = boost::filesystem;

class FFmpegPreset : public FFmpeg
{
public:
	typedef std::vector<std::string> Presets;
	typedef std::map<std::string, std::string> PresetsOptions;

public:
	FFmpegPreset();

	Presets getCodecListWithConfig( );
	
	std::string getCodecName ( const std::string& path );
	std::string getConfigName( const std::string& path );
	
	Presets getConfigList( const std::string& codec );
	
	PresetsOptions getOptionsForPresetFilename( const std::string& presetFile );
	
private:
	void researchPresets();
	
	Presets getPresetConfigurations( std::string environementVariable );
	
	/**
	  * map of codec - absolute filename
	  */
	Presets presets;
};

#endif

