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

#include "OfxhUtilities.hpp"
#include "OfxhException.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/core/Exception.hpp>

#include <boost/type_traits/is_virtual_base_of.hpp>
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/ptr_container/serialize_ptr_map.hpp>
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
class OfxhProperty;
class OfxhSet;

/// type of a property
enum TypeEnum
{
	eNone    = -1,
	eInt     = 0,
	eDouble  = 1,
	eString  = 2,
	ePointer = 3
};

inline std::string mapTypeEnumToString( const TypeEnum& e )
{
	switch( e )
	{
		case eNone:
			return "none";
		case eInt:
			return "int";
		case eDouble:
			return "double";
		case eString:
			return "string";
		case ePointer:
			return "pointer";
		default:
			throw OfxhException( kOfxStatErrValue );
	}
}

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

/// Sits on a property and can override the local property value when a value is being fetched
/// only one of these can be in any property (as the thing has only a single value).
/// We deliberately don't have a getStringPropertyN as it is somewhat more painfull and never
/// used in practice.
class OfxhGetHook
{
public:
	/// dtor
	virtual ~OfxhGetHook()
	{}

	/// We specialise this to do some magic so that it calls get string/int/double/pointer appropriately
	/// this is what is called by the propertytemplate code to fetch values out of a hook.
	template<class T>
	typename T::ReturnType getProperty( const std::string& name, int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// We specialise this to do some magic so that it calls get int/double/pointer appropriately
	/// this is what is called by the propertytemplate code to fetch values out of a hook.
	template<class T>
	void getPropertyN( const std::string & name, typename T::APIType * values, int count ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a single value at the given index.
	virtual const std::string& getStringProperty( const std::string& name, int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a single value at the given index.
	virtual int getIntProperty( const std::string& name, int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a multiple values in a multi-dimension property
	virtual void getIntPropertyN( const std::string& name, int* values, int count ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a single value at the given index.
	virtual double getDoubleProperty( const std::string& name, int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a multiple values in a multi-dimension property
	virtual void getDoublePropertyN( const std::string& name, double* values, int count ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a single value at the given index.
	virtual void* getPointerProperty( const std::string& name, int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch a multiple values in a multi-dimension property
	virtual void getPointerPropertyN( const std::string& name, void** values, int count ) const OFX_EXCEPTION_SPEC;

	/// override this to fetch the dimension size.
	virtual size_t getDimension( const std::string& name ) const OFX_EXCEPTION_SPEC;

	/// override this to handle a reset().
	virtual void reset( const std::string& name ) OFX_EXCEPTION_SPEC;
};

/// Sits on a property and is called when the local property is being set.
/// It notify or notifyN is called whenever the plugin sets a property
/// Many of these can sit on a property, as various objects will need to know when a property
/// has been changed. On notification you should fetch properties with a 'raw' call, rather
/// than the standard calls, as you may be fetching through a getHook and you won't see
/// the local value that has been shoved into the property.
class OfxhNotifyHook
{
public:
	/// dtor
	virtual ~OfxhNotifyHook() {}

	/// override this to be notified when a property changes
	/// \arg name is the name of the property just set
	/// \arg singleValue is whether setProperty on a single index was call, otherwise N properties were set
	/// \arg indexOrN is the index if single value is true, or the count if singleValue is false
	virtual void notify( const std::string& name, bool singleValue, int indexOrN ) OFX_EXCEPTION_SPEC = 0;
};


/// base class for all properties
class OfxhProperty : private boost::noncopyable
{
typedef OfxhProperty This;
public:
	enum EModifiedBy
	{
		eModifiedByHost = 0,
		eModifiedByPlugin
	};
protected:
	std::string _name;                         ///< name of this property
	TypeEnum _type;                            ///< type of this property
	size_t _dimension;                         ///< the fixed dimension of this property
	bool _pluginReadOnly;                      ///< set is forbidden through suite: value may still change between get() calls
	EModifiedBy _modifiedBy;                   ///< who set this property most recently
	std::vector<OfxhNotifyHook*> _notifyHooks; ///< hooks to call whenever the property is set
	OfxhGetHook* _getHook;                     ///< if we are not storing props locally, they are stored via fetching from here

	friend class OfxhSet;

public:
	OfxhProperty( const std::string& name,
	              TypeEnum           type,
	              size_t             dimension = 1,
	              bool               pluginReadOnly = false );

	OfxhProperty( const This& other );

	virtual ~OfxhProperty() = 0;

	virtual bool operator==( const This& other ) const
	{
		if( _name != other._name )
		{
			//COUT( "OfxhProperty::operator== not same name : " << _name << " != " << other._name );
			return false;
		}
		if( _type != other._type )
		{
			//COUT( "OfxhProperty::operator== not same type : " << _type << " != " << other._type );
			return false;
		}
		if( _dimension != other._dimension )
		{
			//COUT( "OfxhProperty::operator== not same size : " << _dimension << " != " << other._dimension );
			return false;
		}
		if( _pluginReadOnly != other._pluginReadOnly )
		{
			//COUT( "OfxhProperty::operator== not sale read only : " << _pluginReadOnly << " != " << other._pluginReadOnly );
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

	void setModifiedBy( const EModifiedBy who ) { _modifiedBy = who; }
	EModifiedBy getModifiedBy() const { return _modifiedBy; }

	/// override this to return a clone of the property
	virtual OfxhProperty* clone() const = 0;

	/// get the name of this property
	const std::string& getName() const
	{
		return _name;
	}

	/// get the type of this property
	TypeEnum getType() const
	{
		return _type;
	}

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

	// get the current dimension of this property
	virtual size_t getDimension() const = 0;

	/// get the fixed dimension of this property
	size_t getFixedDimension() const
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

	// get a string representing the value of this property at element nth
	virtual std::string getStringValue( int nth ) const = 0;


private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(_name);
		ar & BOOST_SERIALIZATION_NVP(_type);
		ar & BOOST_SERIALIZATION_NVP(_dimension);
		ar & BOOST_SERIALIZATION_NVP(_pluginReadOnly);
	}
};

inline OfxhProperty* new_clone( const OfxhProperty& p )
{
	return p.clone();
}

/// this represents a generic property.
/// template parameter T is the type descriptor of the
/// type of property to model.  the class holds an internal _value vector which can be used
/// to store the values.  if set and get hooks are installed, these will be called instead
/// of using this variable.
/// Make sure that T::ReturnType is const if appropriate, as no extra qualifiers are applied here.
template<class T>
class OfxhPropertyTemplate : public OfxhProperty
{
typedef OfxhPropertyTemplate<T> This;
public:
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
			//COUT( "OfxhPropertyTemplate::operator== not same type : " << getType() << " != " << other.getType() );
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
			//COUT( "OfxhPropertyTemplate::operator== not same value : " );
			//for( typename std::vector<Type>::const_iterator it = _value.begin(), itEnd = _value.end(), ito = other._value.begin(), itoEnd = other._value.end();
			//	 it != itEnd && ito != itoEnd;
			//     ++it, ++ito )
			//{
			//	COUT( *it << " != " << *ito );
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
			throw core::exception::LogicError( "You try to copy a property value, but it is not the same property type." );
		copyValues( dynamic_cast<const This&>( other ) );
	}
	
	void copyValues( const This& other )
	{
		if( OfxhProperty::operator!=( other ) )
			throw core::exception::LogicError( "You try to copy a property value, but it is not the same property." );
		_value = other._value;
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
	const ReturnType getValue( int index = 0 ) const OFX_EXCEPTION_SPEC;

	/// get one value, without going through the getHook
	const ReturnType getValueRaw( int index = 0 ) const OFX_EXCEPTION_SPEC;

	#if defined( WINDOWS ) && !defined( __GNUC__ )
	#pragma warning( default : 4181 )
	#endif
	// get multiple values, without going through the getHook
	void getValueNRaw( APIType* value, const int count ) const OFX_EXCEPTION_SPEC;

	/// set one value
	void setValue( const Type& value, const int index = 0, const OfxhProperty::EModifiedBy who = OfxhProperty::eModifiedByHost ) OFX_EXCEPTION_SPEC;

	/// set multiple values
	void setValueN( const APIType* value, const int count, const OfxhProperty::EModifiedBy who = OfxhProperty::eModifiedByHost ) OFX_EXCEPTION_SPEC;

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
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhProperty);
		ar & BOOST_SERIALIZATION_NVP(_value);
		ar & BOOST_SERIALIZATION_NVP(_defaultValue);
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
void Pointer::serialize( Archive &ar, const unsigned int version )
{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhProperty);
}

/// A class that is used to initialize a property set. Feed in an array of these to
/// a property and it will construct a bunch of properties. Terminate such an array
/// with an empty (all zero) set.
struct OfxhPropSpec
{
	const char* name;          ///< name of the property
	TypeEnum type;             ///< type
	int dimension;             ///< fixed dimension of the property, set to zero if variable dimension
	bool readonly;             ///< is the property plug-in read only
	const char* defaultValue;  ///< Default value as a string. Pointers are ignored and always null.
};

/// A std::map of properties by name
typedef boost::ptr_map<std::string, OfxhProperty> PropertyMap;

/**
 * Class that holds a set of properties and manipulates them
 * The 'fetch' methods return a property object.
 * The 'get' methods return a property value
 */
class OfxhSet
{
typedef OfxhSet This;
private:
	static const int kMagic = 0x12082007; ///< magic number for property sets, and Connie's birthday :-)
	const int _magic; ///< to check for handles being nice

protected:
	PropertyMap _props; ///< Our properties.

	/// chained property set, which is read only
	/// these are searched on a get if not found
	/// on a local search
	const OfxhSet* _chainedSet;

	/// set a particular property
	template<class T>
	void setProperty( const std::string& property, int index, const typename T::Type& value );

	/// set the first N of a particular property
	template<class T>
	void setPropertyN( const std::string& property, int count, const typename T::APIType* value );

	/// get a particular property
	template<class T>
	typename T::ReturnType getProperty( const std::string& property, int index )  const;

	/// get the first N of a particular property
	template<class T>
	void getPropertyN( const std::string & property, int index, typename T::APIType * v )  const;

	/// get a particular property without going through any getHook
	template<class T>
	typename T::ReturnType getPropertyRaw( const std::string& property, int index )  const;

	/// get a particular property without going through any getHook
	template<class T>
	void getPropertyRawN( const std::string & property, int count, typename T::APIType * v )  const;

public:
	/// take an array of of PropSpecs (which must be terminated with an entry in which
	/// ->name is null), and turn these into a Set
	OfxhSet( const OfxhPropSpec spec[] );
	//        explicit Set(const std::vector<const PropSpec*>& multipleSpec);

	/// deep copies the property set
	OfxhSet( const OfxhSet& );

	/// empty ctor
	OfxhSet();

	/// destructor
	virtual ~OfxhSet();

	void clear();

	size_t getLocalSize() const { return _props.size(); }
	size_t getSize() const
	{
		if( _chainedSet == NULL )
			return getLocalSize();
		else
			return getLocalSize() + _chainedSet->getSize();
	}

	/// hide assignment
	void operator=( const This& );

	bool operator==( const This& ) const;
	bool operator!=( const This& other ) const { return !This::operator==( other ); }

	void copyValues( const This& other );

	/// dump to cout
	void coutProperties() const;

	/// adds a bunch of properties from PropSpec
	void addProperties( const OfxhPropSpec* );

	void eraseProperty( const std::string& propName );

	bool hasProperty( const std::string& propName, bool followChain = false ) const;

	inline OfxhSet& operator+( const OfxhPropSpec* p ) { addProperties( p ); return *this; }

	/// add one new property
	void createProperty( const OfxhPropSpec& s );

	/// add one new property
	void addProperty( OfxhProperty* prop );

	/// set the chained property set
	void setChainedSet( const OfxhSet* const s ) { _chainedSet = s; }

	/// grab the internal properties map
	const PropertyMap& getMap() const { return _props; }
	PropertyMap&       getMap()       { return _props; }

	/// set the get hook for a particular property.  users may need to call particular
	/// specialised versions of this.
	void setGetHook( const std::string& s, OfxhGetHook* ghook );

	/// add a set hook for a particular property.  users may need to call particular
	/// specialised versions of this.
	void addNotifyHook( const std::string& name, OfxhNotifyHook* hook );

	/// Fetchs a reference to a property of the given name, following the property chain if the
	/// 'followChain' arg is not false.
	const OfxhProperty& fetchProperty( const std::string& name ) const throw( OfxhException, std::bad_cast );
	OfxhProperty&       fetchLocalProperty( const std::string& name ) throw( OfxhException, std::bad_cast );
	const OfxhProperty& fetchLocalProperty( const std::string& name ) const throw( OfxhException, std::bad_cast ) { return const_cast<OfxhSet*>( this )->fetchLocalProperty( name ); }

	/// get property with the particular name and type.  if the property is
	/// missing or is of the wrong type, return an error status.  if this is a sloppy
	/// property set and the property is missing, a new one will be created of the right
	/// type
	template<class T>
	const T& fetchTypedProperty( const std::string& name ) const throw( OfxhException, std::bad_cast )
	{
		return dynamic_cast<const T&>( fetchProperty( name ) );
	}

	template<class T>
	T& fetchLocalTypedProperty( const std::string& name ) throw( OfxhException, std::bad_cast )
	{
		return dynamic_cast<T&>( fetchLocalProperty( name ) );
	}

	template<class T>
	const T& fetchLocalTypedProperty( const std::string& name ) const throw( OfxhException, std::bad_cast )
	{
		return const_cast<OfxhSet*>( this )->fetchLocalTypedProperty<T>( name );
	}

	const String& fetchStringProperty( const std::string& name ) const throw( OfxhException, std::bad_cast )
	{
		return fetchTypedProperty<String>( name );
	}

	String& fetchLocalStringProperty( const std::string& name ) throw( OfxhException, std::bad_cast )
	{
		return fetchLocalTypedProperty<String>( name );
	}

	const String& fetchLocalStringProperty( const std::string& name ) const throw( OfxhException, std::bad_cast ) { return const_cast<OfxhSet*>( this )->fetchLocalStringProperty( name ); }

	const Int& fetchIntProperty( const std::string& name ) const throw( OfxhException, std::bad_cast )
	{
		return fetchTypedProperty<Int>( name );
	}

	Int& fetchLocalIntProperty( const std::string& name ) throw( OfxhException, std::bad_cast )
	{
		return fetchLocalTypedProperty<Int>( name );
	}

	const Int& fetchLocalIntProperty( const std::string& name ) const throw( OfxhException, std::bad_cast ) { return const_cast<OfxhSet*>( this )->fetchLocalIntProperty( name ); }

	const Pointer& fetchPointerProperty( const std::string& name ) const throw( OfxhException, std::bad_cast )
	{
		return fetchTypedProperty<Pointer>( name );
	}

	Pointer& fetchLocalPointerProperty( const std::string& name ) throw( OfxhException, std::bad_cast )
	{
		return fetchLocalTypedProperty<Pointer>( name );
	}

	const Pointer& fetchLocalPointerProperty( const std::string& name ) const throw( OfxhException, std::bad_cast ) { return const_cast<OfxhSet*>( this )->fetchLocalPointerProperty( name ); }

	const Double& fetchDoubleProperty( const std::string& name ) const throw( OfxhException, std::bad_cast )
	{
		return fetchTypedProperty<Double>( name );
	}

	Double& fetchLocalDoubleProperty( const std::string& name ) throw( OfxhException, std::bad_cast )
	{
		return fetchLocalTypedProperty<Double>( name );
	}

	const Double& fetchLocalDoubleProperty( const std::string& name ) const throw( OfxhException, std::bad_cast ) { return const_cast<OfxhSet*>( this )->fetchLocalDoubleProperty( name ); }

	/// get a particular int property without fetching via a get hook, useful for notifies
	int getIntPropertyRaw( const std::string& property, int index = 0 ) const;

	/// get a particular double property without fetching via a get hook, useful for notifies
	double getDoublePropertyRaw( const std::string& property, int index = 0 ) const;

	/// get a particular pointer property without fetching via a get hook, useful for notifies
	void* getPointerPropertyRaw( const std::string& property, int index = 0 ) const; /// @todo tuttle: return const no ?

	/// get a particular string property
	const std::string& getStringPropertyRaw( const std::string& property, int index = 0 ) const;

	/// get the value of a particular string property
	const std::string& getStringProperty( const std::string& property, int index = 0 ) const;

	/// get the value of a particular int property
	int getIntProperty( const std::string& property, int index = 0 ) const;

	/// get the value of a particular double property
	void getIntPropertyN( const std::string& property,  int* v, int N ) const;

	/// get the value of a particular double property
	double getDoubleProperty( const std::string& property, int index = 0 ) const;

	/// get the value of a particular double property
	void getDoublePropertyN( const std::string& property,  double* v, int N ) const;

	/// get the value of a particular pointer property
	void* getPointerProperty( const std::string& property, int index = 0 ) const;

	/// set a particular string property without fetching via a get hook, useful for notifies
	void setStringProperty( const std::string& property, const std::string& value, int index = 0 ) { setProperty<OfxhStringValue >( property, index, value ); }

	/// set a particular int property
	void setIntProperty( const std::string& property, int v, int index = 0 ) { setProperty<OfxhIntValue >( property, index, v ); }

	/// set a particular double property
	void setIntPropertyN( const std::string& property, const int* v, int N )
	{
		setPropertyN<OfxhIntValue >( property, N, v );
	}

	/// get a particular double property
	void setDoubleProperty( const std::string& property, double v, int index = 0 ) { setProperty<OfxhDoubleValue >( property, index, v ); }

	/// get a particular double property
	void setDoublePropertyN( const std::string& property, const double* v, int N ) { setPropertyN<OfxhDoubleValue >( property, N, v ); }

	/// get a particular double property
	void setPointerProperty( const std::string& property, void* v, int index = 0 ) { setProperty<OfxhPointerValue >( property, index, v ); }

	/// get the dimension of a particular property
	size_t getDimension( const std::string& property ) const { return fetchProperty( property ).getDimension(); }

	/// is the given string one of the values of a multi-dimensional string prop
	/// this returns a non negative index if it is found, otherwise, -1
	int findStringPropValueIndex( const std::string& propName,
	                              const std::string& propValue ) const;

	/// get a handle on this object for passing to the C API
	OfxPropertySetHandle getHandle() const { return ( OfxPropertySetHandle ) this; }

	/// is this a nice property set, or a dodgy pointer passed back to us
	bool verifyMagic() { return this != NULL && _magic == kMagic; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(_props);
	}
};

/// set a particular property
template<class T>
void OfxhSet::setProperty( const std::string& property, int index, const typename T::Type& value )
{
	try
	{
		fetchLocalTypedProperty<OfxhPropertyTemplate<T> >( property ).setValue( value, index );
	}
	catch( OfxhException& e )
	{
		COUT_ERROR( "Property::Set::setProperty - Error on " << property << " property (value=" << value << ")." <<
		            "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
		COUT_EXCEPTION( e );
		coutProperties();
	}
	catch(... )
	{
		COUT_ERROR( "Property::Set::setProperty - Error on " << property << " property (value=" << value << ")." <<
		            "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
		coutProperties();
	}
}

/// set a particular property

template<class T>
void OfxhSet::setPropertyN( const std::string& property, int count, const typename T::APIType* value )
{
	try
	{
		fetchLocalTypedProperty<OfxhPropertyTemplate<T> >( property ).setValueN( value, count );
	}
	catch( OfxhException& e )
	{
		COUT_ERROR( "Set::setProperty - Error on " << property << " property (value=" << value << ")." );
		COUT_ERROR( "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
		COUT_EXCEPTION( e );
	}
	catch(... )
	{
		COUT_ERROR( "Set::setProperty - Error on " << property << " property (value=" << value << ")." );
		COUT_ERROR( "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
	}
}

/// get a particular property

template<class T>
typename T::ReturnType OfxhSet::getProperty( const std::string& property, int index ) const
{
	/*
	if( !hasProperty( property, true ) )
	{
		TCOUT( "return kEmpty on property: " << property );
		return T::kEmpty; /// @todo tuttle: is this really needed ?
	}
	 */
	return fetchTypedProperty<OfxhPropertyTemplate<T> >( property ).getValue( index );
}

/// get a particular property

template<class T>
void OfxhSet::getPropertyN( const std::string& property, int count, typename T::APIType* value ) const
{
	fetchTypedProperty<OfxhPropertyTemplate<T> >( property ).getValueN( value, count );
}

/// get a particular property

template<class T>
typename T::ReturnType OfxhSet::getPropertyRaw( const std::string& property, int index ) const
{
	return fetchTypedProperty<OfxhPropertyTemplate<T> >( property ).getValueRaw( index );
}

/// get a particular property

template<class T>
void OfxhSet::getPropertyRawN( const std::string& property, int count, typename T::APIType* value ) const
{
	return fetchTypedProperty<OfxhPropertyTemplate<T> >( property ).getValueNRaw( value, count );
}

}
}
}
}


// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost{
template<> struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::Int>: public mpl::true_ {};
template<> struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::Double>: public mpl::true_ {};
template<> struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::String>: public mpl::true_ {};
template<> struct is_virtual_base_of<tuttle::host::ofx::property::OfxhProperty, tuttle::host::ofx::property::Pointer>: public mpl::true_ {};
}

BOOST_CLASS_EXPORT(tuttle::host::ofx::property::Int)
BOOST_CLASS_EXPORT(tuttle::host::ofx::property::Double)
BOOST_CLASS_EXPORT(tuttle::host::ofx::property::Pointer)
BOOST_CLASS_EXPORT(tuttle::host::ofx::property::String)

#endif
