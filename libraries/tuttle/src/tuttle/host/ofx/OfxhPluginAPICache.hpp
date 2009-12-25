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
#ifndef OFX_PLUGIN_API_CACHE
#define OFX_PLUGIN_API_CACHE

#include <string>
#include <iostream>
#include <ostream>
#include <map>

#include "OfxhProperty.hpp"

namespace tuttle {
namespace host {
namespace ofx {
class OfxhPlugin;
class OfxhPluginBinary;
class OfxhPluginCache;

namespace imageEffect {
class ImageEffectDescriptor;
}
}
}
}

namespace tuttle {
namespace host {
namespace ofx {
namespace APICache {

/// this acts as an interface for the Plugin Cache, handling api-specific cacheing
class OfxhPluginAPICacheI
{
public:
	std::string _apiName;
	int _apiVersionMin, _apiVersionMax;

public:
	OfxhPluginAPICacheI( std::string apiName, int verMin, int verMax )
		: _apiName( apiName ),
		_apiVersionMin( verMin ),
		_apiVersionMax( verMax )
	{}

	virtual ~OfxhPluginAPICacheI() {}

	virtual void loadFromPlugin( OfxhPlugin* ) const = 0;

	/// factory method, to create a new plugin (from binary)
	virtual OfxhPlugin* newPlugin( OfxhPluginBinary*, int pi, OfxPlugin* plug ) = 0;

	/// factory method, to create a new plugin (from the
	virtual OfxhPlugin* newPlugin( OfxhPluginBinary* pb, int pi, const std::string& api, int apiVersion, const std::string& pluginId,
	                               const std::string& rawId, int pluginMajorVersion, int pluginMinorVersion ) = 0;

	virtual void beginXmlParsing( OfxhPlugin* )                                           = 0;
	virtual void xmlElementBegin( const std::string&, std::map<std::string, std::string>) = 0;
	virtual void xmlCharacterHandler( const std::string& )                                = 0;
	virtual void xmlElementEnd( const std::string& )                                      = 0;
	virtual void endXmlParsing()                                                          = 0;

	virtual void saveXML( OfxhPlugin*, std::ostream& ) const = 0;

	virtual void confirmPlugin( OfxhPlugin* ) = 0;

	virtual bool pluginSupported( OfxhPlugin*, std::string& reason ) const = 0;
};

/// helper function to build a property set from XML. Really should be a member of the property set!!!
void propertySetXMLRead( const std::string& el, std::map<std::string, std::string> map, property::OfxhSet& set, property::OfxhProperty*& );

/// helper function to write a property set to XML. Really should be a member of the property set!!!
void propertySetXMLWrite( std::ostream& o, const property::OfxhSet& set, int indent = 0 );

/// helper function to write a single property from a set to XML. Really should be a member of the property set!!!
void propertyXMLWrite( std::ostream& o, const property::OfxhSet& set, const std::string& name, int indent = 0 );

}
}
}
}

#endif

