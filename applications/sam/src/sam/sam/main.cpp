#include <sam/common/utility.hpp>
#include <sam/common/color.hpp>

#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/common/exceptions.hpp>

#include <tuttle/host/Core.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>


namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

namespace sam {

Color _color;

bfs::path retrieveToolFullPath( const std::string& toolName, const std::vector<bfs::path>& searchPaths )
{
	const std::string toolFilename( std::string("sam-") + toolName );
	
	BOOST_FOREACH( const bfs::path& p, searchPaths )
	{
		const bfs::path toolFullPath = p / toolFilename;
		if( bfs::exists( toolFullPath ) )
		{
			return toolFullPath;
		}
	}

	/// @todo exception ?
	TUTTLE_CERR( _color._red << "Sam command \"" << toolName << "\" not found." << _color._std );
	// displayAvailableCommands();
	exit( -1 );
}

std::vector<bfs::path> retrieveSearchPaths( const bfs::path& samDirectory )
{
	std::vector<bfs::path> searchPaths;
	if( const char* env_ptr = std::getenv("SAM_PATH") )
	{
		const std::string env( env_ptr );
		const std::vector<std::string> paths = bpo::split_unix( env, ":;" );
		searchPaths.insert( searchPaths.end(), paths.begin(), paths.end() );
	}
	
	searchPaths.push_back( samDirectory / "sam" );
	searchPaths.push_back( samDirectory );
	
	return searchPaths;
}

std::vector<bfs::path> retrieveAllSamCommands( const bfs::path& dir )
{
	std::vector<bfs::path> res;
	if( ! bfs::is_directory(dir) )
		return res;
	
	bfs::directory_iterator dir_iter(dir), dir_end;
	for(;dir_iter != dir_end; ++dir_iter)
	{
		bfs::path cmd = *dir_iter;
		if( boost::starts_with( cmd.filename().string(), "sam-" ) )
		{
			res.push_back( cmd );
		}
	}
	return res;
}

std::vector<bfs::path> retrieveAllSamCommands( const std::vector<bfs::path>& dirs )
{
	typedef std::map<bfs::path,bfs::path> PathMap;
	PathMap allRes;
	BOOST_FOREACH( const bfs::path& dir, dirs )
	{
		std::vector<bfs::path> dirRes = retrieveAllSamCommands( dir );
		BOOST_FOREACH( const bfs::path& cmd, dirRes )
		{
			if( allRes.find( cmd.filename() ) == allRes.end() )
			{
				allRes[cmd.filename()] = cmd;
			}
		}
	}
	std::vector<bfs::path> res;
	res.reserve( allRes.size() );
	BOOST_FOREACH( PathMap::const_reference p, allRes )
	{
		res.push_back( p.second );
	}
	return res;
}

}

