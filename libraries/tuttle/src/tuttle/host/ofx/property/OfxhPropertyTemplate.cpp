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

#include "OfxhPropertyTemplate.hpp"
#include "OfxhGetHook.hpp"
#include "OfxhNotifyHook.hpp"

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/serialization.hpp>

#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream>
#include <cstring>

//#define DEBUG_PROPERTIES true

namespace tuttle {
namespace host {
namespace ofx {
namespace property {

/// type holder, for integers
int OfxhIntValue::kEmpty       = 0;
double OfxhDoubleValue::kEmpty = 0;
void* OfxhPointerValue::kEmpty = 0;
std::string OfxhStringValue::kEmpty;

inline int castToAPIType( int i )
{
	return i;
}

inline void* castToAPIType( void* v )
{
	return v;
}

inline double castToAPIType( double d )
{
	return d;
}

inline const char* castToAPIType( const std::string& s )
{
	return s.c_str();
}

template<class T>
OfxhPropertyTemplate<T>::OfxhPropertyTemplate()
	: OfxhProperty( "", T::typeCode, 0, false )
{}

template<class T>
OfxhPropertyTemplate<T>::OfxhPropertyTemplate( const std::string& name,
                                               size_t             dimension,
                                               bool               pluginReadOnly,
                                               APIType            defaultValue )
	: OfxhProperty( name, T::typeCode, dimension, pluginReadOnly )
{
	if( dimension != _value.size() )
	{
		_value.resize( dimension );
		_defaultValue.resize( dimension );
	}

	if( dimension )
	{
		for( size_t i = 0; i < dimension; ++i )
		{
			_defaultValue[i] = defaultValue;
			_value[i]        = defaultValue;
		}
	}
}

template<class T>
OfxhPropertyTemplate<T>::OfxhPropertyTemplate( const OfxhPropertyTemplate<T>& pt )
	: OfxhProperty( pt )
	, _value( pt._value )
	, _defaultValue( pt._defaultValue )
{}

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( disable : 4181 )
#endif
/// get one value

template<class T>
typename T::ReturnType OfxhPropertyTemplate<T>::getValue( int index ) const OFX_EXCEPTION_SPEC
{
	if( _getHook )
	{
		return _getHook->getProperty<T >( _name, index );
	}
	else
	{
		return getValueRaw( index );
	}
}

template<class T>
typename T::ReturnType OfxhPropertyTemplate<T>::getConstlessValue( int index ) const OFX_EXCEPTION_SPEC
{
	if( _getHook )
	{
		return _getHook->getProperty<T >( _name, index );
	}
	else
	{
		return getConstlessValueRaw( index );
	}
}

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( default : 4181 )
#endif
// get multiple values

template<class T>
void OfxhPropertyTemplate<T>::getValueN( typename T::APIType* values, int count ) const OFX_EXCEPTION_SPEC
{
	if( _getHook )
	{
		_getHook->getPropertyN<T >( _name, values, count );
	}
	else
	{
		getValueNRaw( values, count );
	}
}

// get multiple values

template<>
void OfxhPropertyTemplate<OfxhStringValue>::getValueN( OfxhStringValue::APIType* values, int count ) const OFX_EXCEPTION_SPEC
{
	if( _getHook )
	{
		for( int i = 0; i < count; ++i )
		{
			values[i] = castToAPIType( _getHook->getStringProperty( _name, i ) );
		}
	}
	else
	{
		getValueNRaw( values, count );
	}
}

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( disable : 4181 )
#endif

/**
 * get one value, without going through the getHook
 */
template<class T>
typename T::ReturnType OfxhPropertyTemplate<T>::getValueRaw( int index ) const OFX_EXCEPTION_SPEC
{
	if( index < 0 || ( (size_t) index >= _value.size() ) )
	{
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrBadIndex ) );
	}
	return _value[index];
}

template<class T>
typename T::ReturnType OfxhPropertyTemplate<T>::getConstlessValueRaw( int index ) const OFX_EXCEPTION_SPEC
{
	if( index < 0 || ( (size_t) index >= _value.size() ) )
	{
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrBadIndex ) );
	}
	return _value[index];
}

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( default : 4181 )
#endif

