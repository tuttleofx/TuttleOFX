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
#ifndef _TUTTLE_HOST_OFX_PROPERTYTEMPLATE_HPP_
#define _TUTTLE_HOST_OFX_PROPERTYTEMPLATE_HPP_

#include "OfxhProperty.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace property {

/// type holder, for integers, used to template up int properties
struct OfxhIntValue
{
	typedef int APIType; ///< C type of the property that is passed across the raw API
	typedef int APITypeConstless;  ///< C type of the property that is passed across the raw API, without any const it
	typedef int Type; ///< Type we actually hold and deal with the propery in everything by the raw API
	typedef int ReturnType; ///< type to return from a function call
	static const TypeEnum typeCode = eInt;
	static int kEmpty;
};

/// type holder, for doubles, used to template up double properties
struct OfxhDoubleValue
{
	typedef double APIType;
	typedef double APITypeConstless;
	typedef double Type;
	typedef double ReturnType; ///< type to return from a function call
	static const TypeEnum typeCode = eDouble;
	static double kEmpty;
};

/// type holder, for pointers, used to template up pointer properties
struct OfxhPointerValue
{
	typedef void* APIType;
	typedef void* APITypeConstless;
	typedef void* Type;
	typedef void* ReturnType; ///< type to return from a function call
	static const TypeEnum typeCode = ePointer;
	static void* kEmpty;
};

/// type holder, for strings, used to template up string properties
struct OfxhStringValue
{
	typedef const char* APIType;
	typedef char* APITypeConstless;
	typedef std::string Type;
	typedef const std::string& ReturnType; ///< type to return from a function call
	static const TypeEnum typeCode = eString;
	static std::string kEmpty;
};

/// this represents a generic property.
/// template parameter T is the type descriptor of the
/// type of property to model.  the class holds an internal _value vector which can be used
/// to store the values.  if set and get hooks are installed, these will be called instead
/// of using this variable.
/// Make sure that T::ReturnType is const if appropriate, as no extra qualifiers are applied here.
template<class T>
class OfxhPropertyTemplate : public OfxhProperty
{
public:
	typedef OfxhPropertyTemplate<T> This;
	typedef typename T::Type Type;
	typedef typename T::ReturnType ReturnType;
	typedef typename T::APIType APIType;
	typedef typename T::APITypeConstless APITypeConstless;

protected:
	/// this is the present value of the property
	std::vector<Type> _value;

	/// this is the default value of the property
	std::vector<Type> _defaultValue;

public:
	OfxhPropertyTemplate();

	/// constructor
	OfxhPropertyTemplate( const std::string& name,
	                      size_t             dimension,
	                      bool               pluginReadOnly,
	                      APIType            defaultValue );

	OfxhPropertyTemplate( const OfxhPropertyTemplate<T>& pt );

	OfxhPropertyTemplate<T>* clone() const
	{
		return new OfxhPropertyTemplate( *this );
	}

	virtual ~OfxhPropertyTemplate()
	{}

	bool operator==( const OfxhProperty& other ) const
	{
		if( getType() != other.getType() )
		{
			//TUTTLE_TCOUT( "OfxhPropertyTemplate::operator== not same type : " << getType() << " != " << other.getType() );
			return false;
		}
		return operator==( dynamic_cast<const This&>( other ) );
	}

	bool operator==( const This& other ) const
	{
		if( OfxhProperty::operator!=( other ) )
			return false;
		if( getType() == ePointer )
			return true; // we can't compare abstract pointer content, so assume true.
		if( _value != other._value )
		{
			//TUTTLE_TCOUT( "OfxhPropertyTemplate::operator== not same value : " );
			//for( typename std::vector<Type>::const_iterator it = _value.begin(), itEnd = _value.end(), ito = other._value.begin(), itoEnd = other._value.end();
			//	 it != itEnd && ito != itoEnd;
			//     ++it, ++ito )
			//{
			//	TUTTLE_TCOUT( *it << " != " << *ito );
			//}
			return false;
		}
		//		if( _defaultValue != other._defaultValue )
		//			return false;
		return true;
	}

