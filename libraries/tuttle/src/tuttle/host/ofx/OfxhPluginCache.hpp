/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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
#ifndef OFX_PLUGIN_CACHE_HPP
#define OFX_PLUGIN_CACHE_HPP

#include "ofxCore.h"
#include "OfxhProperty.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhBinary.hpp"

#include "expat.h"

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

#include <cstdio>


namespace tuttle {
namespace host {
namespace ofx {

class OfxhAbstractHost;

// forward delcarations
class PluginDesc;
class OfxhPlugin;
class OfxhPluginBinary;
class OfxhPluginCache;

/// C++ version of the information kept inside an OfxPlugin struct

class PluginDesc
{
	typedef PluginDesc This;
protected:
	std::string _pluginApi; ///< the API I implement
	int _apiVersion; ///< the version of the API

	std::string _identifier; ///< the identifier of the plugin
	std::string _rawIdentifier; ///< the original identifier of the plugin
	int _versionMajor; ///< the plugin major version
	int _versionMinor; ///< the plugin minor version

public:

	PluginDesc() {}

	virtual ~PluginDesc() {}

	PluginDesc( const std::string& api,
	            int                apiVersion,
	            const std::string& identifier,
	            const std::string& rawIdentifier,
	            int                versionMajor,
	            int                versionMinor )
		: _pluginApi( api ),
		_apiVersion( apiVersion ),
		_identifier( identifier ),
		_rawIdentifier( rawIdentifier ),
		_versionMajor( versionMajor ),
		_versionMinor( versionMinor ) {}

	/**
	 * constructor for the case where we have already loaded the plugin binary and
	 * are populating this object from it
	 */
	PluginDesc( OfxPlugin* ofxPlugin )
		: _pluginApi( ofxPlugin->pluginApi ),
		_apiVersion( ofxPlugin->apiVersion ),
		_identifier( ofxPlugin->pluginIdentifier ),
		_rawIdentifier( ofxPlugin->pluginIdentifier ),
		_versionMajor( ofxPlugin->pluginVersionMajor ),
		_versionMinor( ofxPlugin->pluginVersionMinor )
	{
		boost::to_lower( _identifier );
	}

	bool operator==( const This& other ) const
	{
		if( _pluginApi != other._pluginApi ||
		    _apiVersion != other._apiVersion ||
		    _identifier != other._identifier ||
		    _rawIdentifier != other._rawIdentifier ||
		    _versionMajor != other._versionMajor ||
		    _versionMinor != other._versionMinor )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }

public:
	const std::string& getPluginApi() const
	{
		return _pluginApi;
	}

	int getApiVersion() const
	{
		return _apiVersion;
	}

	const std::string& getIdentifier() const
	{
		return _identifier;
	}

	const std::string& getRawIdentifier() const
	{
		return _rawIdentifier;
	}

	int getVersionMajor() const
	{
		return _versionMajor;
	}

	int getVersionMinor() const
	{
		return _versionMinor;
	}
};

/**
 * class that we use to manipulate a plugin.
 *
 * Owned by the PluginBinary it lives inside.
 * Plugins can only be pass about either by pointer or reference.
 */
class OfxhPlugin : public PluginDesc
{
	typedef OfxhPlugin This;
	OfxhPlugin( const This& ); ///< hidden
	OfxhPlugin& operator=( const This& ); ///< hidden

protected:
	OfxhPluginBinary* _binary; ///< the file I live inside
	int _index; ///< where I live inside that file

public:
	OfxhPlugin();

	/**
	 * construct this based on the struct returned by the getNthPlugin() in the binary
	 */
	OfxhPlugin( OfxhPluginBinary* bin, int idx, OfxPlugin* o ) : PluginDesc( o ),
		_binary( bin ),
		_index( idx ) {}

	/**
	 * construct me from the cache
	 */
	OfxhPlugin( OfxhPluginBinary* bin, int idx, const std::string& api,
	            int apiVersion, const std::string& identifier,
	            const std::string& rawIdentifier,
	            int majorVersion, int minorVersion )
		: PluginDesc( api, apiVersion, identifier, rawIdentifier, majorVersion, minorVersion ),
		_binary( bin ),
		_index( idx ) {}

	virtual ~OfxhPlugin() {}

	bool operator==( const This& other ) const
	{
		if( PluginDesc::operator!=(other) ||
//		    *(_binary) != *(other._binary) ||
		    _index != other._index )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }

