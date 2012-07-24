#ifndef __FFMPEG_PRESET_HPP__
#define __FFMPEG_PRESET_HPP__

#include "FFmpeg.hpp"
#include <tuttle/plugin/global.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;

class FFmpegPreset : public FFmpeg
{
	typedef std::vector<std::string> Presets;

public:
	FFmpegPreset();

	std::string getCodecName ( const std::string& path );
	std::string getConfigName( const std::string& path );
	
	std::vector<std::string> getConfigList( const std::string& codec );
	
private:
	void researchPresets();
	
	std::vector<std::string> getPresetConfigurations( std::string environementVariable );
	
	/**
	  * map of codec - absolute filename
	  */
	Presets presets;
};

#endif

