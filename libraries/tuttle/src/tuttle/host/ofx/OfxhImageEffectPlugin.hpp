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

#ifndef _TUTTLE_HOST_OFX_IMAGEEFFECT_API_HPP_
#define _TUTTLE_HOST_OFX_IMAGEEFFECT_API_HPP_

#include "OfxhImageEffectHost.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhPluginHandle.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"

#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/serialize_ptr_map.hpp>

#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/scoped_ptr.hpp>

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
public:
	typedef OfxhImageEffectPlugin This;
	typedef boost::ptr_map<std::string, OfxhImageEffectNodeDescriptor> ContextMap;
	typedef std::set<std::string> ContextSet;

private:
	OfxhImageEffectPluginCache* _pc;

	/// map to store contexts in
	ContextMap _contexts;
	ContextSet _knownContexts;
	boost::scoped_ptr<OfxhPluginHandle> _pluginHandle;

	// this comes off Descriptor's property set after a describe
	// context independent
	/// @todo tuttle: ???
	boost::scoped_ptr<OfxhImageEffectNodeDescriptor> _baseDescriptor;     ///< NEEDS TO BE MADE WITH A FACTORY FUNCTION ON THE HOST!!!!!!

private:
	OfxhImageEffectPlugin();

public:
	OfxhImageEffectPlugin( OfxhImageEffectPluginCache& pc, OfxhPluginBinary& pb, int pi, OfxPlugin& pl );

	OfxhImageEffectPlugin( OfxhImageEffectPluginCache& pc,
	                       OfxhPluginBinary&           pb,
	                       int                         pi,
	                       const std::string&          api,
	                       int                         apiVersion,
	                       const std::string&          pluginId,
	                       const std::string&          rawId,
	                       int                         pluginMajorVersion,
	                       int                         pluginMinorVersion );

	~OfxhImageEffectPlugin();

	bool operator==( const OfxhImageEffectPlugin& other ) const;
	bool operator!=( const OfxhImageEffectPlugin& other ) const { return !This::operator==( other ); }

	#ifndef SWIG
	void setApiHandler( OfxhImageEffectPluginCache& api ) { _pc = &api; }
	void setApiHandler( APICache::OfxhPluginAPICacheI& api );

	/// @return the API handler this plugin was constructed by
	APICache::OfxhPluginAPICacheI&       getApiHandler();
	const APICache::OfxhPluginAPICacheI& getApiHandler() const;

	/// @brief get the base image effect descriptor
	OfxhImageEffectNodeDescriptor& getDescriptor();

	/// @brief get the base image effect descriptor, const version
	const OfxhImageEffectNodeDescriptor& getDescriptor() const;

	/// @brief get the image effect descriptor for the context
	OfxhImageEffectNodeDescriptor& getDescriptorInContext( const std::string& context );

	void addContext( const std::string& context );
	void addContext( const std::string& context, OfxhImageEffectNodeDescriptor* ied );

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
	void serialize( Archive& ar, const unsigned int version )
	{
		ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP( OfxhPlugin );
		ar& BOOST_SERIALIZATION_NVP( _baseDescriptor );
		//ar & BOOST_SERIALIZATION_NVP(_pluginHandle); // don't save this
		ar& BOOST_SERIALIZATION_NVP( _contexts );
	}

	#endif
};

}
}
}
}

#ifndef SWIG
BOOST_CLASS_EXPORT_KEY( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin )
#endif

#endif

