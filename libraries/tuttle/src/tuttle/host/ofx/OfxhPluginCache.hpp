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
#ifndef _TUTTLE_HOST_OFX_PLUGINCACHE_HPP_
#define _TUTTLE_HOST_OFX_PLUGINCACHE_HPP_

#include "property/OfxhSet.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhPluginBinary.hpp"

#include <ofxCore.h>

#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/ptr_container/serialize_ptr_list.hpp>

#include <string>
#include <set>
#include <algorithm>
#include <iostream>

namespace tuttle {
namespace host {
namespace ofx {

/**
 * for later
 */
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

/**
 * Where we keep our plugins.
 */
class OfxhPluginCache
{
public:
	typedef OfxhPluginCache This;
	typedef boost::ptr_list<OfxhPluginBinary> OfxhPluginBinaryList;

protected:
	tuttle::host::ofx::property::OfxhPropSpec* _hostSpec;

	std::list<std::string> _pluginPath; ///< list of directories to look in
	std::set<std::string> _nonrecursePath; ///< list of directories to look in (non-recursively)
	std::list<std::string> _pluginDirs; ///< list of directories we found
	OfxhPluginBinaryList _binaries; ///< all the binaries we know about, we own these
	std::list<OfxhPlugin*> _plugins; ///< all the plugins inside the binaries, we don't own these, populated from _binaries
	std::map<std::string, OfxhPlugin*> _pluginsByID;
	std::map<OfxhPluginIdent, bool> _loadedMap; ///< Used to check if a plugin is loaded twice
	std::set<std::string> _knownBinFiles;

	std::list<PluginCacheSupportedApi> _apiHandlers;

	// internal state
	bool _ignoreCache;
	std::string _cacheVersion;
	bool _dirty;
	bool _enablePluginSeek; ///< Turn off to make all seekPluginFile() calls return an empty string

public:
	/// ctor, which inits _pluginPath to default locations and not much else
	OfxhPluginCache();

	/// dtor
	~OfxhPluginCache();

protected:
	void scanDirectory( std::set<std::string>& foundBinFiles, const std::string& dir, bool recurse );

	void addPlugin( OfxhPlugin* plugin )
	{
		// Check if the same plugin has already been loaded
		if( _loadedMap.find( plugin->getIdentity() ) == _loadedMap.end() )
		{
			_loadedMap[plugin->getIdentity()] = true;
		}
		else
		{
			TUTTLE_COUT_WARNING( "Warning! Plugin: "
			              << plugin->getRawIdentifier()
			              << " loaded twice!"
				          << " (" << plugin->getBinary().getFilePath() << ")" );
		}
		_plugins.push_back( plugin );

		if( _pluginsByID.find( plugin->getIdentifier() ) != _pluginsByID.end() )
		{
			OfxhPlugin& otherPlugin = *_pluginsByID[plugin->getIdentifier()];
			if( plugin->trumps( otherPlugin ) )
			{
				_pluginsByID[plugin->getIdentifier()] = plugin;
			}
		}
		else
		{
			_pluginsByID[plugin->getIdentifier()] = plugin;
		}
	}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& g );

	#ifdef SWIG
	%extend
	{
		OfxhPlugin& __getitem__( const std::string& name )
		{
			return *self->getPluginById( name );
		}

		std::string __str__() const
		{
			std::stringstream s;

			s << *self;
			return s.str();
		}

	}
	#endif

	/// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
	/// pick the highest found version.
	OfxhPlugin*       getPluginById( const std::string& id, int vermaj = -1, int vermin = -1 );
	const OfxhPlugin* getPluginById( const std::string& id, int vermaj = -1, int vermin = -1 ) const { return const_cast<This&>( *this ).getPluginById( id, vermaj, vermin ); }

	/// get the list in which plugins are sought
	const std::list<std::string>& getPluginPath()
	{
		return _pluginPath;
	}

	/// was the cache outdated?
	bool isDirty() const
	{
		return _dirty;
	}

	void setDirty()
	{
		//TUTTLE_TCOUT("OfxhPluginCache::setDirty()");
		_dirty = true;
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
	//void readCache( std::istream& is );

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

	OfxhPluginBinaryList& getBinaries()
	{
		return _binaries;
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive& ar, const unsigned int version )
	{
		//		ar & BOOST_SERIALIZATION_NVP(_pluginPath);
		//		ar & BOOST_SERIALIZATION_NVP(_nonrecursePath);
		//		ar & BOOST_SERIALIZATION_NVP(_pluginDirs);
		ar& BOOST_SERIALIZATION_NVP( _binaries );
		//		ar & BOOST_SERIALIZATION_NVP(_plugins); // just a link, don't save this
		ar& BOOST_SERIALIZATION_NVP( _knownBinFiles );

		if( typename Archive::is_loading() )
		{
			for( ofx::OfxhPluginCache::OfxhPluginBinaryList::iterator it = getBinaries().begin(), itEnd = getBinaries().end();
			     it != itEnd;
			     ++it )
			{
				for( ofx::OfxhPluginBinary::PluginVector::iterator i = it->getPlugins().begin(), iEnd = it->getPlugins().end();
				     i != iEnd;
				     ++i )
				{
					APICache::OfxhPluginAPICacheI* apiCache = findApiHandler( i->getPluginApi(), i->getApiVersion() );
					i->setApiHandler( *apiCache );
					_plugins.push_back( &( *i ) );
				}
			}
		}
	}

};

}
}
}

#endif
