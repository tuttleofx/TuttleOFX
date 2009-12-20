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

#ifndef OFXH_PARAM_H
#define OFXH_PARAM_H

#include "ofxCore.h"
#include "OfxhPropertySuite.hpp"
#include "OfxhAttribute.hpp"
#include <tuttle/host/core/Exception.hpp>

#include <boost/ptr_container/ptr_list.hpp>

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

/// fetch the param suite
void* GetSuite( int version );

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
class OfxhParamAccessor : virtual public attribute::OfxhAttributeAccessor
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
class OfxhParamDescriptor : virtual public OfxhParamAccessor,
	public attribute::OfxhAttributeDescriptor
{
OfxhParamDescriptor();

public:
	/// make a parameter, with the given type and name
	OfxhParamDescriptor( const std::string& type, const std::string& name );

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

	/// add standard properties to a value holding param
	void addNumericParamProps( const std::string& type, property::TypeEnum valueType, int dim );
};

/// A set of parameters
///
/// As we are the owning object we delete the params inside ourselves. It was tempting
/// to make params autoref objects and have shared ownership with the client code
/// but that adds complexity for no strong gain.
class OfxhParamSet : public OfxhParamAccessorSet
{
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
			throw( std::logic_error( std::string( "Param not found. (" ) + name + ")" ) );
		return *it->second;

	}

	/// The inheriting plugin instance needs to set this up to deal with
	/// plug-ins changing their own values.
	virtual void paramChangedByPlugin( attribute::OfxhParam* param ) = 0;

	/// add a param
	virtual OfxStatus addParam( const std::string& name, OfxhParam* instance );

	/// make a parameter instance
	///
	/// Client host code needs to implement this
	virtual OfxhParam* newParam( OfxhParamDescriptor& Descriptor ) = 0;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	///
	/// Client host code needs to implement this
	virtual OfxStatus editBegin( const std::string& name ) = 0;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	///
	/// Client host code needs to implement this
	virtual OfxStatus editEnd() = 0;

private:
	void initMapFromList();
};

/// a set of parameters
class OfxhParamDescriptorSet : public OfxhParamAccessorSet
{
typedef std::map<std::string, OfxhParamDescriptor*> ParamDescriptorMap;
typedef std::list<OfxhParamDescriptor*> ParamDescriptorList;
ParamDescriptorMap _paramMap;
ParamDescriptorList _paramList;

/// CC doesn't exist
OfxhParamDescriptorSet( const OfxhParamDescriptorSet& );

public:
	/// default ctor
	OfxhParamDescriptorSet();

	/// dtor
	virtual ~OfxhParamDescriptorSet();

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
};

