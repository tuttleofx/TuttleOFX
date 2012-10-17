#include "FFmpegPreset.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <cstdio>
#include <cstring>
#include <fstream>


namespace tuttle {
namespace plugin {
namespace ffmpeg {

namespace bfs = boost::filesystem;

#ifndef __WINDOWS__
#define DIRLIST_SEP_CHARS ":;"
#else
#define DIRLIST_SEP_CHARS ";"
#endif

#define FFMPEG_PRESET_EXTENSION ".ffpreset"
#define FFMPEG_PRESET_SUBDIR    ".ffmpeg"


FFmpegPreset::FFmpegPreset()
{
	researchPresets();
}

FFmpegPreset::Presets FFmpegPreset::getCodecListWithConfig() const
{
	Presets res;
	BOOST_FOREACH( const std::string& config, _presets )
	{
		const std::string codec = getCodecName( config );
		
		bool exist = false;
		BOOST_FOREACH( const std::string& file, res )
		{
			if( file == codec )
			{
				exist = true;
				break;
			}
		}
		if( !exist )
		{
			res.push_back( codec );
		}
	}
	return res;
}

std::string FFmpegPreset::getFilename( const std::string& codec, const std::string& preset ) const
{
	std::string filename;
	BOOST_FOREACH( const std::string& config, _presets )
	{
		const std::string c = getCodecName( config );
		const std::string p = getConfigName( config );
		
		if( c == codec && p == preset )
		{
			filename = config;
			break;
		}
	}
	return filename;
}

std::string FFmpegPreset::getCodecName( const std::string& filepath ) const
{
	std::vector<std::string> splitFilename;
	boost::split( splitFilename, bfs::path( filepath ).stem().string(), boost::is_any_of("-") );
	return splitFilename[0];
}

std::string FFmpegPreset::getConfigName( const std::string& filepath ) const
{
	std::vector<std::string> splitFilename;
	boost::split( splitFilename, bfs::path( filepath ).stem().string(), boost::is_any_of("-.") ); //< @todo: why "-."?
	return splitFilename[1]; //< @todo: why 1?
}

FFmpegPreset::Presets FFmpegPreset::getConfigList( const std::string& codec ) const
{
	Presets res;
	BOOST_FOREACH( const std::string& config, _presets )
	{
		const std::string c = getCodecName( config );
		
		if( c == codec )
		{
			res.push_back( getConfigName( config ) );
		}
	}
	return res;
}

void FFmpegPreset::researchPresets()
{
	Presets files = getPresetConfigurations( "FFMPEG_DATADIR" );
	
	BOOST_FOREACH( const std::string& preset, files )
	{
		_presets.push_back( preset );
		//TUTTLE_TCOUT( getCodecName( preset ) << " @ " << getConfigName( preset ) << " : " << preset );
	}
	
	files = getPresetConfigurations( "TUTTLE_HOME" );
	BOOST_FOREACH( const std::string& preset, files )
	{
		_presets.push_back( preset );
		//TUTTLE_TCOUT( getCodecName( preset ) << " @ " << getConfigName( preset ) << " : " << preset );
	}
	
#ifndef _WIN32
	files = getPresetConfigurations( "HOME" );
	BOOST_FOREACH( const std::string& preset, files )
	{
		_presets.push_back( preset );
		//TUTTLE_TCOUT( getCodecName( preset ) << " @ " << getConfigName( preset ) << " : " << preset );
	}
#endif
}

FFmpegPreset::Presets FFmpegPreset::getPresetConfigurations( const std::string& envVar )
{
	Presets presetPaths;
 
	if( const char* const dataDir = std::getenv( envVar.c_str() ) )
	{
		const std::string s( dataDir );
		boost::split( presetPaths, s, boost::is_any_of(DIRLIST_SEP_CHARS) );
	}
	
	Presets configs;
	BOOST_FOREACH( const bfs::path presetPath, presetPaths )
	{
		if( bfs::exists( presetPath ) && bfs::is_directory( presetPath ) )
		{
			bfs::directory_iterator itEnd;
			for( bfs::directory_iterator iter( presetPath ); iter != itEnd; ++iter )
			{
				bfs::path f = iter->path().filename();
				if( f.extension() == FFMPEG_PRESET_EXTENSION )
					configs.push_back( iter->path().string() );
			}
			// research into the subdir (if exist)
			bfs::path p = presetPath / FFMPEG_PRESET_SUBDIR;
			if( bfs::exists( p ) && bfs::is_directory( p ) )
			{
				for( bfs::directory_iterator iter( p ); iter != itEnd; ++iter )
				{
					bfs::path f = iter->path().filename();
					if( f.extension() == FFMPEG_PRESET_EXTENSION )
						configs.push_back( iter->path().string() );
				}
			}
		}
	}
	return configs;
}

FFmpegPreset::PresetsOptions FFmpegPreset::getOptionsForPresetFilename( const std::string& presetFile )
{
	PresetsOptions opts;
	std::ifstream inPreset;
	
	inPreset.open( presetFile.c_str(), std::ifstream::in ); 
	if( ! inPreset.is_open() )
	{
		using namespace tuttle::plugin;
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user() + "ffmpegPreset: Unable to open preset file."
			<< exception::filename( presetFile ) );
	}
	
	std::size_t lineNum = 0;
	std::string line;
	while( std::getline( inPreset, line ) )
	{
		std::vector<std::string> opt;
		boost::split( opt, line, boost::is_any_of("= ") );
		
		if( opt.size() <= 1 )
			continue;
		
		if( opt[0][0] == '#' || opt[0][0] == '\n' || opt[0][0] == '\r' )
		{
			//TUTTLE_TCOUT("comment : " << line );
			continue;
		}
		
		if( opt.size() > 2 && opt[2][0] != '#' )
		{
			TUTTLE_CERR( "ffmpegPreset: Unable to parse option, ignore: " << lineNum << ":\"" << line << "\"" );
			continue; // ignore this line
		}
		
		//TUTTLE_COUT( "option: " << opt[0] << " => " << opt[1] );
		opts[ opt[0] ] = opt[1];
		
		++lineNum;
	}
	inPreset.close();
	return opts;
}

}
}
}
