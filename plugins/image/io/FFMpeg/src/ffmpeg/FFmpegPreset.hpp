#ifndef _TUTTLE_PLUGIN_FFMPEG_PRESET_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_PRESET_HPP_

#include "FFmpeg.hpp"
#include <tuttle/plugin/global.hpp>

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

class FFmpegPreset
{
public:
	typedef std::vector<std::string> Presets;
	typedef std::map<std::string, std::string> PresetsOptions;

public:
	FFmpegPreset();

	Presets getCodecListWithConfig() const;
	
	std::string getFilename  ( const std::string& codec, const std::string& preset ) const;
	std::string getCodecName ( const std::string& path ) const;
	std::string getConfigName( const std::string& path ) const;
	
	Presets getConfigList( const std::string& codec ) const;
	
	PresetsOptions getOptionsForPresetFilename( const std::string& presetFile );
	
private:
	void researchPresets();
	
	static Presets getPresetConfigurations( const std::string& envVar );
	
	/**
	  * map of codec - absolute filename
	  */
	Presets _presets;
};

}
}
}

#endif