/// plugin parameter instance
class OfxhParam : virtual public OfxhParamAccessor,
	public OfxhAttribute,
	protected property::OfxhNotifyHook,
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

	//        OfxStatus instanceChangedAction(std::string why,
	//                                        OfxTime     time,
	//                                        double      renderScaleX,
	//                                        double      renderScaleY);

	// get the param instance
	OfxhParamSet* getParamSetInstance()                         { return _paramSetInstance; }
	void          setParamSetInstance( OfxhParamSet* instance ) { _paramSetInstance = instance; }

	// set/get parent instance
	void       setParentInstance( OfxhParam* instance );
	OfxhParam* getParentInstance();

	// copy one parameter to another
	virtual OfxStatus copy( const OfxhParam& instance, OfxTime offset );

	// copy one parameter to another, with a range
	virtual OfxStatus copy( const OfxhParam& instance, OfxTime offset, OfxRangeD range );

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

	virtual OfxStatus set( const std::string& value ) { throw core::exception::LogicError("Not a string parameter."); }
	virtual OfxStatus set( const int& value ) { throw core::exception::LogicError("Not an int parameter."); }
	virtual OfxStatus set( const double& value ) { throw core::exception::LogicError("Not a double parameter."); }

	/// get a value, implemented by instances to deconstruct var args
	virtual OfxStatus getV( va_list arg );

	/// get a value, implemented by instances to deconstruct var args
	virtual OfxStatus getV( OfxTime time, va_list arg );

	/// set a value, implemented by instances to deconstruct var args
	virtual OfxStatus setV( va_list arg );

	/// key a value, implemented by instances to deconstruct var args
	virtual OfxStatus setV( OfxTime time, va_list arg );

	/// derive a value, implemented by instances to deconstruct var args
	virtual OfxStatus deriveV( OfxTime time, va_list arg );

	/// integrate a value, implemented by instances to deconstruct var args
	virtual OfxStatus integrateV( OfxTime time1, OfxTime time2, va_list arg );

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
	virtual OfxStatus getNumKeys( unsigned int& nKeys ) const ;
	virtual OfxStatus getKeyTime( int nth, OfxTime& time ) const ;
	virtual OfxStatus getKeyIndex( OfxTime time, int direction, int& index ) const ;
	virtual OfxStatus deleteKey( OfxTime time ) ;
	virtual OfxStatus deleteAllKeys() ;

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
	OfxhMultiDimParam( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	virtual ~OfxhMultiDimParam()
	{
		for( typename std::vector<T*>::iterator it = _controls.begin();
		     it != _controls.end(); ++it )
		{
			delete *it;
		}
	}

	// Deriving implementatation needs to overide these
	inline virtual OfxStatus get( BaseType& dst, const size_t& index )
	{
		assert( _controls.size() > index );
		return _controls[index]->get( dst );
	}

	inline virtual OfxStatus get( OfxTime time, BaseType& dst, const size_t& index )
	{
		assert( _controls.size() > index );
		return _controls[index]->get( time, dst );
	}

	inline virtual OfxStatus set( const BaseType& value, const size_t& index )
	{
		assert( _controls.size() > index );
		return _controls[index]->set( value );
	}

	inline virtual OfxStatus set( OfxTime time, const BaseType& value, const size_t& index )
	{
		assert( _controls.size() > index );
		return _controls[index]->set( time, value );
	}

	// derived class does not need to implement, default is an approximation
	inline virtual OfxStatus derive( OfxTime time, BaseType& dst, const size_t& index )
	{
		assert( _controls.size() > index );
		return _controls[index]->derive( time, dst );
	}

	inline virtual OfxStatus integrate( OfxTime time1, OfxTime time2, BaseType& dst, const size_t& index )
	{
		assert( _controls.size() > index );
		return _controls[index]->integrate( time1, time2, dst );
	}

	/// implementation of var args function
	virtual OfxStatus getV( va_list arg )
	{
		OfxStatus st = kOfxStatOK;

		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			st |= _controls[i]->get( *v );
		}
		return st;
	}

	/// implementation of var args function
	virtual OfxStatus getV( OfxTime time, va_list arg )
	{
		OfxStatus st = kOfxStatOK;

		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			st |= _controls[i]->get( time, *v );
		}
		return st;
	}

	/// implementation of var args function
	virtual OfxStatus setV( va_list arg )
	{
		OfxStatus st = kOfxStatOK;

		for( int i = 0; i < DIM; ++i )
		{
			BaseType v = va_arg( arg, BaseType );
			st |= _controls[i]->set( v );
		}
		return st;
	}

	/// implementation of var args function
	virtual OfxStatus setV( OfxTime time, va_list arg )
	{
		OfxStatus st = kOfxStatOK;

		for( int i = 0; i < DIM; ++i )
		{
			BaseType v = va_arg( arg, BaseType );
			st |= _controls[i]->set( time, v );
		}
		return st;
	}

	/// implementation of var args function
	virtual OfxStatus deriveV( OfxTime time, va_list arg )
	{
		OfxStatus st = kOfxStatOK;

		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			st |= _controls[i]->derive( time, *v );
		}
		return st;
	}

	/// implementation of var args function
	virtual OfxStatus integrateV( OfxTime time1, OfxTime time2, va_list arg )
	{
		OfxStatus st = kOfxStatOK;

		for( int i = 0; i < DIM; ++i )
		{
			BaseType* v = va_arg( arg, BaseType* );
			assert( v );
			st |= _controls[i]->integrate( time1, time2, *v );
		}
		return st;
	}

};

class OfxhParamGroup : public OfxhParam,
	public OfxhParamSet
{
public:
	OfxhParamGroup( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}
	virtual ~OfxhParamGroup() {}
	virtual OfxhParamGroup* clone() const;

	void deleteChildrens()
	{
		_paramList.clear();
	}

	void                     setChildrens( const attribute::OfxhParamSet* childrens );
	attribute::OfxhParamSet* getChildrens() const;
	void                     addChildren( OfxhParam* children );

	property::OfxhSet& getParamSetProps()
	{
		return _paramSetInstance->getParamSetProps();
	}

	/// The inheriting plugin instance needs to set this up to deal with
	/// plug-ins changing their own values.
	virtual void paramChangedByPlugin( attribute::OfxhParam* param )
	{
		_paramSetInstance->paramChangedByPlugin( param );
	}

	virtual OfxhParam* newParam( OfxhParamDescriptor& descriptor )
	{
		return _paramSetInstance->newParam( descriptor );
	}

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	virtual OfxStatus editBegin( const std::string& name )
	{
		return _paramSetInstance->editBegin( name );
	}

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	virtual OfxStatus editEnd()
	{
		return _paramSetInstance->editEnd();
	}

};