int main( int argc, char** argv )
{
	using namespace sam;
	if( argc <= 1 ) // no argument
	{
		TUTTLE_COUT( "sam: missing operands." );
		TUTTLE_COUT( "run 'sam --help' for more informations." << std::endl);
		exit( -1 );
	}

	bool                     enableColor = false;

	try
	{
	
		std::vector<std::string> cl_options;
		for( int i = 1; i < argc; ++i )
		{
			const std::string s( argv[i] );
			if( ! boost::starts_with( s, "-" ) )
			{
				break;
			}
			cl_options.push_back( s );
		}
		
		// Declare the supported options.
		bpo::options_description infoOptions;
		infoOptions.add_options()
			("help,h"       , "show node help")
			("version,v"    , "display version") // which version?
			("commands,c"   , "show list of all available sam commands")
			("color"        , "color the output")
		;

		// describe hidden options
		bpo::options_description hidden;
		hidden.add_options()
			("commands-list" , "show list of all available sam commands")
			("binaries-list" , "show list of all available sam binaries")
			("enable-color", bpo::value<std::string>(), "enable (or disable) color")
		;

		bpo::options_description all_options;
		all_options.add(infoOptions).add(hidden);

		bpo::variables_map sam_vm;

		//parse the command line, and put the result in vm
		bpo::store( bpo::command_line_parser(cl_options).options(all_options).run(), sam_vm );
		// get environment options and parse them
		if( const char* env_sam_options = std::getenv("SAM_OPTIONS") )
		{
			const std::vector<std::string> envOptions = bpo::split_unix( env_sam_options, " " );
			bpo::store(bpo::command_line_parser(envOptions).options(all_options).run(), sam_vm);
		}
		bpo::notify( sam_vm );

		if ( sam_vm.count("color") )
		{
			enableColor = true;
		}
		if ( sam_vm.count("enable-color") )
		{
			const std::string str = sam_vm["enable-color"].as<std::string>();
			enableColor = string_to_boolean( str );
		}

		if( enableColor )
		{
			_color.enable();
		}


		if( sam_vm.count("help") )
		{
			TUTTLE_COUT( _color._blue  << "TuttleOFX project [http://sites.google.com/site/tuttleofx]" << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "NAME" << _color._std );
			TUTTLE_COUT( _color._green <<"\tsam - A set of command line tools." << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "SYNOPSIS" << _color._std );
			TUTTLE_COUT( _color._green << "\tsam COMMAND [options]..." << _color._std << std::endl );
			TUTTLE_COUT( _color._blue  << "DESCRIPTION" << _color._std );
			TUTTLE_COUT( "Sam is the TuttleOFX command line tool to manage image processing" << std::endl );
			TUTTLE_COUT( _color._blue  << "OPTIONS" << _color._std );
			TUTTLE_COUT( infoOptions );
			exit( 0 );
		}
		if( sam_vm.count("version") )
		{
			TUTTLE_COUT( "TuttleOFX Host - version " << TUTTLE_HOST_VERSION_STR );
			exit( 0 );
		}
		
		bfs::path fullsam( argv[0] );
		if( fullsam.is_relative() )
		{
			fullsam = bfs::current_path() / fullsam;
		}
		bfs::path samDirectory( fullsam.parent_path() );

		const std::vector<bfs::path> searchPaths = retrieveSearchPaths( samDirectory );

		if( sam_vm.count("commands") )
		{
			TUTTLE_COUT( "" );
			TUTTLE_COUT( _color._blue << "COMMANDS" << _color._std );
			
			const std::vector<bfs::path> cmds = retrieveAllSamCommands( searchPaths );
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				std::cout << std::left <<  "\t" << std::setw(10) << c.filename().string().substr(4) << std::flush ;
				system( (c.string()+" --brief").c_str() );
				std::cout << std::flush;
			}
			TUTTLE_COUT( "" );

			exit( 0 );
		}
		if( sam_vm.count("binaries-list") )
		{
			const std::vector<bfs::path> cmds = retrieveAllSamCommands( searchPaths );
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				TUTTLE_COUT( c.string() );
			}
			exit( 0 );
		}
		if( sam_vm.count("commands-list") )
		{
			const std::vector<bfs::path> cmds = retrieveAllSamCommands( searchPaths );
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				TUTTLE_COUT( c.filename().string() );
			}
			exit( 0 );
		}
		
		const std::string sam_tool( argv[cl_options.size() + 1] );
		std::vector<std::string> sam_cmd;
		for( int i = cl_options.size() + 2; i < argc; ++i )
		{
			const std::string s( argv[i] );
			sam_cmd.push_back( s );
		}
		
		bfs::path toolFullPath = retrieveToolFullPath( sam_tool, searchPaths );
		
		const std::string fullcmd( toolFullPath.string() + " " + boost::algorithm::join( sam_cmd, " " ) );
		
//		TUTTLE_TCOUT_VAR( fullcmd );
		return system( fullcmd.c_str() );
	}
	catch( const bpo::error& e )
	{
		TUTTLE_CERR( _color._red << "Error in command line: " << e.what() << _color._std );
		exit( -2 );
	}
	catch( ... )
	{
		TUTTLE_CERR( _color._red << "Error in command line: " << boost::current_exception_diagnostic_information() << _color._std );
		exit( -2 );
	}
	return 0;
}

