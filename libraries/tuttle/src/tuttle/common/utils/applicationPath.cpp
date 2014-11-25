/*
 * This code is rewrite from the Trolltech Qt code applicationFilePath / applicationDirPath.
 */

#include "applicationPath.hpp"

#include <tuttle/common/system/system.hpp>
#include <tuttle/common/system/macos.hpp>
#include <tuttle/common/utils/global.hpp>

#include <boost/filesystem/operations.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/version.hpp>

#if defined( __WINDOWS__ )
#include <windows.h>
#elif defined( __MACOS__ )
#elif defined( __LINUX__ )
#endif

namespace tuttle {
namespace common {

#ifdef __WINDOWS__
#define DIRSEP "\\"
#define DIRLIST_SEP_CHARS ";"
#else
#define DIRSEP "/"
#define DIRLIST_SEP_CHARS ";:"
#endif

/*!
	Returns the absolute path to the application executable.

	On Mac OS X this will point to the directory actually containing the
	executable, which may be inside of an application bundle (if the
	application is bundled).

	\warning On Linux, this function will try to get the path from the
	\c {/proc} file system. If that fails, it assumes that \c
	{argv[0]} contains the absolute file name of the executable. The
	function also assumes that the current directory has not been
	changed by the application.

	In Symbian this function will return the application private directory,
	not the path to executable itself, as those are always in \c {/sys/bin}.
	If the application is in a read only drive, i.e. ROM, then the private path
	on the system drive will be returned.
 */
boost::filesystem::path applicationFilepath( const std::string& argv0, const boost::filesystem::path& currentPath )
{
	namespace bfs = boost::filesystem;
	namespace bal = boost::algorithm;
	bfs::path app( argv0 );
	
#if defined( __WINDOWS__ )
	{
		// We do MAX_PATH + 2 here, and request with MAX_PATH + 1, so we can handle all paths
		// up to, and including MAX_PATH size perfectly fine with string termination, as well
		// as easily detect if the file path is indeed larger than MAX_PATH, in which case we
		// need to use the heap instead. This is a work-around, since contrary to what the
		// MSDN documentation states, GetModuleFileName sometimes doesn't set the
		// ERROR_INSUFFICIENT_BUFFER error number, and we thus cannot rely on this value if
		// GetModuleFileName(0, buffer, MAX_PATH) == MAX_PATH.
		// GetModuleFileName(0, buffer, MAX_PATH + 1) == MAX_PATH just means we hit the normal
		// file path limit, and we handle it normally, if the result is MAX_PATH + 1, we use
		// heap (even if the result _might_ be exactly MAX_PATH + 1, but that's ok).
		char buffer[MAX_PATH + 2];
		DWORD v = GetModuleFileName(0, buffer, MAX_PATH + 1);
		buffer[MAX_PATH + 1] = 0;

		if( v > 0 )
		{
			if( v <= MAX_PATH )
				return std::string( buffer );
				//return QString::fromWCharArray( buffer );

			// MAX_PATH sized buffer wasn't large enough to contain the full path, use heap
			char* b = NULL;
			int i = 1;
			size_t size;
			do {
				++i;
				size = MAX_PATH * i;
				b = reinterpret_cast<char *>( realloc( b, (size + 1) * sizeof(char) ) );
				if( b )
					v = GetModuleFileName( NULL, b, size );
			} while( b && v == size );

			if( b )
				*(b + size) = 0;

			std::string res( b );
			//QString res = QString::fromWCharArray(b);
			free(b);

			return res;
		}
	}
#elif defined( __MACOS__ )
	{
		CFTypeContainer<CFURLRef> bundleURL( CFBundleCopyExecutableURL( CFBundleGetMainBundle() ) );
		if( bundleURL )
		{
			CFStringContainer cfPath( CFURLCopyFileSystemPath( bundleURL, kCFURLPOSIXPathStyle ) );
			if( cfPath )
			{
				return cfPath.str();
			}
		}
	}
#elif defined( __LINUX__ )

#if( BOOST_VERSION >= 104800 )
	{
		// Try looking for a /proc/<pid>/exe symlink first which points to
		// the absolute path of the executable
		bfs::path exeLinkPath( ( boost::format("/proc/%s/exe") % getpid() ).str() );
		if( bfs::exists( exeLinkPath ) && bfs::is_symlink( exeLinkPath ) )
		{
			return bfs::canonical( exeLinkPath ); // need boost version >= 1.48
		}
    }
#endif

#endif

	/// @todo on windows check which is the first local file or file in PATH...
	//	const bool isPath = bal::find_first( argv0, DIRSEP);
	const bool isPath = bal::contains( argv0, DIRSEP );
	if( isPath )
	{
		if( app.is_relative() )
		{
			app = currentPath / app;
		}
		if( bfs::exists( app ) && !bfs::is_directory( app ) )
		{
			return app;
		}
	}
	else
	{
		const char* envPath = std::getenv( "PATH" );
		if( envPath != NULL )
		{
			std::vector<std::string> paths;
			bal::split( paths, envPath, bal::is_any_of( DIRLIST_SEP_CHARS ) );

			BOOST_FOREACH( const std::string& pathStr, paths )
			{
				app = bfs::path( pathStr ) / argv0;
				if( bfs::exists( app ) && !bfs::is_directory( app ) )
				{
					return app;
				}
			}
		}
	}

	return argv0; // not found...
}

}
}