	OfxhPluginBinary* getBinary()
	{
		return _binary;
	}

	const OfxhPluginBinary* getBinary() const
	{
		return _binary;
	}

	int getIndex() const
	{
		return _index;
	}

	virtual APICache::OfxhPluginAPICacheI& getApiHandler() = 0;
	virtual const APICache::OfxhPluginAPICacheI& getApiHandler() const = 0;

	bool trumps( OfxhPlugin* other )
	{
		int myMajor    = getVersionMajor();
		int theirMajor = other->getVersionMajor();

		int myMinor    = getVersionMinor();
		int theirMinor = other->getVersionMinor();

		if( myMajor > theirMajor )
		{
			return true;
		}

		if( myMajor == theirMajor && myMinor > theirMinor )
		{
			return true;
		}

		return false;
	}

};

class OfxhPluginHandle;

/// class that represents a binary file which holds plugins

class OfxhPluginBinary
{
typedef OfxhPluginBinary This;
/// has a set of plugins inside it and which it owns
/// These are owned by a PluginCache
friend class OfxhPluginHandle;

protected:
	OfxhBinary _binary; ///< our binary object, abstracted layer ontop of OS calls, defined in OfxhBinary.h
	std::string _filePath; ///< full path to the file
	std::string _bundlePath; ///< path to the .bundle directory
	boost::ptr_vector<OfxhPlugin> _plugins; ///< my plugins
	time_t _fileModificationTime; ///< used as a time stamp to check modification times, used for caching
	size_t _fileSize; ///< file size last time we check, used for caching
	bool _binaryChanged; ///< whether the timestamp/filesize in this cache is different from that in the actual binary

public:
	/// create one from the cache.  this will invoke the Binary() constructor which
	/// will stat() the file.

	explicit OfxhPluginBinary( const std::string& file, const std::string& bundlePath, time_t mtime, size_t size )
		: _binary( file ),
		_filePath( file ),
		_bundlePath( bundlePath ),
		_fileModificationTime( mtime ),
		_fileSize( size ),
		_binaryChanged( false )
	{
		if( _fileModificationTime != _binary.getTime() || _fileSize != _binary.getSize() )
		{
			_binaryChanged = true;
		}
	}

	/// constructor which will open a library file, call things inside it, and then
	/// create Plugin objects as appropriate for the plugins exported therefrom

	explicit OfxhPluginBinary( const std::string& file, const std::string& bundlePath, OfxhPluginCache* cache )
		: _binary( file ),
		_filePath( file ),
		_bundlePath( bundlePath ),
		_binaryChanged( false )
	{
		loadPluginInfo( cache );
	}

	/// dtor
	virtual ~OfxhPluginBinary();


	bool operator==( const This& other ) const
	{
		if( _binary != other._binary ||
			_filePath != other._filePath ||
			_bundlePath != other._bundlePath ||
			_plugins != other._plugins ||
			_fileModificationTime != other._fileModificationTime ||
			_fileSize != other._fileSize ||
			_binaryChanged != other._binaryChanged )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }


	time_t getFileModificationTime() const
	{
		return _fileModificationTime;
	}

	size_t getFileSize() const
	{
		return _fileSize;
	}

	const std::string& getFilePath() const
	{
		return _filePath;
	}

	const std::string& getBundlePath() const
	{
		return _bundlePath;
	}

	bool hasBinaryChanged() const
	{
		return _binaryChanged;
	}

	bool isLoaded() const
	{
		return _binary.isLoaded();
	}

	void addPlugin( OfxhPlugin* pe )
	{
		_plugins.push_back( pe );
	}

	void loadPluginInfo( OfxhPluginCache* );

	/// how many plugins?
	int getNPlugins() const
	{
		return (int) _plugins.size();
	}

	/// get a plugin
	OfxhPlugin& getPlugin( int idx )
	{
		return _plugins[idx];
	}

	/// get a plugin
	const OfxhPlugin& getPlugin( int idx ) const
	{
		return _plugins[idx];
	}

};

/// wrapper class for Plugin/PluginBinary.  use in a RAIA fashion to make sure the binary gets unloaded when needed and not before.

class OfxhPluginHandle
{
OfxhPlugin* _p;
OfxhPluginBinary* _b;
OfxPlugin* _op;

public:
	OfxhPluginHandle( OfxhPlugin* p, tuttle::host::ofx::OfxhAbstractHost* _host );
	virtual ~OfxhPluginHandle();

