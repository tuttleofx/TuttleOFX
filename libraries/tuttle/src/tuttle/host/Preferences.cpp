#include "Preferences.hpp"

#include <tuttle/common/system/system.hpp>
#include <tuttle/common/utils/global.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#ifdef __WINDOWS__
#include <windows.h>
#include <shlobj.h>
#elif defined(__UNIX__)
#include <pwd.h>
#include <sys/types.h>
#endif

namespace tuttle
{
namespace host
{

Preferences::Preferences()
    : _home(buildTuttleHome())
    , _temp(buildTuttleTemp())
{
}

boost::filesystem::path Preferences::buildTuttleHome() const
{
    boost::filesystem::path tuttleHome;

    char* env_tuttle_cache = NULL;
#ifdef __WINDOWS__
    char winPath[MAX_PATH];
#elif defined(__UNIX__)
    struct passwd* userInfos = NULL;
#endif

    if((env_tuttle_cache = std::getenv("TUTTLE_HOME")))
    {
        tuttleHome = env_tuttle_cache;
    }
#ifdef __WINDOWS__
    else if((env_tuttle_cache = std::getenv("APPDATA"))) // application data directory
    {
        // windows root path for application settings
        tuttleHome = boost::filesystem::path(env_tuttle_cache) / "TuttleOFX";
    }
    else if(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, winPath) == S_OK) // same as previous...
    {
        // windows root path for application settings
        tuttleHome = boost::filesystem::path(winPath) / "TuttleOFX";
    }
    // if APPDATA doesn't exist, try to use USERPROFILE home directory
    // as on UNIX platforms even if it's not standard.
    else if((env_tuttle_cache = std::getenv("USERPROFILE"))) // home directory
    {
        // windows root path for user home directory
        tuttleHome = boost::filesystem::path(env_tuttle_cache) / ".tuttleofx";
    }
    else if(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, winPath) == S_OK) // same as previous...
    {
        // windows root path for user home directory
        tuttleHome = boost::filesystem::path(winPath) / ".tuttleofx";
    }
#elif defined(__UNIX__)
    // UNIX home directory to store application settings
    else if((env_tuttle_cache = std::getenv("HOME"))) // UNIX HOME
    {
        /* getpwuid documentation:
         * An application that wants to determine its user's home directory
         * should inspect the value of HOME (rather than the value
         * getpwuid(getuid())->pw_dir ) since this allows the user to modify
         * their notion of "the home directory" during a login session.
         */
        tuttleHome = boost::filesystem::path(env_tuttle_cache) / ".tuttleofx";
    }
    else if((userInfos = getpwuid(getuid())) && userInfos->pw_dir)
    {
        tuttleHome = boost::filesystem::path(userInfos->pw_dir) / ".tuttleofx";
    }
#endif
    else // use temp directory
    {
        tuttleHome = boost::filesystem::temp_directory_path() / "tuttleofx";
    }

    if(!tuttleHome.empty() && !boost::filesystem::exists(tuttleHome))
    {
        boost::filesystem::create_directories(tuttleHome);
    }

    return tuttleHome;
}

boost::filesystem::path Preferences::buildTuttleTemp() const
{
    const boost::filesystem::path tuttleTmp = boost::filesystem::temp_directory_path() / "tuttle";
    if(!boost::filesystem::exists(tuttleTmp))
    {
        boost::filesystem::create_directories(tuttleTmp);
    }

    return tuttleTmp;
}

boost::filesystem::path Preferences::buildTuttleTestPath() const
{
    const boost::filesystem::path tuttleTest = boost::filesystem::current_path() / ".tests";
    if(!boost::filesystem::exists(tuttleTest))
    {
        boost::filesystem::create_directories(tuttleTest);
    }

    return tuttleTest;
}
}
}
