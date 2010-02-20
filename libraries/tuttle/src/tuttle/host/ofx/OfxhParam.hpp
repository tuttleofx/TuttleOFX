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

#ifndef _TUTTLE_HOST_OFX_PARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_HPP_

#include "OfxhCore.hpp"
#include "OfxhProperty.hpp"
#include "OfxhAttribute.hpp"
#include "OfxhException.hpp"

#include <boost/ptr_container/serialize_ptr_list.hpp>

#include <string>
#include <map>
#include <list>
#include <cstdarg>
#include <cassert>
#include <stdexcept>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

/// is this a standard BaseType
bool isStandardType( const std::string& BaseType );

class OfxhParam;
class OfxhParamSet;

/// base class to the param set instance and param set descriptor
class OfxhParamAccessorSet
{
public:
	virtual ~OfxhParamAccessorSet() = 0;

	/// obtain a handle on this set for passing to the C api
	virtual OfxParamSetHandle getParamSetHandle() const = 0;

	/// get the property handle that lives with the set
	/// The plugin descriptor/instance that derives from
	/// this will provide this.
	virtual property::OfxhSet& getParamSetProps() = 0;
};

/// base class for all params
class OfxhParamAccessor : virtual public OfxhAttributeAccessor
{
public:
	OfxhParamAccessor();
	//        ParamAccessor( const ParamAccessor& other );
	virtual ~OfxhParamAccessor() = 0;

	/// grab a handle on the parameter for passing to the C API
	virtual OfxParamHandle getParamHandle() const = 0;

	virtual bool verifyMagic() { return true; }

	const std::string& getParamType() const;

	const std::string& getParentName() const;

	/// @todo tuttle : common to all attributes
	const std::string& getScriptName() const;

	/// @todo tuttle : common to all attributes
	const std::string& getHint() const;

	const std::string& getDoubleType() const;

	const std::string& getCacheInvalidation() const;

	bool getEnabled() const;

	bool getCanUndo() const;

	bool getSecret() const;

	bool getEvaluateOnChange() const;

	bool getCanAnimate() const;
};

/// the Descriptor of a plugin parameter
class OfxhParamDescriptor :
	public OfxhAttributeDescriptor,
	virtual public OfxhParamAccessor
{
typedef OfxhParamDescriptor This;
OfxhParamDescriptor(){}

public:
	/// make a parameter, with the given type and name
	OfxhParamDescriptor( const std::string& type, const std::string& name );
	~OfxhParamDescriptor(){}

	bool operator==( const This& other ) const
	{
		if( OfxhAttributeDescriptor::operator!=(other) )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }

	/// grab a handle on the parameter for passing to the C API
	OfxParamHandle getParamHandle() const
	{
		return ( OfxParamHandle ) this;
	}

	/// add standard param props, will call the below
	void addStandardParamProps( const std::string& type );

	/// add standard properties to a params that can take an interact
	void addInteractParamProps( const std::string& type );

	/// add standard properties to a value holding param
	void addValueParamProps( const std::string& type, property::TypeEnum valueType, int dim );
	void addNoValueParamProps();

	/// add standard properties to a value holding param
	void addNumericParamProps( const std::string& type, property::TypeEnum valueType, int dim );

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhAttributeDescriptor);
	}
};


/// A set of parameters
///
/// As we are the owning object we delete the params inside ourselves. It was tempting
/// to make params autoref objects and have shared ownership with the client code
/// but that adds complexity for no strong gain.
class OfxhParamSet : public OfxhParamAccessorSet
{
typedef OfxhParamSet This;
public:
	typedef std::map<std::string, OfxhParam*> ParamMap;
	typedef boost::ptr_list<OfxhParam> ParamList;

protected:
	ParamMap _params;        ///< params by name
	ParamList _paramList;    ///< params list

public:
	/// ctor
	///
	/// The propery set being passed in belongs to the owning
	/// plugin instance.
	explicit OfxhParamSet();

	explicit OfxhParamSet( const OfxhParamSet& other );

	/// dtor.
	virtual ~OfxhParamSet();

	void operator=( const OfxhParamSet& other );
	
	void copyParamsValues( const OfxhParamSet& other );
	
	bool operator==( const This& other ) const { return _paramList == other._paramList; }

	bool operator!=( const This& other ) const { return !This::operator==( other ); }

	/// obtain a handle on this set for passing to the C api
	OfxParamSetHandle getParamSetHandle() const { return ( OfxParamSetHandle ) this; }

	const ParamMap& getParams() const { return _params; }
	ParamMap&       getParams()       { return _params; }

	const ParamList& getParamList() const { return _paramList; }
	ParamList&       getParamList()       { return _paramList; }