	OfxPlugin*       getOfxPlugin()       { return _op; }
	const OfxPlugin* getOfxPlugin() const { return _op; }

	OfxPlugin* operator->() { return _op; }

};

/// for later

struct PluginCacheSupportedApi
{
	APICache::OfxhPluginAPICacheI* _handler;

	PluginCacheSupportedApi( APICache::OfxhPluginAPICacheI* handler )
		: _handler( handler ) {}

	bool matches( std::string api, int version ) const
	{
		if( api == _handler->_apiName && version >= _handler->_apiVersionMin && version <= _handler->_apiVersionMax )
		{
			return true;
		}
		return false;
	}

};

/// Where we keep our plugins.

class OfxhPluginCache
{
	typedef boost::ptr_list<OfxhPluginBinary> OfxhPluginBinaryList;
protected:
	tuttle::host::ofx::property::OfxhPropSpec* _hostSpec;

	std::list<std::string> _pluginPath; ///< list of directories to look in
	std::set<std::string> _nonrecursePath; ///< list of directories to look in (non-recursively)
	std::list<std::string> _pluginDirs; ///< list of directories we found
	OfxhPluginBinaryList _binaries; ///< all the binaries we know about, we own these
	std::list<OfxhPlugin*> _plugins; ///< all the plugins inside the binaries, we don't own these, populated from _binaries
	std::set<std::string> _knownBinFiles;

	OfxhPluginBinary* _xmlCurrentBinary;
	OfxhPlugin* _xmlCurrentPlugin;

	std::list<PluginCacheSupportedApi> _apiHandlers;

	void scanDirectory( std::set<std::string>& foundBinFiles, const std::string& dir, bool recurse );

	bool _ignoreCache;
	std::string _cacheVersion;

	bool _dirty;
	bool _enablePluginSeek; ///< Turn off to make all seekPluginFile() calls return an empty string

public:
	/// ctor, which inits _pluginPath to default locations and not much else
	OfxhPluginCache();

	/// dtor
	~OfxhPluginCache();

	/// get the list in which plugins are sought
	const std::list<std::string>& getPluginPath()
	{
		return _pluginPath;
	}

	/// was the cache outdated?
	bool dirty() const
	{
		return _dirty;
	}

	/// add a directory to the plugin path
	void addDirectoryToPath( const std::string& f, bool recurse = true )
	{
		_pluginPath.push_back( f );
		if( !recurse )
		{
			_nonrecursePath.insert( f );
		}
	}

	/// specify which subdirectory of /usr/OFX or equivilant
	/// (as well as 'Plugins') to look in for plugins.
	void setPluginHostPath( const std::string& hostId );

	/// set the version string to write to the cache,
	/// and also that we expect on cachess read in
	void setCacheVersion( const std::string& cacheVersion )
	{
		_cacheVersion = cacheVersion;
	}

	// populate the cache.  must call scanPluginFiles() after to check for changes.
	void readCache( std::istream& is );

	// seek a particular file on the OFX plugin path
	std::string seekPluginFile( const std::string& baseName ) const;

	/// Sets behaviour of seekPluginFile().
	/// Enable (the default): normal operation; disable: returns an empty string instead
	void setPluginSeekEnabled( bool enabled )
	{
		_enablePluginSeek = enabled;
	}

	/// scan for plugins
	void scanPluginFiles();

	// write the plugin cache output file to the given stream
	void writePluginCache( std::ostream& os ) const;

	// callback function for the XML
	void elementBeginCallback( void* userData, const XML_Char* name, const XML_Char** attrs );
	void elementCharCallback( void* userData, const XML_Char* data, int len );
	void elementEndCallback( void* userData, const XML_Char* name );

	/// register an API cache handler
	void registerAPICache( APICache::OfxhPluginAPICacheI& apiCache )
	{
		_apiHandlers.push_back( PluginCacheSupportedApi( &apiCache ) );
	}

	/// find the API cache handler for the given api/apiverson
	APICache::OfxhPluginAPICacheI* findApiHandler( const std::string& api, int apiver );

	/// obtain a list of plugins to walk through
	const std::list<OfxhPlugin*>& getPlugins() const
	{
		return _plugins;
	}

};

}
}
}

#endif
