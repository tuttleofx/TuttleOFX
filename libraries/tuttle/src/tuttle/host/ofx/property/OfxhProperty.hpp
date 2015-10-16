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
#ifndef _TUTTLE_HOST_OFX_PROPERTY_HPP_
#define _TUTTLE_HOST_OFX_PROPERTY_HPP_

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/ofx/OfxhUtilities.hpp>
#include <tuttle/host/ofx/OfxhException.hpp>
#include <tuttle/host/exceptions.hpp>

#include <boost/type_traits/is_virtual_base_of.hpp>
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace tuttle {
namespace host {
namespace ofx {
namespace property {

// forward declarations
class OfxhSet;
class OfxhGetHook;
class OfxhNotifyHook;

/// type of a property
enum EPropType
{
	ePropTypeNone    = -1,
	ePropTypeInt     = 0,
	ePropTypeDouble  = 1,
	ePropTypeString  = 2,
	ePropTypePointer = 3
};

inline std::string mapTypeEnumToString( const EPropType& e )
{
	switch( e )
	{
		case ePropTypeNone:
			return "none";
		case ePropTypeInt:
			return "int";
		case ePropTypeDouble:
			return "double";
		case ePropTypeString:
			return "string";
		case ePropTypePointer:
			return "pointer";
	}
	BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrValue ) );
	return "";
}

enum EModifiedBy
{
	eModifiedByHost = 0,
	eModifiedByPlugin
};

/// base class for all properties
class OfxhProperty : private boost::noncopyable
{
public:
	typedef OfxhProperty This;

protected:
	std::string _name;                         ///< name of this property
	EPropType _type;                           ///< type of this property
	std::size_t _dimension;                    ///< the fixed dimension of this property
	bool _pluginReadOnly;                      ///< set is forbidden through suite: value may still change between getValue() calls
	EModifiedBy _modifiedBy;                   ///< who set this property most recently
	std::vector<OfxhNotifyHook*> _notifyHooks; ///< hooks to call whenever the property is set
	OfxhGetHook* _getHook;                     ///< if we are not storing props locally, they are stored via fetching from here

	friend class OfxhSet;

public:
	OfxhProperty( const std::string& name,
	              EPropType          type,
	              std::size_t        dimension = 1,
	              bool               pluginReadOnly = false );

	OfxhProperty( const This& other );

	virtual ~OfxhProperty() = 0;

	virtual bool operator==( const This& other ) const
	{
		if( _name != other._name )
		{
			//TUTTLE_LOG_INFO( "OfxhProperty::operator== not same name : " << _name << " != " << other._name );
			return false;
		}
		if( _type != other._type )
		{
			//TUTTLE_LOG_INFO( "OfxhProperty::operator== not same type : " << _type << " != " << other._type );
			return false;
		}
		if( _dimension != other._dimension )
		{
			//TUTTLE_LOG_INFO( "OfxhProperty::operator== not same size : " << _dimension << " != " << other._dimension );
			return false;
		}
		if( _pluginReadOnly != other._pluginReadOnly )
		{
			//TUTTLE_LOG_INFO( "OfxhProperty::operator== not sale read only : " << _pluginReadOnly << " != " << other._pluginReadOnly );
			return false;
		}
		return true;
	}

	bool operator!=( const This& other ) const { return !This::operator==( other ); }

	virtual void copyValues( const This& other ) = 0;

	/// is it read only?
	bool getPluginReadOnly() const { return _pluginReadOnly; }

	/// change the state of readonlyness
	void setPluginReadOnly( bool v ) { _pluginReadOnly = v; }

	void        setModifiedBy( const EModifiedBy who ) { _modifiedBy = who; }
	EModifiedBy getModifiedBy() const                  { return _modifiedBy; }

	/// override this to return a clone of the property
	virtual OfxhProperty* clone() const = 0;

	/// get the name of this property
	const std::string& getName() const
	{
		return _name;
	}

	/// get the type of this property
	EPropType getType() const
	{
		return _type;
	}

	/// get the type of this property as a string
	std::string getTypeStr() const
	{
		return mapTypeEnumToString(_type);
	}
#ifndef SWIG
	/// add a notify hook
	void addNotifyHook( OfxhNotifyHook* hook )
	{
		_notifyHooks.push_back( hook );
	}

	/// set the get hook
	void setGetHook( OfxhGetHook* hook )
	{
		_getHook = hook;
	}
	/// call notify on the contained notify hooks
	void notify( bool single, int indexOrN );
#endif

	// get the current dimension of this property
	virtual std::size_t getDimension() const = 0;

	/// get the fixed dimension of this property
	std::size_t getFixedDimension() const
	{
		return _dimension;
	}

	/// are we a fixed dim property
	bool isFixedSize() const
	{
		return _dimension != 0;
	}

	/// reset this property to the default
	virtual void reset() = 0;

	/// get a string representing the value of this property at element nth
	virtual std::string getStringValueAt( int index = 0 ) const = 0;

	std::vector<std::string> getStringValues() const;

	/// get a string representing all the values of this property
	std::string getStringValue() const;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive& ar, const unsigned int version )
	{
		ar& BOOST_SERIALIZATION_NVP( _name );
		ar& BOOST_SERIALIZATION_NVP( _type );
		ar& BOOST_SERIALIZATION_NVP( _dimension );
		ar& BOOST_SERIALIZATION_NVP( _pluginReadOnly );
	}

};

#ifndef SWIG
inline OfxhProperty* new_clone( const OfxhProperty& p )
{
	return p.clone();
}

#endif

}
}
}
}

#endif