	// get the param
	OfxhParam& getParam( const std::string& name )
	{
		ParamMap::iterator it = _params.find( name );
		if( it == _params.end() )
			throw OfxhException( kOfxStatErrBadIndex, std::string( "Param not found. (" ) + name + ")" );
		return *it->second;
	}
	const OfxhParam& getParam( const std::string& name ) const { return const_cast<This*>(this)->getParam( name ); }

	/// The inheriting plugin instance needs to set this up to deal with
	/// plug-ins changing their own values.
	virtual void paramChangedByPlugin( OfxhParam* param ) = 0;

	/// add a param
	virtual void addParam( const std::string& name, OfxhParam* instance ) OFX_EXCEPTION_SPEC;

	/// make a parameter instance
	///
	/// Client host code needs to implement this
	virtual OfxhParam* newParam( const OfxhParamDescriptor& Descriptor ) OFX_EXCEPTION_SPEC = 0;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	///
	/// Client host code needs to implement this
	virtual void editBegin( const std::string& name ) OFX_EXCEPTION_SPEC = 0;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	///
	/// Client host code needs to implement this
	virtual void editEnd() OFX_EXCEPTION_SPEC = 0;

private:
	void initMapFromList();
};

/// a set of parameters
class OfxhParamDescriptorSet : public OfxhParamAccessorSet
{
typedef OfxhParamDescriptorSet This;
public:
	typedef std::map<std::string, OfxhParamDescriptor*> ParamDescriptorMap;
	typedef boost::ptr_list<OfxhParamDescriptor> ParamDescriptorList;
	ParamDescriptorMap _paramMap;
	ParamDescriptorList _paramList;

private:
/// CC doesn't exist
OfxhParamDescriptorSet( const OfxhParamDescriptorSet& );

public:
	/// default ctor
	OfxhParamDescriptorSet();

	/// dtor
	virtual ~OfxhParamDescriptorSet();

	bool operator==( const This& other ) const
	{
		if( _paramList != other._paramList )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }

	/// obtain a handle on this set for passing to the C api
	OfxParamSetHandle getParamSetHandle() const { return ( OfxParamSetHandle ) this; }

	/// get the map of params
	const ParamDescriptorMap& getParams() const { return _paramMap; }
	ParamDescriptorMap&       getParams()       { return _paramMap; }

	/// get the list of params
	const ParamDescriptorList& getParamList() const { return _paramList; }

	/// define a param
	virtual OfxhParamDescriptor* paramDefine( const char* paramType,
	                                          const char* name );

	/// add a param in
	virtual void addParam( const std::string& name, OfxhParamDescriptor* p );

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(_paramList);
	}
};

