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

#ifndef OFXH_IMAGE_EFFECT_API_H
#define OFXH_IMAGE_EFFECT_API_H

#include "ofxCore.h"
#include "ofxImageEffect.h"
#include "OfxhImageEffectHost.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include <string>
#include <set>
#include <memory>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

class OfxhImageEffectPluginCache;

/**
 * subclass of Plugin representing an ImageEffect plugin.
 * used to store API-specific data
 */
class OfxhImageEffectPlugin : public OfxhPlugin
{
typedef OfxhImageEffectPlugin This;
OfxhImageEffectPluginCache& _pc;

/// map to store contexts in
typedef boost::ptr_map<const std::string, OfxhImageEffectNodeDescriptor> ContextMap;
ContextMap _contexts;

typedef std::set<std::string> ContextSet;
ContextSet _knownContexts;

boost::scoped_ptr<OfxhPluginHandle> _pluginHandle;

// this comes off Descriptor's property set after a describe
// context independent
/// @todo tuttle: ???
boost::scoped_ptr<OfxhImageEffectNodeDescriptor> _baseDescriptor;     ///< NEEDS TO BE MADE WITH A FACTORY FUNCTION ON THE HOST!!!!!!

public:
	OfxhImageEffectPlugin( OfxhImageEffectPluginCache& pc, OfxhPluginBinary* pb, int pi, OfxPlugin* pl );

	OfxhImageEffectPlugin( OfxhImageEffectPluginCache& pc,
	                       OfxhPluginBinary*           pb,
	                       int                         pi,
	                       const std::string&          api,
	                       int                         apiVersion,
	                       const std::string&          pluginId,
	                       const std::string&          rawId,
	                       int                         pluginMajorVersion,
	                       int                         pluginMinorVersion );

	virtual ~OfxhImageEffectPlugin();

	bool operator==( const OfxhImageEffectPlugin& other ) const;
	bool operator!=( const OfxhImageEffectPlugin& other ) const { return !This::operator==(other); }

	/// @return the API handler this plugin was constructed by
	APICache::OfxhPluginAPICacheI& getApiHandler();
	const APICache::OfxhPluginAPICacheI& getApiHandler() const;

	/// @brief get the base image effect descriptor
	OfxhImageEffectNodeDescriptor& getDescriptor();

	/// @brief get the base image effect descriptor, const version
	const OfxhImageEffectNodeDescriptor& getDescriptor() const;

	/// @brief get the image effect descriptor for the context
	OfxhImageEffectNodeDescriptor& getDescriptorInContext( const std::string& context );

	void addContext( const std::string& context );
	void addContext( const std::string& context, OfxhImageEffectNodeDescriptor* ied );

	virtual void saveXML( std::ostream& os ) const;

	void              initContexts();
	const ContextSet& getContexts() const;
	bool              supportsContext( const std::string& context ) const;

	OfxhPluginHandle*       getPluginHandle()       { return _pluginHandle.get(); }
	const OfxhPluginHandle* getPluginHandle() const { return _pluginHandle.get(); }

	void loadAndDescribeActions();

	void unloadAction();

	/**
	 * @brief this is called to make an instance of the effect
	 *  the client data ptr is what is passed back to the client creation function
	 */
	imageEffect::OfxhImageEffectNode* createInstance( const std::string& context );

private:
	OfxhImageEffectNodeDescriptor& describeInContextAction( const std::string& context );


private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhPlugin);
//		ar & BOOST_SERIALIZATION_NVP(_); /// @todo tuttle: ???
	}
};

class OfxhMajorPlugin
{
std::string _id;
int _major;

public:
	OfxhMajorPlugin( const std::string& id, int major ) : _id( id ),
		_major( major ) {}

	OfxhMajorPlugin( OfxhImageEffectPlugin* iep ) : _id( iep->getIdentifier() ),
		_major( iep->getVersionMajor() ) {}

	const std::string& getId() const
	{
		return _id;
	}

	int getMajor() const
	{
		return _major;
	}

	bool operator<( const OfxhMajorPlugin& other ) const
	{
		if( _id < other._id )
			return true;

		if( _id > other._id )
			return false;

		if( _major < other._major )
			return true;

		return false;
	}

};

/// implementation of the specific Image Effect handler API cache.
class OfxhImageEffectPluginCache : public APICache::OfxhPluginAPICacheI
{
public:
private:
	/// all plugins
	std::vector<OfxhImageEffectPlugin*> _plugins;

	/// latest version of each plugin by ID
	std::map<std::string, OfxhImageEffectPlugin*> _pluginsByID;

	/// latest minor version of each plugin by (ID,major)
	std::map<OfxhMajorPlugin, OfxhImageEffectPlugin*> _pluginsByIDMajor;

	/// xml parsing state
	OfxhImageEffectPlugin* _currentPlugin;
	/// xml parsing state
	property::OfxhProperty* _currentProp;

	OfxhImageEffectNodeDescriptor* _currentContext;
	attribute::OfxhParamDescriptor* _currentParam;
	attribute::OfxhClipImageDescriptor* _currentClip;

	/// pointer to our image effect host
	OfxhImageEffectHost* _host;

public:
	explicit OfxhImageEffectPluginCache( OfxhImageEffectHost& host );
	virtual ~OfxhImageEffectPluginCache();

	/// get the plugin by id.  vermaj and vermin can be specified.  if they are not it will
	/// pick the highest found version.
	OfxhImageEffectPlugin* getPluginById( const std::string& id, int vermaj = -1, int vermin = -1 );

	/// get the plugin by label.  vermaj and vermin can be specified.  if they are not it will
	/// pick the highest found version.
	OfxhImageEffectPlugin* getPluginByLabel( const std::string& label, int vermaj = -1, int vermin = -1 );

	OfxhImageEffectHost* getHost()
	{
		return _host;
	}

	const std::vector<OfxhImageEffectPlugin*>& getPlugins() const;

	const std::map<std::string, OfxhImageEffectPlugin*>& getPluginsByID() const;

	const std::map<OfxhMajorPlugin, OfxhImageEffectPlugin*>& getPluginsByIDMajor() const
	{
		return _pluginsByIDMajor;
	}

	/// handle the case where the info needs filling in from the file.  runs the "describe" action on the plugin.
	void loadFromPlugin( OfxhPlugin* p ) const;

	/// handler for preparing to read in a chunk of XML from the cache, set up context to do this
	void beginXmlParsing( OfxhPlugin* p );

	/// XML handler : element begins (everything is stored in elements and attributes)
	virtual void xmlElementBegin( const std::string& el, std::map<std::string, std::string> map );

	virtual void xmlCharacterHandler( const std::string& );

	virtual void xmlElementEnd( const std::string& el );

	virtual void endXmlParsing();

	virtual void saveXML( const OfxhPlugin* const ip, std::ostream& os ) const;

	void confirmPlugin( OfxhPlugin* p );

	virtual bool pluginSupported( OfxhPlugin* p, std::string& reason ) const;

	OfxhPlugin* newPlugin( OfxhPluginBinary* pb,
	                       int               pi,
	                       OfxPlugin*        pl );

	OfxhPlugin* newPlugin( OfxhPluginBinary*  pb,
	                       int                pi,
	                       const std::string& api,
	                       int                apiVersion,
	                       const std::string& pluginId,
	                       const std::string& rawId,
	                       int                pluginMajorVersion,
	                       int                pluginMinorVersion );

	void dumpToStdOut() const;
};

}
}
}
}

#endif
