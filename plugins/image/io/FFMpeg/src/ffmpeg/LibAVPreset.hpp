#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESET_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVPRESET_HPP_

#include "LibAVPresetParser.hpp"
#include "LibAVPresetFileParser.hpp"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

namespace bfs = boost::filesystem;

class LibAVPreset
{
public:
	LibAVPreset( const std::string& id )
		: presetID( id )
	{
		BOOST_FOREACH( const std::string& presetPath, LibAVPresetParser::researchPresetsFiles() )
		{
			bfs::path f = presetPath;
			if( f.extension() == kPresetExtension )
			{
				LibAVPresetFileParser fp( presetPath );
				if( presetID == fp.getId() )
				{
					params = fp.getParameters();
				}
			}
		}
		//TUTTLE_TCOUT_VAR( presetID );
	}
	
	~LibAVPreset()
	{
		
	}
	
	static void getPresetList( std::vector<std::string>& presetID )
	{
		LibAVPresetParser::getPresetList( presetID, kPresetExtension );
	}
	
	static void getPresetList( std::vector<std::string>& presetID, std::vector<std::string>& presetLabelID )
	{
		LibAVPresetParser::getPresetList( presetID, presetLabelID, kPresetExtension );
	}
	
	std::string getPresetID() { return presetID; }
	std::string getFormatID() { return params.find( "format" )->second.at(0); }
	std::string getVideoID()  { return params.find( "video" )->second.at(0); }
	std::string getAudioID()  { return params.find( "audio" )->second.at(0); }
	
	PresetsList getPresetsPathList()
	{
		return LibAVPresetParser::getPresetsPathList( kPresetExtension );
	}
	
private:
	std::string      presetID;
	PresetParameters params;
	
};

}
}
}

#endif
