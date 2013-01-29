#include "LibAVPresetParser.hpp"
#include "LibAVPresetFileParser.hpp"

#include <tuttle/plugin/global.hpp>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

namespace bfs = boost::filesystem;

LibAVPresetParser::LibAVPresetParser()
{
}

LibAVPresetParser::~LibAVPresetParser()
{
}

PresetsList LibAVPresetParser::researchPresetsFiles()
{
	PresetsList presetList;
	
	PresetsList files = getPresetListForPathList( "LIBAV_DATADIR" );
	BOOST_FOREACH( const std::string& preset, files )
	{
		presetList.push_back( preset );
	}
	
	files = getPresetListForPathList( "TUTTLE_HOME" );
	BOOST_FOREACH( const std::string& preset, files )
	{
		presetList.push_back( preset );
	}
	
#ifndef _WIN32
	files = getPresetListForPathList( "HOME" );
	BOOST_FOREACH( const std::string& preset, files )
	{
		presetList.push_back( preset );
	}
#endif
	return presetList;
}

PresetsList LibAVPresetParser::getPresetListForPathList( const std::string& envVar )
{
	PresetsList presetPaths;
 
	if( const char* const dataDir = std::getenv( envVar.c_str() ) )
	{
		const std::string s( dataDir );
		boost::split( presetPaths, s, boost::is_any_of( DIRLIST_SEP_CHARS ) );
	}
	
	PresetsList configs;
	BOOST_FOREACH( const bfs::path presetPath, presetPaths )
	{
		if( bfs::exists( presetPath ) && bfs::is_directory( presetPath ) )
		{
			bfs::directory_iterator itEnd;
			for( bfs::directory_iterator iter( presetPath ); iter != itEnd; ++iter )
			{
				if( isValidExtension( iter->path().string() ) )
				{
					configs.push_back( iter->path().string() );
				}
			}
			// research into the subdir (if exist)
			bfs::path p = presetPath / kPresetSubDirPath;
			if( bfs::exists( p ) && bfs::is_directory( p ) )
			{
				for( bfs::directory_iterator iter( p ); iter != itEnd; ++iter )
				{
					if( isValidExtension( iter->path().string() ) )
					{
						configs.push_back( iter->path().string() );
					}
				}
			}
		}
	}
	return configs;
}

bool LibAVPresetParser::isValidExtension( const std::string& filename )
{
	bfs::path f = filename;
	
	return  ( f.extension() == kPresetExtension ) ||
			( f.extension() == kPresetFormatExtension ) ||
			( f.extension() == kPresetVideoExtension ) ||
			( f.extension() == kPresetAudioExtension );
}

void LibAVPresetParser::getPresetList( std::vector<std::string>& presetID, const std::string& extension )
{
	BOOST_FOREACH( const std::string& presetPath, researchPresetsFiles() )
	{
		bfs::path f = presetPath;
		if( f.extension() == extension )
		{
			presetID.push_back( LibAVPresetFileParser::getId( presetPath ) );
		}
	}
}

void LibAVPresetParser::getPresetList( std::vector<std::string>& presetID, std::vector<std::string>& presetLabelID , const std::string& extension )
{
	BOOST_FOREACH( const std::string& presetPath, researchPresetsFiles() )
	{
		bfs::path f = presetPath;
		if( f.extension() == extension )
		{
			LibAVPresetFileParser fp( presetPath );
			presetID.push_back( fp.getId() );
			presetLabelID.push_back( fp.getIdLabel() );
		}
	}
}

PresetsList getPresetsPathList( const std::string& extension = "" )
{
	PresetsList list;
	BOOST_FOREACH( const std::string& presetPath, LibAVPresetParser::researchPresetsFiles() )
	{
		bfs::path f = presetPath;
		if( f.extension() == extension )
			list.push_back( presetPath );
	}
	return list;
}

PresetsList getPresetsFiles( const std::string& extension = "" )
{
	PresetsList list;
	BOOST_FOREACH( const std::string& presetPath, LibAVPresetParser::researchPresetsFiles() )
	{
		bfs::path f = presetPath;
		if( f.extension() == extension )
			LibAVPresetFileParser preset( presetPath );
	}
	return list;
}


}
}
}
