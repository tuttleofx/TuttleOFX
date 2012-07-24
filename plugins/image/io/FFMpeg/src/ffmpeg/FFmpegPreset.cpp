#include "FFmpegPreset.hpp"

#include <tuttle/plugin/global.hpp>

#include <cstdio>
#include <cstring>
#include <fstream>

#include <boost/algorithm/string.hpp>


#define FFMPEG_PRESET_EXTENSION ".ffpreset"
#define FFMPEG_PRESET_SUBDIR    ".ffmpeg"

FFmpegPreset::FFmpegPreset()
{
	researchPresets();
}

std::string FFmpegPreset::getCodecName( const std::string& path )
{
	std::vector<std::string> splitPath;
	std::vector<std::string> splitFilename;
	boost::split( splitPath, path, boost::is_any_of("/") );
	
	boost::split( splitFilename, splitPath.at( splitPath.size() - 1 ), boost::is_any_of("-") );
	return splitFilename.at( 0 );
}

std::string FFmpegPreset::getConfigName( const std::string& path )
{
	std::vector<std::string> splitPath;
	std::vector<std::string> splitFilename;
	boost::split( splitPath, path, boost::is_any_of("/") );
	
	boost::split( splitFilename, splitPath.at( splitPath.size() - 1 ), boost::is_any_of("-.") );
	return splitFilename.at( 1 );
}

FFmpegPreset::Presets FFmpegPreset::getConfigList( const std::string& codec )
{
	std::vector<std::string> list;
	Presets::iterator config;
	for ( config = presets.begin() ; config != presets.end(); config++ )
	{
		std::string c = getCodecName( *config );
		
		if( std::strcmp( c.c_str(), codec.c_str() ) == 0 )
		{
			list.push_back( getConfigName( *config ) );
		}
	}
	return list;
}

void FFmpegPreset::researchPresets()
{
	std::vector<std::string> files;
	
	files = getPresetConfigurations( "FFMPEG_DATADIR" );
	
	std::vector<std::string>::iterator fileIt;
	for( fileIt = files.begin(); fileIt < files.end(); fileIt++ )
	{
		std::string preset = (*fileIt);
		presets.push_back( preset );
		//TUTTLE_TCOUT( getCodecName( preset ) << " @ " << getConfigName( preset ) << " : " << preset );
	}
	
	files = getPresetConfigurations( "TUTTLE_HOME" );
	
	for( fileIt = files.begin(); fileIt < files.end(); fileIt++ )
	{
		std::string preset = (*fileIt);
		presets.push_back( preset );
		//TUTTLE_TCOUT( getCodecName( preset ) << " @ " << getConfigName( preset ) << " : " << preset );
	}
	
#ifndef _WIN32
	files = getPresetConfigurations( "HOME" );
	
	for( fileIt = files.begin(); fileIt < files.end(); fileIt++ )
	{
		std::string preset = (*fileIt);
		presets.push_back( preset );
		//TUTTLE_TCOUT( getCodecName( preset ) << " @ " << getConfigName( preset ) << " : " << preset );
	}
#endif
}

std::vector<std::string> FFmpegPreset::getPresetConfigurations( std::string environementVariable )
{
	std::vector<std::string> paths;
	std::vector<std::string> configs;
	
	if( const char* dataDir = std::getenv( environementVariable.c_str() ) )
	{
		boost::split( paths, dataDir, boost::is_any_of(":") );
	}
	
	std::vector<std::string>::iterator basePath;
	int i = 0;
	for( basePath = paths.begin(); basePath < paths.end(); basePath++, i++ )
	{
		bfs::path path = (*basePath);
		
		if( bfs::exists( path ) && bfs::is_directory( path ) )
		{
			bfs::directory_iterator itEnd;
			for( bfs::directory_iterator iter( path ); iter != itEnd; ++iter )
			{
				bfs::path f = iter->path().filename();
				if( std::strcmp( f.extension().c_str(), FFMPEG_PRESET_EXTENSION ) == 0 )
					configs.push_back( iter->path().string() );
			}
			// research into the subdir (if exist)
			bfs::path p = path / FFMPEG_PRESET_SUBDIR;
			if( bfs::exists( p ) && bfs::is_directory( p ) )
			{
				for( bfs::directory_iterator iter( p ); iter != itEnd; ++iter )
				{
					bfs::path f = iter->path().filename();
					if( strcmp( f.extension().c_str(), FFMPEG_PRESET_EXTENSION ) == 0 )
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
		std::cerr << "ffmpegPreset: Unable to open preset file" << presetFile << std::endl;
		return opts;
	}
	while( ! inPreset.eof() )
	{
#define LINE_SIZE 256
		char line[LINE_SIZE];
		
		inPreset.getline( line, LINE_SIZE );
		
		std::vector<std::string> opt;
		boost::split( opt, line, boost::is_any_of("= ") );
		
		if( opt.size() > 1 )
		{
			if( !( opt.at(0)[0] == '#' || opt.at(0)[0] == '\n' || opt.at(0)[0] == '\r' ) )
			{
				if( opt.size() == 2 || opt.at(2)[0] == '#' )
				{
					//TUTTLE_COUT( "option: " << opt.at(0) << " => " << opt.at(1) );
					opts[ opt.at(0) ] = opt.at(1);
				}
				else
				{
					TUTTLE_CERR( "ffmpegPreset: Unable to parse option : " << line );
				}
			}
			else
			{
				//TUTTLE_TCOUT("comment : " << line );
			}
		}
	}
	inPreset.close();
	return opts;
}