/// plugin parameter instance
class OfxhParam :
	public OfxhAttribute,
	protected property::OfxhNotifyHook,
	virtual public OfxhParamAccessor,
	private boost::noncopyable
{
OfxhParam();

protected:
	OfxhParamSet*  _paramSetInstance;
	OfxhParam*     _parentInstance;

protected:
	OfxhParam( const OfxhParam& other )
		: OfxhAttribute( other ),
		_paramSetInstance( other._paramSetInstance ),
		_parentInstance( other._parentInstance )
	{
		/// @todo tuttle : copy content, not pointer ?
	}

public:
	virtual ~OfxhParam() = 0;

	/// make a parameter, with the given type and name
	explicit OfxhParam( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance );

	/// clone this parameter
	virtual OfxhParam* clone() const = 0;
	/**
	 * @todo check values...
	 */
	bool operator==( const OfxhParam& p ) const { return true; }

	/// grab a handle on the parameter for passing to the C API
	OfxParamHandle getParamHandle() const
	{
		return ( OfxParamHandle ) this;
	}

	// void instanceChangedAction( std::string why,
	//                             OfxTime     time,
	//                             double      renderScaleX,
	//                             double      renderScaleY) OFX_EXCEPTION_SPEC;

	// get the param instance
	OfxhParamSet* getParamSetInstance()                         { return _paramSetInstance; }
	void          setParamSetInstance( OfxhParamSet* instance ) { _paramSetInstance = instance; }

	// set/get parent instance
	void       setParentInstance( OfxhParam* instance );
	OfxhParam* getParentInstance();

	// copy one parameter to another
	virtual void copy( const OfxhParam& instance ) OFX_EXCEPTION_SPEC = 0;

	// copy one parameter to another
	virtual void copy( const OfxhParam& instance, OfxTime offset ) OFX_EXCEPTION_SPEC;

	// copy one parameter to another, with a range
	virtual void copy( const OfxhParam& instance, OfxTime offset, OfxRangeD range ) OFX_EXCEPTION_SPEC;

	// callback which should set enabled state as appropriate
	virtual void setEnabled();

	// callback which should set secret state as appropriate
	virtual void setSecret();

	/// callback which should update label
	virtual void setLabel();

	/// callback which should set
	virtual void setDisplayRange();

	// va list calls below turn the var args (oh what a mistake)
	// suite functions into virtual function calls on instances
	// they are not to be overridden by host implementors by
	// by the various typeed param instances so that they can
	// deconstruct the var args lists

	virtual void set( const std::string& value ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a string parameter." ); }
	virtual void set( const int& value ) OFX_EXCEPTION_SPEC         { throw OfxhException( kOfxStatErrBadHandle, "Not an int parameter." ); }
	virtual void set( const double& value ) OFX_EXCEPTION_SPEC      { throw OfxhException( kOfxStatErrBadHandle, "Not a double parameter." ); }

	/// get a value, implemented by instances to deconstruct var args
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// get a value, implemented by instances to deconstruct var args
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// set a value, implemented by instances to deconstruct var args
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// key a value, implemented by instances to deconstruct var args
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// derive a value, implemented by instances to deconstruct var args
	virtual void deriveV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// integrate a value, implemented by instances to deconstruct var args
	virtual void integrateV( OfxTime time1, OfxTime time2, va_list arg ) OFX_EXCEPTION_SPEC;

	/// overridden from Property::NotifyHook
	virtual void notify( const std::string& name, bool single, int num ) OFX_EXCEPTION_SPEC;
};

/**
 * @brief to make ParamInstance clonable (for use in boost::ptr_container)
 */
inline OfxhParam* new_clone( const OfxhParam& a )
{
	return a.clone();
}

class OfxhKeyframeParam
{
public:
	virtual void getNumKeys( unsigned int& nKeys ) const OFX_EXCEPTION_SPEC;
	virtual void getKeyTime( int nth, OfxTime& time ) const OFX_EXCEPTION_SPEC;
	virtual void getKeyIndex( OfxTime time, int direction, int& index ) const OFX_EXCEPTION_SPEC;
	virtual void deleteKey( OfxTime time ) OFX_EXCEPTION_SPEC;
	virtual void deleteAllKeys() OFX_EXCEPTION_SPEC;

	virtual ~OfxhKeyframeParam() {}

};

template <class T, int DIM>
class OfxhMultiDimParam : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef T Type;
	typedef typename T::BaseType BaseType;

protected:
	std::vector<T*> _controls;

public:
	OfxhMultiDimParam( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	virtual ~OfxhMultiDimParam()
	{
		for( typename std::vector<T*>::iterator it = _controls.begin();
		     it != _controls.end(); ++it )
		{
			delete *it;
		}
	}

	// Deriving implementatation needs to overide these
	inline virtual void get( BaseType& dst, const std::size_t& index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->get( dst );
	}

	inline virtual void get( OfxTime time, BaseType& dst, const std::size_t& index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->get( time, dst );
	}

	inline virtual void set( const BaseType& value, const std::size_t& index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->set( value );
	}

	inline virtual void set( OfxTime time, const BaseType& value, const std::size_t& index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->set( time, value );
	}

	// derived class does not need to implement, default is an approximation
	inline virtual void derive( OfxTime time, BaseType& dst, const std::size_t& index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->derive( time, dst );
	}

	inline virtual void integrate( OfxTime time1, OfxTime time2, BaseType& dst, const std::size_t& index ) OFX_EXCEPTION_SPEC
	{
		assert( _controls.size() > index );
		_controls[index]->integrate( time1, time2, dst );
	}

	/// implementation of var args function
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[i]->get( *v );
		}
	}

	/// implementation of var args function
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[i]->get( time, *v );
		}
	}

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[i]->set( v );
		}
	}

	/// implementation of var args function
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType v = va_arg( arg, BaseType );
			_controls[i]->set( time, v );
		}
	}

	/// implementation of var args function
	virtual void deriveV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			_controls[i]->derive( time, *v );
		}
	}

	/// implementation of var args function
	virtual void integrateV( OfxTime time1, OfxTime time2, va_list arg ) OFX_EXCEPTION_SPEC
	{
		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			_controls[i]->integrate( time1, time2, *v );
		}
	}

};

class OfxhParamGroup : public OfxhParam,
	public OfxhParamSet
{
public:
	OfxhParamGroup( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}
	virtual ~OfxhParamGroup() {}

	void deleteChildrens()
	{
		_paramList.clear();
	}

	void                     setChildrens( const OfxhParamSet* childrens );
	OfxhParamSet* getChildrens() const;
	void                     addChildren( OfxhParam* children );

	property::OfxhSet& getParamSetProps()
	{
		return _paramSetInstance->getParamSetProps();
	}

	/// The inheriting plugin instance needs to set this up to deal with
	/// plug-ins changing their own values.
	virtual void paramChangedByPlugin( OfxhParam* param )
	{
		_paramSetInstance->paramChangedByPlugin( param );
	}

	virtual OfxhParam* newParam( const OfxhParamDescriptor& descriptor ) OFX_EXCEPTION_SPEC
	{
		return _paramSetInstance->newParam( descriptor );
	}

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	virtual void editBegin( const std::string& name ) OFX_EXCEPTION_SPEC
	{
		return _paramSetInstance->editBegin( name );
	}

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	virtual void editEnd() OFX_EXCEPTION_SPEC
	{
		return _paramSetInstance->editEnd();
	}

};

