#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESETFILEPARSER_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESETFILEPARSER_HPP_

#include "LibAVPresetDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace ffmpeg {

class LibAVPresetFileParser
{
	
public:
	LibAVPresetFileParser( const std::string& filepath );
	~LibAVPresetFileParser();
	
	static std::string getId( const std::string& filepath );
	std::string getId() { return id; }
	std::string getIdLabel() { return idLabel; }
	
	PresetParameters& getParameters() { return presetsParameters; }

	bool idValidFile() const { return presetType != eUnknownPresetType ; }
	
	void print() const;
	
protected:
	PresetParameters parseFile( const std::string& filepath );
	
private:
	std::string id;
	std::string idLabel;
	EPresetType presetType;
	PresetParameters presetsParameters;
};

}
}
}

#endif
