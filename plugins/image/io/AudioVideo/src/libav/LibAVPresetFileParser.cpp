#include "LibAVPresetFileParser.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cstdio>
#include <cstring>
#include <fstream>

namespace tuttle {
namespace plugin {
namespace av {

namespace bfs = boost::filesystem;

LibAVPresetFileParser::LibAVPresetFileParser( const std::string& filepath )
	: presetType( eUnknownPresetType )
{
	presetsParameters = parseFile( filepath );
}


LibAVPresetFileParser::~LibAVPresetFileParser()
{
	
}

std::string LibAVPresetFileParser::getId( const std::string& filepath )
{
	return bfs::path( filepath ).stem().string();
}

PresetParameters LibAVPresetFileParser::parseFile( const std::string& filepath )
{
	id = getId( filepath );
	std::string ext = bfs::path( filepath ).extension().string();
	
	if( ext == kPresetExtension )
	{
		presetType = eMainPresetType;
	}
	if( ext == kPresetFormatExtension )
	{
		presetType = eFormatPresetType;
	}
	if( ext == kPresetVideoExtension )
	{
		presetType = eVideoPresetType;
	}
	if( ext == kPresetAudioExtension )
	{
		presetType = eAudioPresetType;
	}
	
	PresetParameters opts;
	std::ifstream inPreset;
	
	inPreset.open( filepath.c_str(), std::ifstream::in ); 
	if( ! inPreset.is_open() )
	{
		using namespace tuttle::plugin;
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user() + "AV Preset: Unable to open preset file."
			<< exception::filename( filepath ) );
	}
	
	boost::property_tree::basic_ptree<std::string,std::string> pt;
	try
	{
		boost::property_tree::json_parser::read_json( inPreset, pt );
		boost::property_tree::basic_ptree<std::string,std::string>::const_iterator iter = pt.begin(), iterEnd = pt.end();
		for( ; iter != iterEnd; ++iter )
		{
			//TUTTLE_COUT( iter->first << " " << iter->second.get_value<std::string>() );
			std::vector<std::string> values;
			std::string value = iter->second.get_value<std::string>();
			
			if(  value.length() == 0 )
			{
				BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child( iter->first ) )
				{
					//TUTTLE_COUT( v.second.get_value<std::string>() );
					values.push_back( v.second.get_value<std::string>() );
				}
				opts[ iter->first ] = values;
			}
			else
			{
				if( iter->first == kPresetLabel )
				{
					idLabel = value;
				}
				else
				{
					values.push_back( iter->second.get_value<std::string>() );
					opts[ iter->first ] = values;
				}
			}
		}
	}
	catch( boost::property_tree::json_parser::json_parser_error &je )
	{
		TUTTLE_CERR( "Error parsing: " << je.filename() << " on line: " << je.line() );
		TUTTLE_CERR( je.message() );
	}

	inPreset.close();
	return opts;
	
}

void LibAVPresetFileParser::print() const
{
	std::string name = id;
	
	switch( presetType )
	{
		case eMainPresetType:    name += "(main)";    break;
		case eFormatPresetType:  name += "(format)";  break;
		case eVideoPresetType:   name += "(video)";   break;
		case eAudioPresetType:   name += "(audio)";   break;
		case eUnknownPresetType: name += "(unknown)"; break;
	}
	
	name += ": ";
	name += idLabel;
	
	TUTTLE_COUT_X( name.length() + 4, "*");
	TUTTLE_COUT( "* " << name << " *" );
	TUTTLE_COUT_X( name.length() + 4, "*");
	
	//TUTTLE_COUT_VAR( presetsParameters.size() );
	
	PresetParameters::const_iterator it = presetsParameters.begin();
	for( ; it != presetsParameters.end(); ++it )
		TUTTLE_COUT( it->first << ": " << it->second.at(0) );
	
	TUTTLE_COUT_X( name.length() + 4, "_");
}

}
}
}