class OfxhParamPage : public OfxhParam
{
public:
	OfxhParamPage( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}
	const std::map<int, OfxhParam*>& getChildren() const;

protected:
	mutable std::map<int, OfxhParam*> _children; // if set in a notify hook, this need not be mutable
};

class OfxhParamInteger : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef int BaseType;
	OfxhParamInteger( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance )
		: OfxhParam( descriptor, setInstance )
	{
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMin, this );
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMax, this );
	}

	// Deriving implementatation needs to overide these
	virtual void get( int& )               OFX_EXCEPTION_SPEC = 0;
	virtual void get( OfxTime time, int& ) OFX_EXCEPTION_SPEC = 0;
	virtual void set( const int& )         OFX_EXCEPTION_SPEC = 0;
	virtual void set( OfxTime time, const int& ) OFX_EXCEPTION_SPEC = 0;

	// probably derived class does not need to implement, default is an approximation
	virtual void derive( OfxTime time, int& ) OFX_EXCEPTION_SPEC;
	virtual void integrate( OfxTime time1, OfxTime time2, int& ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void deriveV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void integrateV( OfxTime time1, OfxTime time2, va_list arg ) OFX_EXCEPTION_SPEC;
};

class OfxhParamChoice : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	OfxhParamChoice( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	// Deriving implementatation needs to overide these
	virtual void get( int& )               OFX_EXCEPTION_SPEC = 0;
	virtual void get( OfxTime time, int& ) OFX_EXCEPTION_SPEC = 0;
	virtual void set( int )                OFX_EXCEPTION_SPEC = 0;
	virtual void set( OfxTime time, int )  OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;
};

class OfxhParamDouble : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef double BaseType;
	OfxhParamDouble( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance )
		: OfxhParam( descriptor, setInstance )
	{
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMin, this );
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMax, this );
	}

	// Deriving implementatation needs to overide these
	virtual void get( double& )                                     OFX_EXCEPTION_SPEC = 0;
	virtual void get( OfxTime time, double& )                       OFX_EXCEPTION_SPEC = 0;
	virtual void set( const double& )                               OFX_EXCEPTION_SPEC = 0;
	virtual void set( OfxTime time, const double& )                 OFX_EXCEPTION_SPEC = 0;
	virtual void derive( OfxTime time, double& )                    OFX_EXCEPTION_SPEC = 0;
	virtual void integrate( OfxTime time1, OfxTime time2, double& ) OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void deriveV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void integrateV( OfxTime time1, OfxTime time2, va_list arg ) OFX_EXCEPTION_SPEC;
};

class OfxhParamBoolean : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef bool BaseType;
	OfxhParamBoolean( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	// Deriving implementatation needs to overide these
	virtual void get( bool& )               OFX_EXCEPTION_SPEC = 0;
	virtual void get( OfxTime time, bool& ) OFX_EXCEPTION_SPEC = 0;
	virtual void set( bool )                OFX_EXCEPTION_SPEC = 0;
	virtual void set( OfxTime time, bool )  OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;
};

class OfxhParamString : public OfxhParam,
	public OfxhKeyframeParam
{
std::string _returnValue; ///< location to hold temporary return value. Should delegate this to implementation!!!

public:
	typedef std::string BaseType;
	OfxhParamString( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	virtual void get( std::string& )               OFX_EXCEPTION_SPEC = 0;
	virtual void get( OfxTime time, std::string& ) OFX_EXCEPTION_SPEC = 0;
	virtual void set( const char* )                OFX_EXCEPTION_SPEC = 0;
	virtual void set( OfxTime time, const char* )  OFX_EXCEPTION_SPEC = 0;

	/// implementation of var args function
	virtual void getV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void getV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( va_list arg ) OFX_EXCEPTION_SPEC;

	/// implementation of var args function
	virtual void setV( OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC;
};

class OfxhParamCustom : public OfxhParamString
{
public:
	OfxhParamCustom( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParamString( descriptor, setInstance ) {}
};

class OfxhParamPushButton : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	OfxhParamPushButton( const OfxhParamDescriptor& descriptor, OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}
};

}
}
}
}

// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost{
template<> struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhAttribute, tuttle::host::ofx::attribute::OfxhParam>: public mpl::true_ {};
template<> struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhAttributeDescriptor, tuttle::host::ofx::attribute::OfxhParamDescriptor>: public mpl::true_ {};
}


#endif
