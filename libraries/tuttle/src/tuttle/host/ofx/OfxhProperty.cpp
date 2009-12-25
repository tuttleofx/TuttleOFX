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

// ofx host
#include "OfxhProperty.hpp"
#include "OfxhBinary.hpp"

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/core/Exception.hpp>

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

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
int OfxhGetHook::getProperty<OfxhIntValue>( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	return getIntProperty( name, index );
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
double OfxhGetHook::getProperty<OfxhDoubleValue>( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	return getDoubleProperty( name, index );
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
void* OfxhGetHook::getProperty<OfxhPointerValue>( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	return getPointerProperty( name, index );
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
const std::string& OfxhGetHook::getProperty<OfxhStringValue>( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	return getStringProperty( name, index );
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
void OfxhGetHook::getPropertyN<OfxhIntValue>( const std::string& name, int* values, int count ) const OFX_EXCEPTION_SPEC
{
	getIntPropertyN( name, values, count );
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
void OfxhGetHook::getPropertyN<OfxhDoubleValue>( const std::string& name, double* values, int count ) const OFX_EXCEPTION_SPEC
{
	getDoublePropertyN( name, values, count );
}

/// this does some magic so that it calls get string/int/double/pointer appropriately

template<>
void OfxhGetHook::getPropertyN<OfxhPointerValue>( const std::string& name, void** values, int count ) const OFX_EXCEPTION_SPEC
{
	getPointerPropertyN( name, values, count );
}

/// override this to get a single value at the given index.

const std::string& OfxhGetHook::getStringProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getStringProperty!!!! " << std::endl;
	#endif
	return OfxhStringValue::kEmpty;
}

/// override this to fetch a single value at the given index.

int OfxhGetHook::getIntProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getIntProperty!!!! " << std::endl;
	#endif
	return 0;
}

/// override this to fetch a single value at the given index.

double OfxhGetHook::getDoubleProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getDoubleProperty!!!! " << std::endl;
	#endif
	return 0;
}

/// override this to fetch a single value at the given index.

void* OfxhGetHook::getPointerProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getPointerProperty!!!! " << std::endl;
	#endif
	return NULL;
}

/// override this to fetch a multiple values in a multi-dimension property

void OfxhGetHook::getDoublePropertyN( const std::string& name, double* values, int count ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getDoublePropertyN!!!! " << std::endl;
	#endif
	memset( values, 0, sizeof( double ) * count );
}

/// override this to fetch a multiple values in a multi-dimension property

void OfxhGetHook::getIntPropertyN( const std::string& name, int* values, int count ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getIntPropertyN!!!! " << std::endl;
	#endif
	memset( values, 0, sizeof( int ) * count );
}

/// override this to fetch a multiple values in a multi-dimension property

void OfxhGetHook::getPointerPropertyN( const std::string& name, void** values, int count ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getPointerPropertyN!!!! " << std::endl;
	#endif
	memset( values, 0, sizeof( void* ) * count );
}

/// override this to fetch the dimension size.

size_t OfxhGetHook::getDimension( const std::string& name ) const OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::getDimension!!!! " << std::endl;
	#endif
	return 0;
}

/// override this to handle a reset().

void OfxhGetHook::reset( const std::string& name ) OFX_EXCEPTION_SPEC
{
	#ifdef DEBUG_PROPERTIES
	std::cout << "Calling un-overriden GetHook::reset!!!! " << std::endl;
	#endif
}

OfxhProperty::OfxhProperty( const std::string& name,
                            TypeEnum           type,
                            size_t             dimension,
                            bool               pluginReadOnly )
	: _name( name ),
	_type( type ),
	_dimension( dimension ),
	_pluginReadOnly( pluginReadOnly ),
	_getHook( 0 ) {}

OfxhProperty::OfxhProperty( const OfxhProperty& other )
	: _name( other._name ),
	_type( other._type ),
	_dimension( other._dimension ),
	_pluginReadOnly( other._pluginReadOnly ),
	_getHook( 0 ) {}

/// call notify on the contained notify hooks

void OfxhProperty::notify( bool single, int indexOrN )
{
	std::vector<OfxhNotifyHook*>::iterator i;
	for( i = _notifyHooks.begin(); i != _notifyHooks.end(); ++i )
	{
		( *i )->notify( _name, single, indexOrN );
	}
}

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
	: OfxhProperty( pt ),
	_value( pt._value ),
	_defaultValue( pt._defaultValue ) {}

#if defined( WINDOWS ) && !defined( __GNUC__ )
#pragma warning( disable : 4181 )
#endif
/// get one value

template<class T>
const typename T::ReturnType OfxhPropertyTemplate<T>::getValue( int index ) const OFX_EXCEPTION_SPEC
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
const typename T::ReturnType OfxhPropertyTemplate<T>::getValueRaw( int index ) const OFX_EXCEPTION_SPEC
{
	if( index < 0 || ( (size_t) index >= _value.size() ) )
	{
		throw OfxhException( kOfxStatErrBadIndex );
	}
	return _value[index];
}