class OfxhParamPage : public OfxhParam
{
public:
	OfxhParamPage( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}
	virtual OfxhParamPage*                  clone() const;
	const std::map<int, attribute::OfxhParam*>& getChildren() const;

protected:
	mutable std::map<int, attribute::OfxhParam*> _children; // if set in a notify hook, this need not be mutable
};

class OfxhParamInteger : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef int BaseType;
	OfxhParamInteger( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance )
	: OfxhParam( descriptor, setInstance )
	{
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMin, this );
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMax, this );
	}

	// Deriving implementatation needs to overide these
	virtual OfxStatus get( int& )               = 0;
	virtual OfxStatus get( OfxTime time, int& ) = 0;
	virtual OfxStatus set( int )                = 0;
	virtual OfxStatus set( OfxTime time, int )  = 0;

	// probably derived class does not need to implement, default is an approximation
	virtual OfxStatus derive( OfxTime time, int& );
	virtual OfxStatus integrate( OfxTime time1, OfxTime time2, int& );

	/// implementation of var args function
	virtual OfxStatus getV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus getV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus deriveV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus integrateV( OfxTime time1, OfxTime time2, va_list arg );
};

class OfxhParamChoice : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	OfxhParamChoice( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	// Deriving implementatation needs to overide these
	virtual OfxStatus get( int& )               = 0;
	virtual OfxStatus get( OfxTime time, int& ) = 0;
	virtual OfxStatus set( int )                = 0;
	virtual OfxStatus set( OfxTime time, int )  = 0;

	/// implementation of var args function
	virtual OfxStatus getV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus getV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( OfxTime time, va_list arg );
};

class OfxhParamDouble : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef double BaseType;
	OfxhParamDouble( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance )
	: OfxhParam( descriptor, setInstance )
	{
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMin, this );
		getEditableProperties().addNotifyHook( kOfxParamPropDisplayMax, this );
	}

	// Deriving implementatation needs to overide these
	virtual OfxStatus get( double& )                                     = 0;
	virtual OfxStatus get( OfxTime time, double& )                       = 0;
	virtual OfxStatus set( double )                                      = 0;
	virtual OfxStatus set( OfxTime time, double )                        = 0;
	virtual OfxStatus derive( OfxTime time, double& )                    = 0;
	virtual OfxStatus integrate( OfxTime time1, OfxTime time2, double& ) = 0;

	/// implementation of var args function
	virtual OfxStatus getV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus getV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus deriveV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus integrateV( OfxTime time1, OfxTime time2, va_list arg );
};

class OfxhParamBoolean : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	typedef bool BaseType;
	OfxhParamBoolean( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	// Deriving implementatation needs to overide these
	virtual OfxStatus get( bool& )               = 0;
	virtual OfxStatus get( OfxTime time, bool& ) = 0;
	virtual OfxStatus set( bool )                = 0;
	virtual OfxStatus set( OfxTime time, bool )  = 0;

	/// implementation of var args function
	virtual OfxStatus getV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus getV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( OfxTime time, va_list arg );
};

class OfxhParamString : public OfxhParam,
	public OfxhKeyframeParam
{
std::string _returnValue; ///< location to hold temporary return value. Should delegate this to implementation!!!

public:
	typedef std::string BaseType;
	OfxhParamString( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}

	virtual OfxStatus get( std::string& )               = 0;
	virtual OfxStatus get( OfxTime time, std::string& ) = 0;
	virtual OfxStatus set( const char* )                = 0;
	virtual OfxStatus set( OfxTime time, const char* )  = 0;

	/// implementation of var args function
	virtual OfxStatus getV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus getV( OfxTime time, va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( va_list arg );

	/// implementation of var args function
	virtual OfxStatus setV( OfxTime time, va_list arg );
};

class OfxhParamCustom : public OfxhParamString
{
public:
	OfxhParamCustom( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParamString( descriptor, setInstance ) {}
};

class OfxhParamPushButton : public OfxhParam,
	public OfxhKeyframeParam
{
public:
	OfxhParamPushButton( OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance ) : OfxhParam( descriptor, setInstance ) {}
};

}
}
}
}

#endif