	void copyValues( const OfxhProperty& other )
	{
		if( getType() != other.getType() )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
			    << exception::dev( "You try to copy a property value, but it is not the same property type." ) );
		}
		copyValues( dynamic_cast<const This&>( other ) );
	}

	void copyValues( const This& other )
	{
		if( OfxhProperty::operator!=( other ) )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
			    << exception::dev( "You try to copy a property value, but it is not the same property." ) );
		}
		_value      = other._value;
		_modifiedBy = other._modifiedBy;
		//_defaultValue = other._defaultValue;
	}

	/// get the vector
	const std::vector<Type>& getValues() const
	{
		return _value;
	}

	// get multiple values
	void getValueN( APIType* value, int count ) const OFX_EXCEPTION_SPEC;

	#if defined( WINDOWS ) && !defined( __GNUC__ )
	#pragma warning( disable : 4181 )
	#endif
	/// get one value
	ReturnType getValue( int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// get one value, without going through the getHook
	ReturnType getValueRaw( int index = 0 ) const OFX_EXCEPTION_SPEC;

	#if defined( WINDOWS ) && !defined( __GNUC__ )
	#pragma warning( default : 4181 )
	#endif
	// get multiple values, without going through the getHook
	void getValueNRaw( APIType* value, const int count ) const OFX_EXCEPTION_SPEC;

	/// set one value
	void setValue( const Type& value, const int index = 0, const EModifiedBy who = eModifiedByHost ) OFX_EXCEPTION_SPEC;

	/// set multiple values
	void setValueN( const APIType* value, const int count, const EModifiedBy who = eModifiedByHost ) OFX_EXCEPTION_SPEC;

	/// reset
	void reset() OFX_EXCEPTION_SPEC;

	/// get the size of the vector
	size_t getDimension() const OFX_EXCEPTION_SPEC;

	/// return the value as a string
	inline std::string getStringValue( int idx ) const
	{
		return boost::lexical_cast<std::string>( _value[idx] );
	}

public:
	/// @todo in private and friend function...
	ReturnType getConstlessValue( int index = 0 ) const OFX_EXCEPTION_SPEC;
	ReturnType getConstlessValueRaw( int index = 0 ) const OFX_EXCEPTION_SPEC;
	/// @todo tuttle remove ReturnType, only use Type
	inline APITypeConstless getAPIConstlessValue( int index = 0 ) const OFX_EXCEPTION_SPEC { return getConstlessValue( index ); }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive& ar, const unsigned int version )
	{
		ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP( OfxhProperty );
		ar& BOOST_SERIALIZATION_NVP( _value );
		ar& BOOST_SERIALIZATION_NVP( _defaultValue );
	}

};

typedef OfxhPropertyTemplate<OfxhIntValue>     Int;     /// Our int property
typedef OfxhPropertyTemplate<OfxhDoubleValue>  Double;  /// Our double property
typedef OfxhPropertyTemplate<OfxhStringValue>  String;  /// Our string property
typedef OfxhPropertyTemplate<OfxhPointerValue> Pointer; /// Our pointer property

template<>
inline String::APITypeConstless String::getAPIConstlessValue( int index ) const OFX_EXCEPTION_SPEC { return const_cast<String::APITypeConstless>( getConstlessValue( index ).c_str() ); }

template<>
template<class Archive>
void Pointer::serialize( Archive& ar, const unsigned int version )
{
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP( OfxhProperty );
}

}
}
}
}

#ifndef SWIG
// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost {
template<>
struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::Int>: public mpl::true_ {};
template<>
struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::Double>: public mpl::true_ {};
template<>
struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::String>: public mpl::true_ {};
template<>
struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::Pointer>: public mpl::true_ {};
}

BOOST_CLASS_EXPORT_KEY( tuttle::host::ofx::property::Int )
BOOST_CLASS_EXPORT_KEY( tuttle::host::ofx::property::Double )
BOOST_CLASS_EXPORT_KEY( tuttle::host::ofx::property::Pointer )
BOOST_CLASS_EXPORT_KEY( tuttle::host::ofx::property::String )
#endif

#endif