/**
 * get multiple values, without going through the getHook
 */
template<class T>
void OfxhPropertyTemplate<T>::getValueNRaw( APIType* value, const int count ) const OFX_EXCEPTION_SPEC
{
	size_t size = count;

	if( size > _value.size() )
	{
		size = _value.size();
	}

	for( size_t i = 0; i < size; i++ )
	{
		value[i] = castToAPIType( _value[i] );
	}
}

/**
 * set one value
 */
template<class T>
void OfxhPropertyTemplate<T>::setValue( const typename T::Type& value, const int index, const EModifiedBy who ) OFX_EXCEPTION_SPEC
{
	if( index < 0 || ( (size_t) index > _value.size() && _dimension ) )
	{
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrBadIndex ) );
	}

	if( _value.size() <= (size_t) index )
	{
		_value.resize( index + 1 );
	}
	_value[index] = value;

	//	TCOUT( "setValue: " << value << " at index: " << index );

	_modifiedBy = who;

	notify( true, index );
}

/**
 * set multiple values
 */
template<class T>
void OfxhPropertyTemplate<T>::setValueN( const typename T::APIType* value, const int count, const EModifiedBy who ) OFX_EXCEPTION_SPEC
{
	if( _dimension && ( (size_t) count > _value.size() ) )
	{
		BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrBadIndex ) );
	}
	if( _value.size() != (size_t) count )
	{
		_value.resize( count );
	}
	for( int i = 0; i < count; ++i )
	{
		_value[i] = value[i];
	}

	_modifiedBy = who;

	notify( false, count );
}

/**
 * get the dimension of the property
 */
template <class T>
size_t OfxhPropertyTemplate<T>::getDimension() const OFX_EXCEPTION_SPEC
{
	if( _dimension != 0 )
	{
		return _dimension;
	}
	else
	{
		// code to get it from the hook
		if( _getHook )
		{
			return _getHook->getDimension( _name );
		}
		else
		{
			return (int) _value.size();
		}
	}
}

template <class T>
void OfxhPropertyTemplate<T>::reset() OFX_EXCEPTION_SPEC
{
	if( _getHook )
	{
		_getHook->reset( _name );
		int dim = getDimension();

		if( !isFixedSize() )
		{
			_value.resize( dim );
		}
		for( int i = 0; i < dim; ++i )
		{
			_value[i] = _getHook->getProperty<T >( _name, i );
		}
	}
	else
	{
		if( isFixedSize() )
		{
			_value = _defaultValue;
		}
		else
		{
			_value.resize( 0 );
		}

		// now notify on a reset
		notify( false, _dimension );
	}
	_modifiedBy = eModifiedByHost;
}

// only compile this templates list
template class OfxhPropertyTemplate<OfxhIntValue>;
template class OfxhPropertyTemplate<OfxhDoubleValue>;
template class OfxhPropertyTemplate<OfxhStringValue>;
template class OfxhPropertyTemplate<OfxhPointerValue>;

}
}
}
}

BOOST_CLASS_EXPORT_IMPLEMENT( tuttle::host::ofx::property::Int )
BOOST_CLASS_EXPORT_IMPLEMENT( tuttle::host::ofx::property::Double )
BOOST_CLASS_EXPORT_IMPLEMENT( tuttle::host::ofx::property::Pointer )
BOOST_CLASS_EXPORT_IMPLEMENT( tuttle::host::ofx::property::String )
//BOOST_CLASS_EXPORT( tuttle::host::ofx::property::Int )
//BOOST_CLASS_EXPORT( tuttle::host::ofx::property::Double )
//BOOST_CLASS_EXPORT( tuttle::host::ofx::property::Pointer )
//BOOST_CLASS_EXPORT( tuttle::host::ofx::property::String )

