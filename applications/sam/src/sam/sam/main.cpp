#include <sam/common/utility.hpp>
#include <sam/common/options.hpp>

#include <tuttle/common/exceptions.hpp>
#include <tuttle/common/utils/applicationPath.hpp>

#include <tuttle/host/Core.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>

#include <climits>

namespace sam {

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

bfs::path retrieveToolFullPath(const std::string& toolName, const std::vector<bfs::path>& searchPaths)
{
	const std::string toolFilename(std::string("sam-") + toolName);
	
	BOOST_FOREACH( const bfs::path& p, searchPaths )
	{
		const bfs::path toolFullPath = p / toolFilename;
		if (bfs::exists(toolFullPath))
		{
			return toolFullPath;
		}
	}
	
	/// @todo exception ?
	TUTTLE_LOG_ERROR( "Sam command \"" << toolName << "\" not found." );
	// displayAvailableCommands();
	exit(255);
}

std::vector<bfs::path> retrieveSearchPaths(const bfs::path& samDirectory)
{
	std::vector<bfs::path> searchPaths;
	if( const char* env_ptr = std::getenv( "SAM_PATH" ) )
	{
		const std::string env(env_ptr);
		const std::vector<std::string> paths = bpo::split_unix(env, ":;");
		searchPaths.insert(searchPaths.end(), paths.begin(), paths.end());
	}
	
	searchPaths.push_back(samDirectory / "sam");
	searchPaths.push_back(samDirectory);
	
	return searchPaths;
}

std::vector<bfs::path> retrieveAllSamCommands(const bfs::path& dir)
{
	std::vector<bfs::path> res;
	if( !bfs::is_directory( dir ) )
		return res;
	
	bfs::directory_iterator dir_iter(dir), dir_end;
	for( ; dir_iter != dir_end; ++dir_iter )
	{
		bfs::path cmd = *dir_iter;
		if( boost::starts_with(cmd.filename().string(), "sam-" ) )
		{
			res.push_back( cmd );
		}
	}
	return res;
}

std::vector<bfs::path> retrieveAllSamCommands(const std::vector<bfs::path>& dirs)
{
	typedef std::map<bfs::path, bfs::path> PathMap;
	PathMap allRes;
	BOOST_FOREACH( const bfs::path& dir, dirs )
	{
		std::vector<bfs::path> dirRes = retrieveAllSamCommands(dir);
		BOOST_FOREACH( const bfs::path& cmd, dirRes )
		{
			if( allRes.find( cmd.filename() ) == allRes.end() )
			{
				allRes[cmd.filename()] = cmd;
			}
		}
	}
	std::vector<bfs::path> res;
	res.reserve(allRes.size());
	BOOST_FOREACH( PathMap::const_reference p, allRes )
	{
		res.push_back( p.second );
	}
	return res;
}

}

