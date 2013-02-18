#ifndef _TUTTLE_PLUGIN_FFMPEG_LIBAVFORMATPRESET_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_LIBAVFORMATPRESET_HPP_

#include "LibAVPresetParser.hpp"
#include "LibAVPresetFileParser.hpp"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

namespace bfs = boost::filesystem;

class LibAVFormatPreset
{
public:
	LibAVFormatPreset( const std::string& id )
		: presetID( id )
	{
		BOOST_FOREACH( const std::string& presetPath, LibAVPresetParser::researchPresetsFiles() )
		{
			bfs::path f = presetPath;
			if( f.extension() == kPresetFormatExtension )
			{
				LibAVPresetFileParser fp( presetPath );
				if( presetID == fp.getId() )
				{
					params = fp.getParameters();
				}
			}
		}
	}
	
	~LibAVFormatPreset()
	{
		
	}
	
	static void getPresetList( std::vector<std::string>& presetID )
	{
		LibAVPresetParser::getPresetList( presetID, kPresetFormatExtension );
	}
	
	static void getPresetList( std::vector<std::string>& presetID, std::vector<std::string>& presetLabelID )
	{
		LibAVPresetParser::getPresetList( presetID, presetLabelID, kPresetFormatExtension );
	}
	
	std::string getPresetID() { return presetID; }
	std::string getFormat()   { return params.find( "format" )->second.at(0); }
	
	PresetParameters getParameters(){ return params; }
	
	PresetsList getPresetsPathList()
	{
		return LibAVPresetParser::getPresetsPathList( kPresetFormatExtension );
	}
	
private:
	std::string      presetID;
	PresetParameters params;
	
};

}
}
}
#endif
