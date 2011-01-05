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
#ifndef _TUTTLE_HOST_OFX_PROPERTY_SET_HPP_
#define _TUTTLE_HOST_OFX_PROPERTY_SET_HPP_

#include "OfxhPropertyTemplate.hpp"

#include <boost/ptr_container/serialize_ptr_map.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace property {

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
public:
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

	#ifndef SWIG
	friend std::ostream& operator<<( std::ostream& os, const This& g );
	#endif

	#ifdef SWIG
	%extend
	{
		//		const OfxhProperty& __getitem__( const std::string& name ) const
		//		{
		//			return self->fetchProperty(name);
		//		}
		OfxhProperty& __getitem__( const std::string& name )
		{
			return self->fetchLocalProperty( name );
		}

		std::string __str__() const
		{
			std::stringstream s;

			s << *self;
			return s.str();
		}

	}
	#endif

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
	const OfxhProperty& fetchProperty( const std::string& name ) const;
	OfxhProperty&       fetchLocalProperty( const std::string& name );
	const OfxhProperty& fetchLocalProperty( const std::string& name ) const { return const_cast<OfxhSet*>( this )->fetchLocalProperty( name ); }

	/// get property with the particular name and type.  if the property is
	/// missing or is of the wrong type, return an error status.  if this is a sloppy
	/// property set and the property is missing, a new one will be created of the right
	/// type
	template<class T>
	const T& fetchTypedProperty( const std::string& name ) const
	{
		return dynamic_cast<const T&>( fetchProperty( name ) );
	}

	template<class T>
	T& fetchLocalTypedProperty( const std::string& name )
	{
		return dynamic_cast<T&>( fetchLocalProperty( name ) );
	}

	template<class T>
	const T& fetchLocalTypedProperty( const std::string& name ) const
	{
		return const_cast<OfxhSet*>( this )->fetchLocalTypedProperty<T>( name );
	}

	const String& fetchStringProperty( const std::string& name ) const
	{
		return fetchTypedProperty<String>( name );
	}

	String& fetchLocalStringProperty( const std::string& name )
	{
		return fetchLocalTypedProperty<String>( name );
	}

	const String& fetchLocalStringProperty( const std::string& name ) const { return const_cast<OfxhSet*>( this )->fetchLocalStringProperty( name ); }

	const Int& fetchIntProperty( const std::string& name ) const
	{
		return fetchTypedProperty<Int>( name );
	}

	Int& fetchLocalIntProperty( const std::string& name )
	{
		return fetchLocalTypedProperty<Int>( name );
	}

	const Int& fetchLocalIntProperty( const std::string& name ) const { return const_cast<OfxhSet*>( this )->fetchLocalIntProperty( name ); }

	const Pointer& fetchPointerProperty( const std::string& name ) const
	{
		return fetchTypedProperty<Pointer>( name );
	}

	Pointer& fetchLocalPointerProperty( const std::string& name )
	{
		return fetchLocalTypedProperty<Pointer>( name );
	}

	const Pointer& fetchLocalPointerProperty( const std::string& name ) const { return const_cast<OfxhSet*>( this )->fetchLocalPointerProperty( name ); }

	const Double& fetchDoubleProperty( const std::string& name ) const
	{
		return fetchTypedProperty<Double>( name );
	}

	Double& fetchLocalDoubleProperty( const std::string& name )
	{
		return fetchLocalTypedProperty<Double>( name );
	}

	const Double& fetchLocalDoubleProperty( const std::string& name ) const { return const_cast<OfxhSet*>( this )->fetchLocalDoubleProperty( name ); }

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
	void serialize( Archive& ar, const unsigned int version )
	{
		ar& BOOST_SERIALIZATION_NVP( _props );
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
		TUTTLE_COUT_ERROR( "Property::Set::setProperty - Error on " << property << " property (value=" << value << ")." <<
		            "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
		TUTTLE_COUT_EXCEPTION( e );
		//TUTTLE_COUT_DEBUG( *this );
	}
	catch(... )
	{
		TUTTLE_COUT_ERROR( "Property::Set::setProperty - Error on " << property << " property (value=" << value << ")." <<
		            "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
		//TUTTLE_COUT_DEBUG( *this );
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
		TUTTLE_COUT_ERROR( "Set::setProperty - Error on " << property << " property (value=" << value << ")." );
		TUTTLE_COUT_ERROR( "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
		TUTTLE_COUT_EXCEPTION( e );
	}
	catch(... )
	{
		TUTTLE_COUT_ERROR( "Set::setProperty - Error on " << property << " property (value=" << value << ")." );
		TUTTLE_COUT_ERROR( "on Property::Set (type:" << this->getStringProperty( kOfxPropType ) << ", name:" << this->getStringProperty( kOfxPropName ) << ")." );
	}
}

/// get a particular property

template<class T>
typename T::ReturnType OfxhSet::getProperty( const std::string& property, int index ) const
{
	/*
	   if( !hasProperty( property, true ) )
	   {
	    TUTTLE_TCOUT( "return kEmpty on property: " << property );
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

#endif
