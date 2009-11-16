/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
	  contributors may be used to endorse or promote products derived from this
	  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OFXH_ATTRIBUTE_H
#define OFXH_ATTRIBUTE_H

#include "ofxhPropertySuite.h"
#include "ofxAttribute.h"
#include "ofxCore.h"

#include <string>
#include <cassert>

namespace OFX {
namespace Host {
namespace Attribute {

class AttributeAccessor
{
public:
	AttributeAccessor( );
	//	explicit Base( const Base& ); // auto
	virtual ~AttributeAccessor( ) = 0;

protected:
	virtual void setProperties( const Property::Set& properties ) = 0;
public:
	virtual const Property::Set& getProperties( ) const = 0;
	virtual Property::Set& getEditableProperties( ) = 0;
public:
	/// is the clip an output clip

	bool isOutput( ) const
	{
		return getName( ) == kOfxOutputAttributeName;
	}

	//					{ kOfxPropName, Property::eString, 1, false, cname },
	//					{ kOfxPropLabel, Property::eString, 1, false, cname },
	//					{ kOfxPropShortLabel, Property::eString, 1, false, cname },
	//					{ kOfxPropLongLabel, Property::eString, 1, false, cname },

	void setAllNames( const std::string&name )
	{
		setName( name );
		setLabel( name );
		setShortLabel( name );
		setLongLabel( name );
	}

	const std::string &getAttributeType( ) const
	{
		return getProperties( ).getStringProperty( kOfxPropType );
	}

	const std::string &getName( ) const
	{
		return getProperties( ).getStringProperty( kOfxPropName );
	}
	void setName( const std::string & name )
	{
		return getEditableProperties().setStringProperty( kOfxPropName, name );
	}

	/** name of the clip
	 */
	const std::string &getShortLabel( ) const
	{
		const std::string &s = getProperties( ).getStringProperty( kOfxPropShortLabel );
		if( s == "" )
		{
			return getLabel( );
		}
		return s;
	}
	void setShortLabel( const std::string &label )
	{
		return getEditableProperties().setStringProperty( kOfxPropShortLabel, label );
	}

	/** name of the clip
	 */
	const std::string &getLabel( ) const
	{
		const std::string &s = getProperties( ).getStringProperty( kOfxPropLabel );
		if( s == "" )
		{
			return getName( );
		}
		return s;
	}
	void setLabel( const std::string &label )
	{
		return getEditableProperties().setStringProperty( kOfxPropLabel, label );
	}

	/** name of the clip
	 */
	const std::string &getLongLabel( ) const
	{
		const std::string &s = getProperties( ).getStringProperty( kOfxPropLongLabel );
		if( s == "" )
		{
			return getLabel( );
		}
		return s;
	}
	void setLongLabel( const std::string &label )
	{
		return getEditableProperties().setStringProperty( kOfxPropLongLabel, label );
	}

	const Property::Set& getProps( ) const
	{
		return getProperties( );
	}

	/** get a handle on the properties of the clip descriptor for the C api
	 */
	OfxPropertySetHandle getPropHandle( ) const
	{
		return getProperties( ).getHandle( );
	}


};

class AttributeDescriptor : virtual public AttributeAccessor
{
public:
	AttributeDescriptor( const Property::Set& properties );
	virtual ~AttributeDescriptor( ) = 0;
protected:
	Property::Set _properties;

protected:

	void setProperties( const Property::Set& properties )
	{
		_properties = properties;
		assert( getAttributeType( ).c_str( ) );
	}
public:

	const Property::Set& getProperties( ) const
	{
		return _properties;
	}

	Property::Set& getEditableProperties( )
	{
		return _properties;
	}
};

class AttributeInstance : virtual public AttributeAccessor
{
public:
	AttributeInstance( const Property::Set& properties );
	AttributeInstance( const AttributeDescriptor& desc );
	virtual ~AttributeInstance( ) = 0;
protected:
	Property::Set _properties;

protected:

	void setProperties( const Property::Set& properties )
	{
		_properties = properties;
		assert( getAttributeType( ).c_str( ) );
	}
public:

	const Property::Set& getProperties( ) const
	{
		return _properties;
	}

	Property::Set& getEditableProperties( )
	{
		return _properties;
	}
};

}
}
}

#endif
