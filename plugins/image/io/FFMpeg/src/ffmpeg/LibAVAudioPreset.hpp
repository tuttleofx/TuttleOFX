#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVAUDIOPRESET_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVAUDIOPRESET_HPP_

#include "LibAVPresetParser.hpp"
#include "LibAVPresetFileParser.hpp"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

namespace bfs = boost::filesystem;

class LibAVAudioPreset
{
public:
	LibAVAudioPreset( const std::string& id )
		: presetID( id )
	{
		BOOST_FOREACH( const std::string& presetPath, LibAVPresetParser::researchPresetsFiles() )
		{
			bfs::path f = presetPath;
			if( f.extension() == kPresetAudioExtension )
			{
				LibAVPresetFileParser fp( presetPath );
				if( presetID == fp.getId() )
				{
					params = fp.getParameters();
				}
			}
		}
	}
	
	~LibAVAudioPreset()
	{
		
	}
	
	static void getPresetList( std::vector<std::string>& presetID )
	{
		LibAVPresetParser::getPresetList( presetID, kPresetAudioExtension );
	}
	
	static void getPresetList( std::vector<std::string>& presetID, std::vector<std::string>& presetLabelID )
	{
		LibAVPresetParser::getPresetList( presetID, presetLabelID, kPresetAudioExtension );
	}
	
	std::string getPresetID() { return presetID; }
	std::string getAudioCodec()   { return params.find( "acodec" )->second.at(0); }
	
	PresetParameters& getParameters(){ return params; }
	
	PresetsList getPresetsPathList()
	{
		return LibAVPresetParser::getPresetsPathList( kPresetAudioExtension );
	}
	
private:
	std::string      presetID;
	PresetParameters params;
	
};

}
}
}
#endif