int main( int argc, char** argv )
{
	signal(SIGINT, signal_callback_handler);

	using namespace tuttle::common;
	using namespace sam;
	
	boost::shared_ptr<formatters::Formatter> formatter( formatters::Formatter::get() );
	boost::shared_ptr<Color>                 color( Color::get() );
	
	formatter->init_logging();
	
	try
	{
		std::vector<std::string> cl_options;
		for (int i = 1; i < argc; ++i)
		{
			const std::string s(argv[i]);
			if (!boost::starts_with(s, "-"))
			{
				break;
			}
			cl_options.push_back(s);
		}
		
		// Declare the supported options.
		bpo::options_description infoOptions;
		infoOptions.add_options()
				( kHelpOptionString,     kHelpOptionMessage )
				( kVersionOptionString,  kVersionOptionMessage ) // which version?
				( kCommandsOptionString, kCommandsOptionMessage )
				( kColorOptionString,    kColorOptionMessage );
		
		// describe hidden options
		bpo::options_description hidden;
		hidden.add_options()
				( kCommandsListOptionString, kCommandsListOptionMessage )
				( kBinariesListOptionString, kBinariesListOptionMessage )
				( kEnableColorOptionString, bpo::value<std::string>(), kEnableColorOptionMessage );
		
		bpo::options_description all_options;
		all_options.add(infoOptions).add(hidden);
		
		bpo::variables_map sam_vm;
		
		//parse the command line, and put the result in vm
		bpo::store(bpo::command_line_parser(cl_options).options(all_options).run(), sam_vm);
		// get environment options and parse them
		if( const char* env_sam_options = std::getenv( "SAM_OPTIONS" ) )
		{
			const std::vector<std::string> envOptions = bpo::split_unix(env_sam_options, " ");
			bpo::store(bpo::command_line_parser(envOptions).options(all_options).run(), sam_vm);
		}
		bpo::notify(sam_vm);
		
		if( sam_vm.count( kColorOptionLongName ) )
		{
			color->enable();
		}
		
		if( sam_vm.count( kEnableColorOptionLongName ) )
		{
			const std::string str = sam_vm[kEnableColorOptionLongName].as<std::string>();
			if( string_to_boolean(str) )
			{
				color->enable();
			}
			else
			{
				color->disable();
			}
		}

		bfs::path fullsam = tuttle::common::applicationFilepath( argv[0] );
		
		bfs::path samDirectory( fullsam.parent_path() );
		const std::vector<bfs::path> searchPaths = retrieveSearchPaths( samDirectory );
		
		if( ( sam_vm.count( kHelpOptionLongName ) || ( argc == 1 ) )
				|| ( ( argc == 2 ) && (strstr( argv[1], kColorOptionLongName ) != NULL ) ) )
		{
			TUTTLE_LOG_INFO( color->_blue << "TuttleOFX project [" << kUrlTuttleofxProject << "]" << color->_std );
			TUTTLE_LOG_INFO( "" );
			TUTTLE_LOG_INFO( color->_blue << "NAME" << color->_std );
			TUTTLE_LOG_INFO( color->_green <<"\tsam - A set of command line tools." << color->_std );
			TUTTLE_LOG_INFO( "" );
			TUTTLE_LOG_INFO( color->_blue << "SYNOPSIS" << color->_std );
			TUTTLE_LOG_INFO( color->_green << "\tsam COMMAND [options]..." << color->_std );
			TUTTLE_LOG_INFO( "" );
			TUTTLE_LOG_INFO( color->_blue << "COMMANDS" << color->_std );
			const std::vector<bfs::path> cmds = retrieveAllSamCommands( searchPaths );
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				std::cout << std::left << "\t" << std::setw(10) << c.filename().string().substr(4) << std::flush;
				system((c.string() + " --brief").c_str());
				std::cout << std::flush;
			}
			TUTTLE_LOG_INFO( "" );
			TUTTLE_LOG_INFO( color->_blue << "DESCRIPTION" << color->_std );
			TUTTLE_LOG_INFO( "Sam is the TuttleOFX command line tool to manage image processing." );
			TUTTLE_LOG_INFO( "" );
			TUTTLE_LOG_INFO( color->_blue << "OPTIONS" << color->_std );
			TUTTLE_LOG_INFO( infoOptions );
			exit( 0 );
		}
		
		if( sam_vm.count( kVersionOptionLongName ) )
		{
			TUTTLE_LOG_INFO( "TuttleOFX Host - version " << TUTTLE_HOST_VERSION_STR);
			exit(0);
		}
		
		if( sam_vm.count(kCommandsOptionLongName ) )
		{
			TUTTLE_LOG_INFO( "" );
			TUTTLE_LOG_INFO( color->_blue << "COMMANDS" << color->_std );
			
			const std::vector<bfs::path> cmds = retrieveAllSamCommands(searchPaths);
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				std::cout << std::left << "\t" << std::setw(10) << c.filename().string().substr(4) << std::flush;
				system((c.string() + " --brief").c_str());
				std::cout << std::flush;
			}
			TUTTLE_LOG_INFO( "" );
			
			exit(0);
		}
		if( sam_vm.count( kBinariesListOptionLongName ) )
		{
			const std::vector<bfs::path> cmds = retrieveAllSamCommands( searchPaths );
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				TUTTLE_LOG_INFO( c.string());
			}
			exit(0);
		}
		if( sam_vm.count( kCommandsListOptionLongName ) )
		{
			const std::vector<bfs::path> cmds = retrieveAllSamCommands( searchPaths );
			BOOST_FOREACH( const bfs::path& c, cmds )
			{
				TUTTLE_LOG_INFO( c.filename().string().substr(4) );
			}
			exit(0);
		}
		
		const std::string sam_tool(argv[cl_options.size() + 1]);
		std::vector<std::string> sam_cmd;
		for( int i = cl_options.size() + 2; i < argc; ++i )
		{
			static const std::string quote("\"");
			const std::string s(argv[i]);
			sam_cmd.push_back(quote + s + quote);
		}
		
		bfs::path toolFullPath = retrieveToolFullPath(sam_tool, searchPaths);
		
		const std::string fullcmd( toolFullPath.string() + " " + boost::algorithm::join( sam_cmd, " " ) );
		
		// TUTTLE_TLOG_VAR( TUTTLE_INFO, fullcmd );
		int returnCode = system( fullcmd.c_str() );
		// TUTTLE_CLOG_VAR( TUTTLE_INFO, UINT_MAX - returnCode );
		return returnCode>>8;
	}
	catch( const bpo::error& e )
	{
		TUTTLE_LOG_ERROR( "Error in command line: " << e.what() );
		exit(254);
	}
	catch( ... )
	{
		TUTTLE_LOG_ERROR( "Error in command line: " << boost::current_exception_diagnostic_information() );
		exit(254);
	}
	
	return 0;
}

