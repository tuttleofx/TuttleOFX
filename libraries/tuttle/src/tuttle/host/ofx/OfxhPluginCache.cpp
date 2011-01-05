/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// ofx host
#include "property/OfxhSet.hpp"
#include "OfxhBinary.hpp"
#include "OfxhMemory.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"
#include "OfxhUtilities.hpp"

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

// Define this to enable ofx plugin cache debug messages.
//#define CACHE_DEBUG

#if defined ( __linux__ )

 #define DIRLIST_SEP_CHARS ":;"
 #define DIRSEP "/"
#include <dirent.h>

 #define ARCHSTR getArchStr()

#elif defined ( __APPLE__ )

 #define DIRLIST_SEP_CHARS ";:"
 #define ARCHSTR "MacOS"
 #define DIRSEP "/"
#include <dirent.h>

#elif defined ( WINDOWS )
 #define DIRLIST_SEP_CHARS ";"
#ifdef _WIN64
  #define ARCHSTR "win64"
#else
  #define ARCHSTR "win32"
#endif
 #define DIRSEP "\\"

// CINTERFACE needs to be declared if compiling with VC++
#include <shlobj.h>
#include <tchar.h>
#ifndef _MSC_VER
  #define SHGFP_TYPE_CURRENT 0
#endif

#endif