template<class T>
typename T::ReturnType OfxhPropertyTemplate<T>::getConstlessValueRaw( int index ) const OFX_EXCEPTION_SPEC
{
	if( index < 0 || ( (size_t) index >= _value.size() ) )
	{
		throw OfxhException( kOfxStatErrBadIndex );
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
void OfxhPropertyTemplate<T>::getValueNRaw( APIType* value, int count ) const OFX_EXCEPTION_SPEC
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
void OfxhPropertyTemplate<T>::setValue( const typename T::Type& value, int index ) OFX_EXCEPTION_SPEC
{
	if( index < 0 || ( (size_t) index > _value.size() && _dimension ) )
	{
		throw OfxhException( kOfxStatErrBadIndex );
	}

	if( _value.size() <= (size_t) index )
	{
		_value.resize( index + 1 );
	}
	_value[index] = value;

//	TCOUT( "setValue: " << value << " at index: " << index );

	notify( true, index );
}

/**
 * set multiple values
 */
template<class T>
void OfxhPropertyTemplate<T>::setValueN( const typename T::APIType* value, int count ) OFX_EXCEPTION_SPEC
{
	if( _dimension && ( (size_t) count > _value.size() ) )
	{
		throw OfxhException( kOfxStatErrBadIndex );
	}
	if( _value.size() != (size_t) count )
	{
		_value.resize( count );
	}
	for( int i = 0; i < count; ++i )
	{
		_value[i] = value[i];
	}

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
}

void OfxhSet::setGetHook( const std::string& s, OfxhGetHook* ghook )
{
	fetchLocalProperty( s ).setGetHook( ghook );
}

/**
 * add a notify hook for a particular property.  users may need to call particular
 * specialised versions of this.
 */
void OfxhSet::addNotifyHook( const std::string& s, OfxhNotifyHook* hook )
{
	fetchLocalProperty( s ).addNotifyHook( hook );
}

OfxhProperty& OfxhSet::fetchLocalProperty( const std::string& name ) throw( core::exception::LogicError, std::bad_cast )
{
	PropertyMap::iterator i = _props.find( name );

	if( i == _props.end() )
	{
		throw core::exception::LogicError( "fetchLocalProperty: " + name + ". Property not found."); //+ " on type:" + getStringProperty(kOfxPropType) + " name:" + getStringProperty(kOfxPropName) );// " NULL, (followChain: " << followChain << ").";
	}
	return *(i->second);
}

const OfxhProperty& OfxhSet::fetchProperty( const std::string& name ) const throw( core::exception::LogicError, std::bad_cast )
{
	PropertyMap::const_iterator i = _props.find( name );

	if( i == _props.end() )
	{
		if( _chainedSet )
		{
			return _chainedSet->fetchProperty( name );
		}
		throw core::exception::LogicError( "fetchProperty: " + name + " property not found." ); //+ " on type:" + getStringProperty(kOfxPropType) + " name:" + getStringProperty(kOfxPropName) );// " NULL, (followChain: " << followChain << ").";
	}
	return *(i->second);
}

/**
 * add one new property
 */
void OfxhSet::createProperty( const OfxhPropSpec& spec )
{
	if( _props.find( spec.name ) != _props.end() )
	{
		throw core::exception::LogicError( "Tried to add a duplicate property to a Property::Set (" ) << spec.name << ")";
	}

	switch( spec.type )
	{
		case eInt:
			_props.insert( spec.name, new Int( spec.name, spec.dimension, spec.readonly, spec.defaultValue ? atoi( spec.defaultValue ) : 0 ) );
			break;
		case eDouble:
			_props.insert( spec.name, new Double( spec.name, spec.dimension, spec.readonly, spec.defaultValue ? atof( spec.defaultValue ) : 0 ) );
			break;
		case eString:
			_props.insert( spec.name, new String( spec.name, spec.dimension, spec.readonly, spec.defaultValue ? spec.defaultValue : "" ) );
			break;
		case ePointer:
			_props.insert( spec.name, new Pointer( spec.name, spec.dimension, spec.readonly, (void*) spec.defaultValue ) );
			break;
		default:
			throw core::exception::LogicError( "Tried to create a property of an unrecognized type (" ) << spec.name << ", " << spec.type << ")";
			break;
	}
}

void OfxhSet::addProperties( const OfxhPropSpec spec[] )
{
	while( spec->name )
	{
		createProperty( *spec );
		++spec;
	}
}

void OfxhSet::eraseProperty( const std::string& propName )
{
	_props.erase( propName );
}

bool OfxhSet::hasProperty( const std::string& propName, bool followChain ) const
{
	PropertyMap::const_iterator it = _props.find(propName);
	if( it == _props.end() )
	{
		if( followChain && _chainedSet )
		{
			return _chainedSet->hasProperty( propName, true );
		}
	}
	return it != _props.end();
}

/**
 * add one new property
 */
void OfxhSet::addProperty( OfxhProperty* prop )
{
	_props.insert(prop->getName(), prop);
}

/**
 * empty ctor
 */
OfxhSet::OfxhSet()
	: _magic( kMagic )
	, _chainedSet( NULL )
{

}

OfxhSet::OfxhSet( const OfxhPropSpec spec[] )
	: _magic( kMagic )
	, _chainedSet( NULL )
{
	addProperties( spec );
}

OfxhSet::OfxhSet( const OfxhSet& other )
	: _magic( kMagic )
	, _props( other._props )
	, _chainedSet( NULL )
{
}

OfxhSet::~OfxhSet()
{
	clear();
}

void OfxhSet::clear()
{
	_props.clear();
}

/// hide assignment
void OfxhSet::operator=( const OfxhSet& other )
{
	clear();
	_props      = other._props.clone();
	_chainedSet = other._chainedSet;
}

bool OfxhSet::operator==( const OfxhSet& other ) const
{
	if( _props != other._props )
		return false;
	if( _chainedSet == NULL )
	{
		if( other._chainedSet != NULL )
			if( other._chainedSet->getSize() != 0 )
				return false;
	}
	else
	{
		if( other._chainedSet == NULL )
			if( _chainedSet->getSize() != 0 )
				return false;
		if( *_chainedSet != *(other._chainedSet) )
			return false;
	}
	return true;
}

void OfxhSet::coutProperties() const
{
	COUT( "property::Set {" );
	for( PropertyMap::const_iterator it = _props.begin(), itEnd = _props.end();
	     it != itEnd;
	     ++it )
	{
		const OfxhProperty& prop = *(it->second);
		std::cout << "    " << it->first << " ";
		std::cout << "(type:" << mapTypeEnumToString( prop.getType() ) << " dim:" << prop.getDimension() << " ro:" << prop.getPluginReadOnly() << ") : [";
		int i = 0;
		for( ; i < (int)( prop.getDimension() ) - 1; ++i )
		{
			std::cout << prop.getStringValue( i ) << ", ";
		}
		if( prop.getDimension() > 0 )
			std::cout << prop.getStringValue( i );
		std::cout << "] " << std::endl;
	}
	COUT( "}" );
}

/// get a particular int property
int OfxhSet::getIntPropertyRaw( const std::string& property, int index ) const
{
	return getPropertyRaw<OfxhIntValue>( property, index );
}

/// get a particular double property

double OfxhSet::getDoublePropertyRaw( const std::string& property, int index ) const
{
	return getPropertyRaw<OfxhDoubleValue>( property, index );
}

/// get a particular double property

void* OfxhSet::getPointerPropertyRaw( const std::string& property, int index ) const
{
	return getPropertyRaw<OfxhPointerValue>( property, index );
}

/// get a particular double property

const std::string& OfxhSet::getStringPropertyRaw( const std::string& property, int index ) const
{
	return fetchTypedProperty<OfxhPropertyTemplate<String> >( property ).getValueRaw( index );
	//return OfxhStringValue::kEmpty;
}

/// get a particular int property

int OfxhSet::getIntProperty( const std::string& property, int index ) const
{
	return getProperty<OfxhIntValue >( property, index );
}

/// get the value of a particular double property

void OfxhSet::getIntPropertyN( const std::string& property, int* v, int N ) const
{
	return getPropertyN<OfxhIntValue >( property, N, v );
}

/// get a particular double property

double OfxhSet::getDoubleProperty( const std::string& property, int index ) const
{
	return getProperty<OfxhDoubleValue >( property, index );
}

/// get the value of a particular double property

void OfxhSet::getDoublePropertyN( const std::string& property, double* v, int N ) const
{
	return getPropertyN<OfxhDoubleValue >( property, N, v );
}

/// get a particular double property

void* OfxhSet::getPointerProperty( const std::string& property, int index ) const
{
	return getProperty<OfxhPointerValue >( property, index );
}

/// get a particular double property

const std::string& OfxhSet::getStringProperty( const std::string& property, int index ) const
{
	return getProperty<OfxhStringValue >( property, index );
}

/// is the given string one of the values of a multi-dimensional string prop
/// this returns a non negative index if it is found, otherwise, -1

int OfxhSet::findStringPropValueIndex( const std::string& propName,
                                       const std::string& propValue ) const
{
	const String& prop = fetchStringProperty( propName );
	const std::vector<std::string>& values     = prop.getValues();
	std::vector<std::string>::const_iterator i = find( values.begin(), values.end(), propValue );
	if( i != values.end() )
	{
		return int(i - values.begin() );
	}
	return -1;
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
