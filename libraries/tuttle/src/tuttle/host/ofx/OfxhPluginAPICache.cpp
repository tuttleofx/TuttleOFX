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

#include <cassert>

#include <string>
#include <map>

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhProperty.hpp"
#include "OfxhClip.hpp"
#include "OfxhParam.hpp"
#include "OfxhMemory.hpp"
#include "OfxhImageEffectNode.hpp"
#include "OfxhPluginAPICache.hpp"
#include "OfxhPluginCache.hpp"
#include "OfxhHost.hpp"
#include "OfxhImageEffectPlugin.hpp"
#include "OfxhXml.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace APICache {
void propertySetXMLRead( const std::string& el,
                         std::map<std::string, std::string> map,
                         property::OfxhSet& set,
                         property::OfxhProperty*& currentProp )
{
	if( el == "property" )
	{
		std::string propName = map["name"];
		std::string propType = map["type"];
		int dimension        = atoi( map["dimension"].c_str() );

		if( set.hasProperty( propName, false ) )
		{
			currentProp = &set.fetchLocalProperty( propName );
		}
		else
		{
			if( propType == "int" )
			{
				currentProp = new property::Int( propName, dimension, false, 0 );
			}
			else if( propType == "string" )
			{
				currentProp = new property::String( propName, dimension, false, "" );
			}
			else if( propType == "double" )
			{
				currentProp = new property::Double( propName, dimension, false, 0 );
			}
			else if( propType == "pointer" )
			{
				currentProp = new property::Pointer( propName, dimension, false, 0 );
			}
			set.addProperty( currentProp );
		}
		return;
	}

	if( el == "value" && currentProp )
	{
		int index         = atoi( map["index"].c_str() );
		std::string value = map["value"];

		switch( currentProp->getType() )
		{
			case property::eInt:
				set.setIntProperty( currentProp->getName(), atoi( value.c_str() ), index );
				break;
			case property::eString:
				set.setStringProperty( currentProp->getName(), value, index );
				break;
			case property::eDouble:
				set.setDoubleProperty( currentProp->getName(), atof( value.c_str() ), index );
				break;
			case property::ePointer:
				break;
			default:
				break;
		}

		return;
	}

	std::cout << "got unrecognised key " << el << "\n";

	assert( false );
}

void propertyXMLWrite( std::ostream& o, const property::OfxhProperty& prop, const std::string& indent = "" )
{
	if( prop.getType() != property::ePointer )
	{

		o << indent << "<property "
		  << XML::attribute( "name", prop.getName() )
		  << XML::attribute( "type", property::mapTypeEnumToString( prop.getType() ) )
		  << XML::attribute( "dimension", prop.getFixedDimension() )
		  << ">\n";

		for( size_t i = 0; i < prop.getDimension(); i++ )
		{
			o << indent << "  <value "
			  << XML::attribute( "index", i )
			  << XML::attribute( "value", prop.getStringValue( i ) )
			  << "/>\n";
		}

		o << indent << "</property>\n";
	}
}

void propertyXMLWrite( std::ostream& o, const property::OfxhSet& set, const std::string& name, int indent )
{
	if( set.hasProperty( name ) )
	{
		const property::OfxhProperty& prop = set.fetchLocalProperty( name );
		std::string indent_prefix( indent, ' ' );
		propertyXMLWrite( o, prop, indent_prefix );
	}
}

void propertySetXMLWrite( std::ostream& o, const property::OfxhSet& set, int indent )
{
	std::string indent_prefix( indent, ' ' );

	for( property::PropertyMap::const_iterator i = set.getMap().begin();
	     i != set.getMap().end();
	     i++ )
	{
		const property::OfxhProperty& prop = *( i->second );
		propertyXMLWrite( o, prop, indent_prefix );
	}
}

}
}
}
}