namespace tuttle {
namespace host {
namespace ofx {

#if defined ( __linux__ )

static const char* getArchStr()
{
	if( sizeof( void* ) == 4 )
	{
		return "Linux-x86";
	}
	else
	{
		return "Linux-x86-64";
	}
}

#endif
#if defined ( WINDOWS )

const TCHAR* getStdOFXPluginPath( const std::string& hostId = "Plugins" )
{
	static TCHAR buffer[MAX_PATH];
	static int gotIt = 0;

	if( !gotIt )
	{
		gotIt = 1;
		SHGetFolderPath( NULL, CSIDL_PROGRAM_FILES_COMMON, NULL, SHGFP_TYPE_CURRENT, buffer );
		strncat( buffer, "\\OFX\\Plugins", MAX_PATH );
	}
	return buffer;
}

#endif

std::string OFXGetEnv( const char* e )
{
	#if !defined( __GNUC__ ) && defined( WINDOWS )
	size_t requiredSize;
	getenv_s( &requiredSize, 0, 0, e );
	std::vector<char> buffer( requiredSize );
	if( requiredSize > 0 )
	{
		getenv_s( &requiredSize, &buffer.front(), requiredSize, e );
		return &buffer.front();
	}
	return "";
	#else
	if( getenv( e ) )
		return getenv( e );
	#endif
	return "";
}

OfxhPluginCache::OfxhPluginCache()
	: _ignoreCache( false )
	, _cacheVersion( "" )
	, _dirty( false )
	, _enablePluginSeek( true )
{
	std::string s = OFXGetEnv( "OFX_PLUGIN_PATH" );

	while( s.length() )
	{
		int spos = int(s.find_first_of( DIRLIST_SEP_CHARS ) );

		std::string path;

		if( spos != -1 )
		{
			path = s.substr( 0, spos );
			s    = s.substr( spos + 1 );
		}
		else
		{
			path = s;
			s    = "";
		}

		_pluginPath.push_back( path );
	}

	#if defined( WINDOWS )
	_pluginPath.push_back( getStdOFXPluginPath() );
	_pluginPath.push_back( "C:\\Program Files\\Common Files\\OFX\\Plugins" );
	#endif
	#if defined( __linux__ )
	_pluginPath.push_back( "/usr/OFX/Plugins" );
	#endif
	#if defined( __APPLE__ )
	_pluginPath.push_back( "/Library/OFX/Plugins" );
	#endif
}

OfxhPluginCache::~OfxhPluginCache()
{}

void OfxhPluginCache::setPluginHostPath( const std::string& hostId )
{
	#if defined( WINDOWS )
	_pluginPath.push_back( getStdOFXPluginPath( hostId ) );
	_pluginPath.push_back( "C:\\Program Files\\Common Files\\OFX\\" + hostId );
	#endif
	#if defined( __linux__ )
	_pluginPath.push_back( "/usr/OFX/" + hostId );
	#endif
	#if defined( __APPLE__ )
	_pluginPath.push_back( "/Library/OFX/" + hostId );
	#endif
}

void OfxhPluginCache::scanDirectory( std::set<std::string>& foundBinFiles, const std::string& dir, bool recurse )
{
	#ifdef CACHE_DEBUG
	TUTTLE_TCOUT( "looking in " << dir << " for plugins" );
	#endif

	#if defined ( WINDOWS )
	WIN32_FIND_DATA findData;
	HANDLE findHandle;
	#else
	DIR* d = opendir( dir.c_str() );
	if( !d )
	{
		return;
	}
	#endif

	_pluginDirs.push_back( dir.c_str() );

	#if defined ( UNIX )
	while( dirent * de = readdir( d ) )
	#elif defined ( WINDOWS )
	findHandle = FindFirstFile( ( dir + "\\*" ).c_str(), &findData );

	if( findHandle == INVALID_HANDLE_VALUE )
	{
		return;
	}

	while( 1 )
	#endif
	{
		#if defined ( UNIX )
		std::string name = de->d_name;
		bool isdir       = true;
		#else
		std::string name = findData.cFileName;
		bool isdir       = ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
		#endif
		if( name.find( ".ofx.bundle" ) != std::string::npos )
		{
			std::string barename   = name.substr( 0, name.length() - strlen( ".bundle" ) );
			std::string bundlename = dir + DIRSEP + name;
			std::string binpath    = dir + DIRSEP + name + DIRSEP "Contents" DIRSEP + ARCHSTR + DIRSEP + barename;

			foundBinFiles.insert( binpath );

			if( _knownBinFiles.find( binpath ) == _knownBinFiles.end() )
			{
				#ifdef CACHE_DEBUG
				TUTTLE_TCOUT( "found non-cached binary " << binpath );
				#endif
				setDirty();
				try
				{
					// the binary was not in the cache
					OfxhPluginBinary* pb = new OfxhPluginBinary( binpath, bundlename, this );
					_binaries.push_back( pb );
					_knownBinFiles.insert( binpath );

					for( int j = 0; j < pb->getNPlugins(); ++j )
					{
						OfxhPlugin& plug                   = pb->getPlugin( j );
						APICache::OfxhPluginAPICacheI& api = plug.getApiHandler();
						api.loadFromPlugin( plug );
					}
				}
				catch(... )
				{
					TUTTLE_COUT_CURRENT_EXCEPTION;
				}
			}
			else
			{
				#ifdef CACHE_DEBUG
				TUTTLE_TCOUT( "found cached binary " << binpath );
				#endif
			}
		}
		else
		{
			if( isdir && ( recurse && name[0] != '@' && name != "." && name != ".." ) )
			{
				scanDirectory( foundBinFiles, dir + DIRSEP + name, recurse );
			}
		}
		#if defined( WINDOWS )
		int rval = FindNextFile( findHandle, &findData );

		if( rval == 0 )
		{
			break;
		}
		#endif
	}

	#if defined( UNIX )
	closedir( d );
	#else
	FindClose( findHandle );
	#endif
}

std::string OfxhPluginCache::seekPluginFile( const std::string& baseName ) const
{
	// Exit early if disabled
	if( !_enablePluginSeek )
		return "";

	for( std::list<std::string>::const_iterator paths = _pluginDirs.begin();
	     paths != _pluginDirs.end();
	     ++paths )
	{
		std::string candidate = *paths + DIRSEP + baseName;
		FILE* f               = fopen( candidate.c_str(), "r" );
		if( f )
		{
			fclose( f );
			return candidate;
		}
	}
	return "";
}

void OfxhPluginCache::scanPluginFiles()
{
	std::set<std::string> foundBinFiles;

	for( std::list<std::string>::iterator paths = _pluginPath.begin();
	     paths != _pluginPath.end();
	     ++paths )
	{
		scanDirectory( foundBinFiles, *paths, _nonrecursePath.find( *paths ) == _nonrecursePath.end() );
	}

	OfxhPluginBinaryList::iterator i = _binaries.begin();
	while( i != _binaries.end() )
	{
		if( foundBinFiles.find( i->getFilePath() ) == foundBinFiles.end() )
		{
			// the binary was in the cache, but was not on the path
			setDirty();
			i = _binaries.erase( i );
		}
		else
		{
			bool binChanged = i->hasBinaryChanged();

			// the binary was in the cache, but the binary has changed and thus we need to reload
			if( binChanged )
			{
				i->loadPluginInfo( this );
				setDirty();
			}

			for( int j = 0; j < i->getNPlugins(); ++j )
			{
				OfxhPlugin& plug                   = i->getPlugin( j );
				APICache::OfxhPluginAPICacheI& api = plug.getApiHandler();

				if( binChanged )
				{
					api.loadFromPlugin( plug );
				}

				std::string reason;

				if( api.pluginSupported( plug, reason ) )
				{
					addPlugin( &plug );
					api.confirmPlugin( plug );
				}
				else
				{
					TUTTLE_COUT_ERROR( "Ignoring plugin " << plug.getIdentifier() <<
					            " as unsupported (" << reason << ")" );
				}
			}

			++i;
		}
	}
}

APICache::OfxhPluginAPICacheI* OfxhPluginCache::findApiHandler( const std::string& api, int version )
{
	std::list<PluginCacheSupportedApi>::iterator i = _apiHandlers.begin();
	while( i != _apiHandlers.end() )
	{
		if( i->matches( api, version ) )
		{
			return i->_handler;
		}
		++i;
	}
	return 0;
}

/**
 * get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
 * pick the highest found version.
 */
OfxhPlugin* OfxhPluginCache::getPluginById( const std::string& id, int vermaj, int vermin )
{
	if( vermaj == -1 &&  vermin == -1 )
		return _pluginsByID[id];

	// return the highest version one, which fits the pattern provided
	OfxhPlugin* sofar = 0;

	for( std::list<OfxhPlugin*>::iterator i = _plugins.begin(); i != _plugins.end(); ++i )
	{
		OfxhPlugin* p = *i;

		if( p->getIdentifier() != id )
		{
			continue;
		}

		if( vermaj != -1 && p->getVersionMajor() != vermaj )
		{
			continue;
		}

		if( vermin != -1 && p->getVersionMinor() != vermin )
		{
			continue;
		}

		if( !sofar || p->trumps( *sofar ) )
		{
			sofar = p;
		}
	}
	return sofar;
}

std::ostream& operator<<( std::ostream& os, const OfxhPluginCache& v )
{
	os << "OfxhPluginCache {" << std::endl;

	if( v._pluginsByID.empty() )
		os << "No Plug-ins Found." << std::endl;

	os << "________________________________________________________________________________" << std::endl;
	for( std::map<std::string, OfxhPlugin*>::const_iterator it = v._pluginsByID.begin(); it != v._pluginsByID.end(); ++it )
	{
		os << "Plug-in:" << it->first << std::endl;
		os << "  " << "Filepath: " << it->second->getBinary().getFilePath();
		os << "(" << it->second->getIndex() << ")" << std::endl;

		//		os << "Contexts:" << std::endl;
		//		const std::set<std::string>& contexts = it->second->getContexts();
		//		for( std::set<std::string>::const_iterator it2 = contexts.begin(); it2 != contexts.end(); ++it2 )
		//			os << "  * " << *it2 << std::endl;
		//		const OfxhImageEffectNodeDescriptor& d = it->second->getDescriptor();
		//		os << "Inputs:" << std::endl;
		//		const std::map<std::string, attribute::OfxhClipImageDescriptor*>& inputs = d.getClips();
		//		for( std::map<std::string, attribute::OfxhClipImageDescriptor*>::const_iterator it2 = inputs.begin(); it2 != inputs.end(); ++it2 )
		//			os << "    * " << it2->first << std::endl;
		os << "________________________________________________________________________________" << std::endl;
	}
	os << "}" << std::endl;
	return os;
}

}
}
}


