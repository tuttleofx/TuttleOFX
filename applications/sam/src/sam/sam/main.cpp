#include <tuttle/common/exceptions.hpp>

#include <tuttle/host/Core.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

int main( int argc, char** argv )
{
	if( argc <= 1 ) // no argument
	{
		TUTTLE_COUT(
			"sam: missing operands.\n"
			"'sam --help' for more informations.\n"
			);
		exit( -1 );
	}

	TUTTLE_TCOUT_VAR( argc );
	TUTTLE_TCOUT_VAR( argv[0] );
	
	try
	{
		namespace bpo = boost::program_options;
	
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
		bpo::options_description infoOptions( "\tOptions" );
		infoOptions.add_options()
			("help,h"       , "show node help")
			("version,v"    , "display version")
			("commands,c"   , "show list of all available sam commands")
		;
		bpo::options_description all_options;
		all_options.add(infoOptions);

		bpo::variables_map sam_vm;
		bpo::store( bpo::command_line_parser(cl_options).options(all_options).run(), sam_vm );
		if( const char* env_ptr = std::getenv("SAM_OPTIONS") )
		{
			std::vector<std::string> envOptions;
			std::string env( env_ptr );
			envOptions.push_back( env );
			bpo::store( bpo::command_line_parser(envOptions).options(all_options).run(), sam_vm );
		}

		bpo::notify( sam_vm );

		if( sam_vm.count("help") )
		{
			TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]" );
			TUTTLE_COUT( "" );
			TUTTLE_COUT( "NAME" );
			TUTTLE_COUT( "\tsam - A set of command line tools." );
			TUTTLE_COUT( "" );
			TUTTLE_COUT( "SYNOPSIS" );
			TUTTLE_COUT( "\tsam command [options]..." );
			TUTTLE_COUT( "" );
			TUTTLE_COUT( "DESCRIPTION" );
			TUTTLE_COUT( infoOptions );
			exit( 0 );
		}
		if( sam_vm.count("version") )
		{
			TUTTLE_COUT( "TuttleOFX Host - version " << TUTTLE_HOST_VERSION_STR );
			exit( 0 );
		}
		
		boost::filesystem::path fullsam( argv[0] );
		if( fullsam.is_relative() )
		{
			fullsam = boost::filesystem::current_path() / fullsam;
		}
		boost::filesystem::path samDirectory( fullsam.parent_path() );
		
		TUTTLE_TCOUT_VAR( fullsam );
		TUTTLE_TCOUT_VAR( samDirectory );
		
		const std::string sam_tool( argv[cl_options.size() + 1] );
		std::vector<std::string> sam_cmd;
		for( int i = cl_options.size() + 2; i < argc; ++i )
		{
			const std::string s( argv[i] );
			sam_cmd.push_back( s );
		}
		
		if( sam_vm.count("commands") )
		{
			TUTTLE_COUT( "COMMANDS" );
			/// @todo
			exit( 0 );
		}
		
		std::vector<boost::filesystem::path> searchPaths;
		const std::string toolFilename( (std::string("sam-") + sam_tool) );
		boost::filesystem::path tmpToolFullPath;
		boost::filesystem::path toolFullPath;
		if( const char* env_ptr = std::getenv("SAM_PATH") )
		{
			const std::string env( env_ptr );
			const std::vector<std::string> paths = boost::program_options::split_unix( env, ":;" );
			searchPaths.insert( searchPaths.end(), paths.begin(), paths.end() );
		}
		searchPaths.push_back( samDirectory / "sam" );
		searchPaths.push_back( samDirectory );
		
		BOOST_FOREACH( const boost::filesystem::path& p, searchPaths )
		{
			const boost::filesystem::path tmpToolFullPath = p / toolFilename;
			if( boost::filesystem::exists( tmpToolFullPath ) )
			{
				toolFullPath = tmpToolFullPath;
				break;
			}
		}

		if( toolFullPath.empty() )
		{
			TUTTLE_CERR(
				"Sam command \"" + sam_tool + "\" not found."
				"\n"
				);
//			displayAvailableCommands();
			exit( -1 );
		}
		
		const std::string fullcmd( toolFullPath.string() + " " + boost::algorithm::join( sam_cmd, " " ) );
		
		TUTTLE_TCOUT_VAR( fullcmd );
		
		return system( fullcmd.c_str() );
	}
	catch( const boost::program_options::error& e )
	{
		TUTTLE_CERR( "sam");
		TUTTLE_CERR( "Error: " << e.what() );
		exit( -2 );
	}
	catch( ... )
	{
		TUTTLE_CERR( "sam");
		TUTTLE_CERR( boost::current_exception_diagnostic_information() );
		exit( -2 );
	}
	return 0;
}

