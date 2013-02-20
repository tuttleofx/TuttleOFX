#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESETPARSER_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESETPARSER_HPP_

#include "LibAVPresetDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace ffmpeg {

class LibAVPresetParser
{
public:
	LibAVPresetParser();
	~LibAVPresetParser();
	
	static PresetsList researchPresetsFiles();

	static PresetsList getPresetListForPathList( const std::string& envVar );
	
	static bool isValidExtension( const std::string& filename );
	
	static void getPresetList( std::vector<std::string>& presetID, const std::string& extension = "" );
	static void getPresetList( std::vector<std::string>& presetID, std::vector<std::string>& presetLabelID, const std::string& extension = "" );
	static PresetsList getPresetsPathList( const std::string& extension = "" );
};

}
}
}

#endif
