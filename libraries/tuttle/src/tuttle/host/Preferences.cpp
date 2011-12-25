#include "Preferences.hpp"

#include <tuttle/common/system/system.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace tuttle {
namespace host {

Preferences::Preferences()
: _home( buildTuttleHome() )
, _temp( buildTuttleTemp() )
{}

boost::filesystem::path Preferences::buildTuttleHome()
{
	boost::filesystem::path tuttleHome;
	char* env_tuttle_cache = NULL;
	if(( env_tuttle_cache = std::getenv("TUTTLE_HOME") ))
	{
		tuttleHome = env_tuttle_cache;
	}
#ifdef __WINDOWS__
	else if( ( env_tuttle_cache = std::getenv("APPDATA") ) )
	{
		// windows root path for application settings
		tuttleHome = boost::filesystem::path(env_tuttle_cache) / "TuttleOFX";
	}
	// if APPDATA doesn't exist, try to use USERPROFILE home directory
	// as on UNIX platforms even if it's not standard.
	else if( ( env_tuttle_cache = std::getenv("USERPROFILE") ) )
	{
		// windows root path for application settings
		tuttleHome = boost::filesystem::path(env_tuttle_cache) / ".tuttleofx";
	}
#else
	// UNIX home directory to store application settings
	else if(( env_tuttle_cache = std::getenv("HOME") )) // UNIX HOME
	{
		tuttleHome = boost::filesystem::path(env_tuttle_cache) / ".tuttleofx";
	}
#endif
	else // use temp directory
	{
		tuttleHome = boost::filesystem::temp_directory_path() / ".tuttleofx";
	}
	
	if( ! tuttleHome.empty() &&
	    ! boost::filesystem::exists( tuttleHome ) )
	{
		boost::filesystem::create_directories( tuttleHome );
	}
	
	return tuttleHome;
}

boost::filesystem::path Preferences::buildTuttleTemp()
{
	const boost::filesystem::path tuttleTmp = boost::filesystem::temp_directory_path() / "tuttle";
	if( ! boost::filesystem::exists( tuttleTmp ) )
	{
		boost::filesystem::create_directories( tuttleTmp );
	}
	
	return tuttleTmp;
}


}
}

