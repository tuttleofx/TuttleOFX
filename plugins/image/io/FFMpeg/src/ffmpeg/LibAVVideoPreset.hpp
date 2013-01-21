#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVVIDEOPRESET_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVVIDEOPRESET_HPP_

#include "LibAVPresetParser.hpp"
#include "LibAVPresetFileParser.hpp"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

namespace bfs = boost::filesystem;

class LibAVVideoPreset
{
public:
	LibAVVideoPreset( const std::string& id )
		: presetID( id )
	{
		BOOST_FOREACH( const std::string& presetPath, LibAVPresetParser::researchPresetsFiles() )
		{
			bfs::path f = presetPath;
			if( f.extension() == kPresetVideoExtension )
			{
				LibAVPresetFileParser fp( presetPath );
				if( presetID == fp.getId() )
				{
					params = fp.getParameters();
				}
			}
		}
	}
	
	~LibAVVideoPreset()
	{
		
	}
	
	static void getPresetList( std::vector<std::string>& presetID )
	{
		LibAVPresetParser::getPresetList( presetID, kPresetVideoExtension );
	}
	
	static void getPresetList( std::vector<std::string>& presetID, std::vector<std::string>& presetLabelID )
	{
		LibAVPresetParser::getPresetList( presetID, presetLabelID, kPresetVideoExtension );
	}
	
	std::string getPresetID() { return presetID; }
	std::string getVideoCodec() { return params.find( "vcodec" )->second.at(0); }
	
	PresetParameters& getParameters() { return params; }
	
	PresetsList getPresetsPathList()
	{
		return LibAVPresetParser::getPresetsPathList( kPresetVideoExtension );
	}
	
private:
	std::string      presetID;
	PresetParameters params;
	
};

}
}
}
#endif
