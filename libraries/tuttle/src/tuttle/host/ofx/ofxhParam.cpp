/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
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

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhParam.h"
#include "ofxhImageEffect.h"

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

ParamAccessor::ParamAccessor()
{}

ParamAccessor::~ParamAccessor() {}

const std::string& ParamAccessor::getParamType() const
{
	return getProperties().getStringProperty( kOfxParamPropType );
}

const std::string& ParamAccessor::getParentName() const
{
	return getProperties().getStringProperty( kOfxParamPropParent );
}

const std::string& ParamAccessor::getScriptName() const
{
	return getProperties().getStringProperty( kOfxParamPropScriptName );
}

const std::string& ParamAccessor::getHint() const
{
	return getProperties().getStringProperty( kOfxParamPropHint, 0 );
}

const std::string& ParamAccessor::getDoubleType() const
{
	return getProperties().getStringProperty( kOfxParamPropDoubleType, 0 );
}

const std::string& ParamAccessor::getCacheInvalidation() const
{
	return getProperties().getStringProperty( kOfxParamPropCacheInvalidation, 0 );
}

bool ParamAccessor::getEnabled() const
{
	return getProperties().getIntProperty( kOfxParamPropEnabled, 0 ) != 0;
}

bool ParamAccessor::getSecret() const
{
	return getProperties().getIntProperty( kOfxParamPropSecret, 0 ) != 0;
}

bool ParamAccessor::getEvaluateOnChange() const
{
	return getProperties().getIntProperty( kOfxParamPropEvaluateOnChange, 0 ) != 0;
}

bool ParamAccessor::getCanUndo() const
{
	if( getProperties().fetchProperty( kOfxParamPropCanUndo ) )
	{
		return getProperties().getIntProperty( kOfxParamPropCanUndo ) != 0;
	}
	return false;
}

bool ParamAccessor::getCanAnimate() const
{
	if( getProperties().fetchProperty( kOfxParamPropAnimates ) )
	{
		return getProperties().getIntProperty( kOfxParamPropAnimates ) != 0;
	}
	return false;
}

//
// Descriptor
//

struct TypeMap
{

	const char* paramType;
	Property::TypeEnum propType;
	int propDimension;
};

bool isDoubleParam( const std::string& paramType )
{
	return paramType == kOfxParamTypeDouble ||
	       paramType == kOfxParamTypeDouble2D ||
	       paramType == kOfxParamTypeDouble3D;
}

bool isColourParam( const std::string& paramType )
{
	return
	    paramType == kOfxParamTypeRGBA ||
	    paramType == kOfxParamTypeRGB;
}

bool isIntParam( const std::string& paramType )
{
	return paramType == kOfxParamTypeInteger ||
	       paramType == kOfxParamTypeInteger2D ||
	       paramType == kOfxParamTypeInteger3D;
}

static TypeMap typeMap[] = {
	{ kOfxParamTypeInteger, Property::eInt, 1 },
	{ kOfxParamTypeDouble, Property::eDouble, 1 },
	{ kOfxParamTypeBoolean, Property::eInt, 1 },
	{ kOfxParamTypeChoice, Property::eInt, 1 },
	{ kOfxParamTypeRGBA, Property::eDouble, 4 },
	{ kOfxParamTypeRGB, Property::eDouble, 3 },
	{ kOfxParamTypeDouble2D, Property::eDouble, 2 },
	{ kOfxParamTypeInteger2D, Property::eInt, 2 },
	{ kOfxParamTypeDouble3D, Property::eDouble, 3 },
	{ kOfxParamTypeInteger3D, Property::eInt, 3 },
	{ kOfxParamTypeString, Property::eString, 1 },
	{ kOfxParamTypeCustom, Property::eString, 1 },
	{ kOfxParamTypeGroup, Property::eNone },
	{ kOfxParamTypePage, Property::eNone },
	{ kOfxParamTypePushButton, Property::eNone },
	{ 0 }
};

/// is this a standard type

bool isStandardType( const std::string& type )
{
	TypeMap* tm = typeMap;

	while( tm->paramType )
	{
		if( tm->paramType == type )
			return true;
		tm++;
	}
	return false;
}

bool findType( const std::string paramType, Property::TypeEnum& propType, int& propDim )
{
	TypeMap* tm = typeMap;

	while( tm->paramType )
	{
		if( tm->paramType == paramType )
		{
			propType = tm->propType;
			propDim  = tm->propDimension;
			return true;
		}
		tm++;
	}
	return false;
}

/**
 * @brief make a parameter, with the given type and name
 */
ParamDescriptor::ParamDescriptor( const std::string& type, const std::string& name )
	: attribute::AttributeDescriptor( Property::Set() )
{
	const char* ctype = type.c_str();
	const char* cname = name.c_str();

	static const Property::PropSpec paramDescriptorProps[] = {
		{ kOfxPropType, Property::eString, 1, true, kOfxTypeParameter },
		{ kOfxParamPropSecret, Property::eInt, 1, false, "0" },
		{ kOfxParamPropHint, Property::eString, 1, false, "" },
		{ kOfxParamPropParent, Property::eString, 1, false, "" },
		{ kOfxParamPropEnabled, Property::eInt, 1, false, "1" },
		{ kOfxParamPropDataPtr, Property::ePointer, 1, false, 0 },
		{ 0 }
	};

	const Property::PropSpec dynamicParamDescriptorProps[] = {
		{ kOfxParamPropType, Property::eString, 1, true, ctype },
		{ kOfxParamPropScriptName, Property::eString, 1, false, cname }, ///< @todo TUTTLE_TODO : common property for all Attributes
		{ 0 }
	};

	getEditableProperties().addProperties( paramDescriptorProps );
	getEditableProperties().addProperties( dynamicParamDescriptorProps );

	setAllNames( name );

	getEditableProperties().setStringProperty( kOfxParamPropType, type );
	assert( ctype );
}

/// make a parameter, with the given type and name

void ParamDescriptor::addStandardParamProps( const std::string& type )
{
	Property::TypeEnum propType = Property::eString;
	int propDim                 = 1;

	findType( type, propType, propDim );

	static Property::PropSpec allString[] = {
		{ kOfxParamPropStringMode, Property::eString, 1, false, kOfxParamStringIsSingleLine },
		{ kOfxParamPropStringFilePathExists, Property::eInt, 1, false, "1" },
		{ 0 }
	};

	static Property::PropSpec allChoice[] = {
		{ kOfxParamPropChoiceOption, Property::eString, 0, false, "" },
		{ 0 }
	};

	static Property::PropSpec allCustom[] = {
		{ kOfxParamPropCustomInterpCallbackV1, Property::ePointer, 1, false, 0 },
		{ 0 },
	};

	static Property::PropSpec allPage[] = {
		{ kOfxParamPropPageChild, Property::eString, 0, false, "" },
		{ 0 }
	};

	if( propType != Property::eNone )
	{
		addValueParamProps( type, propType, propDim );
	}

	if( type == kOfxParamTypeString )
	{
		getEditableProperties().addProperties( allString );
	}

	if( isDoubleParam( type ) || isIntParam( type ) || isColourParam( type ) )
	{
		addNumericParamProps( type, propType, propDim );
	}

	if( type != kOfxParamTypeGroup && type != kOfxParamTypePage )
	{
		addInteractParamProps( type );
	}

	if( type == kOfxParamTypeChoice )
	{
		getEditableProperties().addProperties( allChoice );
	}

	if( type == kOfxParamTypeCustom )
	{
		getEditableProperties().addProperties( allCustom );
	}

	if( type == kOfxParamTypePage )
	{
		getEditableProperties().addProperties( allPage );
	}
}

/// add standard properties to a params that can take an interact

void ParamDescriptor::addInteractParamProps( const std::string& type )
{
	static Property::PropSpec allButGroupPageProps[] = {
		{ kOfxParamPropInteractV1, Property::ePointer, 1, false, 0 },
		{ kOfxParamPropInteractSize, Property::eDouble, 2, false, "0" },
		{ kOfxParamPropInteractSizeAspect, Property::eDouble, 1, false, "1" },
		{ kOfxParamPropInteractMinimumSize, Property::eDouble, 2, false, "10" },
		{ kOfxParamPropInteractPreferedSize, Property::eInt, 2, false, "10" },
		{ 0 }
	};

	getEditableProperties().addProperties( allButGroupPageProps );
}

/// add standard properties to a value holding param

void ParamDescriptor::addValueParamProps( const std::string& type, Property::TypeEnum valueType, int dim )
{
	static Property::PropSpec invariantProps[] = {
		{ kOfxParamPropAnimates, Property::eInt, 1, false, "1" },
		{ kOfxParamPropIsAnimating, Property::eInt, 1, false, "0" },
		{ kOfxParamPropIsAutoKeying, Property::eInt, 1, false, "0" },
		{ kOfxParamPropPersistant, Property::eInt, 1, false, "1" },
		{ kOfxParamPropEvaluateOnChange, Property::eInt, 1, false, "1" },
		{ kOfxParamPropPluginMayWrite, Property::eInt, 1, false, "0" },
		{ kOfxParamPropCanUndo, Property::eInt, 1, false, "1" },
		{ kOfxParamPropCacheInvalidation, Property::eString, 1, false, kOfxParamInvalidateValueChange },
		{ 0 }
	};

	Property::PropSpec variantProps[] = {
		{ kOfxParamPropDefault, valueType, dim, false, valueType == Property::eString ? "" : "0" },
		{ 0 }
	};

	getEditableProperties().addProperties( invariantProps );
	getEditableProperties().addProperties( variantProps );
}

/// add standard properties to a value holding param

void ParamDescriptor::addNumericParamProps( const std::string& type, Property::TypeEnum valueType, int dim )
{
	static std::string dbl_minstr, dbl_maxstr, int_minstr, int_maxstr;
	bool doneOne = false;

	if( !doneOne )
	{
		std::ostringstream dbl_min, dbl_max, int_min, int_max;
		doneOne = true;
		dbl_min << -DBL_MAX;
		dbl_max << DBL_MAX;
		int_min << INT_MIN;
		int_max << INT_MAX;

		dbl_minstr = dbl_min.str();
		dbl_maxstr = dbl_max.str();
		int_minstr = int_min.str();
		int_maxstr = int_max.str();
	}

	Property::PropSpec allNumeric[] = {
		{ kOfxParamPropDisplayMin, valueType, dim, false, isColourParam( type ) ? "0" : ( valueType == Property::eDouble ? dbl_minstr : int_minstr ).c_str() },
		{ kOfxParamPropDisplayMax, valueType, dim, false, isColourParam( type ) ? "1" : ( valueType == Property::eDouble ? dbl_maxstr : int_maxstr ).c_str() },
		{ kOfxParamPropMin, valueType, dim, false, ( valueType == Property::eDouble ? dbl_minstr : int_minstr ).c_str() },
		{ kOfxParamPropMax, valueType, dim, false, ( valueType == Property::eDouble ? dbl_maxstr : int_maxstr ).c_str() },
		{ 0 }
	};

	getEditableProperties().addProperties( allNumeric );

	/// if any double or a colour
	if( valueType == Property::eDouble )
	{
		static Property::PropSpec allDouble[] = {
			{ kOfxParamPropIncrement, Property::eDouble, 1, false, "1" },
			{ kOfxParamPropDigits, Property::eInt, 1, false, "2" },
			{ 0 }
		};
		getEditableProperties().addProperties( allDouble );
	}

	/// if a double param type
	if( isDoubleParam( type ) )
	{
		static Property::PropSpec allDouble[] = {
			{ kOfxParamPropDoubleType, Property::eString, 1, false, kOfxParamDoubleTypePlain },
			{ 0 }
		};
		getEditableProperties().addProperties( allDouble );

		if( dim == 1 )
		{
			static Property::PropSpec allDouble1D[] = {
				{ kOfxParamPropShowTimeMarker, Property::eInt, 1, false, "0" },
				{ 0 }
			};

			getEditableProperties().addProperties( allDouble1D );
		}
	}

	/// if a multi dimensional param
	if( isDoubleParam( type ) && ( dim == 2 || dim == 3 ) )
	{
		Property::PropSpec all2D3D[] = {
			{ kOfxParamPropDimensionLabel, Property::eString, dim, false, "" },
			{ 0 },
		};

		getEditableProperties().addProperties( all2D3D );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "X", 0 );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "Y", 1 );
		if( dim == 3 )
		{
			getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "Z", 2 );
		}
	}

	/// if a multi dimensional param
	if( isColourParam( type ) )
	{
		Property::PropSpec allColor[] = {
			{ kOfxParamPropDimensionLabel, Property::eString, dim, false, "" },
			{ 0 },
		};

		getEditableProperties().addProperties( allColor );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "R", 0 );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "G", 1 );
		getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "B", 2 );
		if( dim == 4 )
		{
			getEditableProperties().setStringProperty( kOfxParamPropDimensionLabel, "A", 3 );
		}
	}
}

ParamAccessorSet::~ParamAccessorSet() {}

/// obtain a handle on this set for passing to the C api

ParamDescriptorSet::ParamDescriptorSet() {}

ParamDescriptorSet::~ParamDescriptorSet()
{
	for( std::list<ParamDescriptor*>::iterator it = _paramList.begin(); it != _paramList.end(); ++it )
	{
		delete ( *it );
	}
}

const std::map<std::string, ParamDescriptor*>& ParamDescriptorSet::getParams() const
{
	return _paramMap;
}

std::map<std::string, ParamDescriptor*>& ParamDescriptorSet::getParams()
{
	return _paramMap;
}

const std::list<ParamDescriptor*>& ParamDescriptorSet::getParamList() const
{
	return _paramList;
}

void ParamDescriptorSet::addParam( const std::string& name, ParamDescriptor* p )
{
	_paramList.push_back( p );
	_paramMap[name] = p;
}

/// define a param on this effect

ParamDescriptor* ParamDescriptorSet::paramDefine( const char* paramType,
                                                  const char* name )
{
	if( !isStandardType( paramType ) )
		return NULL;                                                       /// << EEK! This is bad.

	ParamDescriptor* desc = new ParamDescriptor( paramType, name );
	desc->addStandardParamProps( paramType );
	addParam( name, desc );
	return desc;
}

////////////////////////////////////////////////////////////////////////////////
//
// Instance
//
/// the description of a plugin parameter

ParamInstance::~ParamInstance() {}

/// make a parameter, with the given type and name

ParamInstance::ParamInstance( ParamDescriptor& descriptor, attribute::ParamInstanceSet& setInstance )
	: attribute::AttributeInstance( descriptor ),
	_paramSetInstance( &setInstance ),
	_parentInstance( 0 )
{
	// parameter has to be owned by paramSet
	setInstance.addParam( descriptor.getName(), this );

	getProperties().addNotifyHook( kOfxParamPropEnabled, this );
	getProperties().addNotifyHook( kOfxParamPropSecret, this );
	getProperties().addNotifyHook( kOfxPropLabel, this );
	getProperties().addNotifyHook( kOfxParamPropDisplayMin, this );
	getProperties().addNotifyHook( kOfxParamPropDisplayMax, this );
}

// callback which should set enabled state as appropriate

void ParamInstance::setEnabled() {}

// callback which should set secret state as appropriate

void ParamInstance::setSecret() {}

// callback which should update label

void ParamInstance::setLabel() {}

/// callback which should set

void ParamInstance::setDisplayRange() {}

/// get a value, implemented by instances to deconstruct var args

OfxStatus ParamInstance::getV( va_list arg )
{
	COUT_WITHINFOS( "ParamInstance getValue failed !" );
	COUT( "(paramName:" << getName() << ")" );
	return kOfxStatErrUnsupported;
}

/// get a value, implemented by instances to deconstruct var args

OfxStatus ParamInstance::getV( OfxTime time, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/// set a value, implemented by instances to deconstruct var args

OfxStatus ParamInstance::setV( va_list arg )
{
	return kOfxStatErrUnsupported;
}

/// key a value, implemented by instances to deconstruct var args

OfxStatus ParamInstance::setV( OfxTime time, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/// derive a value, implemented by instances to deconstruct var args

OfxStatus ParamInstance::deriveV( OfxTime time, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/// integrate a value, implemented by instances to deconstruct var args

OfxStatus ParamInstance::integrateV( OfxTime time1, OfxTime time2, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/// overridden from Property::NotifyHook

void ParamInstance::notify( const std::string& name, bool single, int num ) OFX_EXCEPTION_SPEC
{
	if( name == kOfxPropLabel )
	{
		setLabel();
	}
	if( name == kOfxParamPropEnabled )
	{
		setEnabled();
	}
	if( name == kOfxParamPropSecret )
	{
		setSecret();
	}
	if( name == kOfxParamPropDisplayMin || name == kOfxParamPropDisplayMax )
	{
		setDisplayRange();
	}
}

// copy one parameter to another

OfxStatus ParamInstance::copy( const ParamInstance& instance, OfxTime offset )
{
	return kOfxStatErrMissingHostFeature;
}

// copy one parameter to another, with a range

OfxStatus ParamInstance::copy( const ParamInstance& instance, OfxTime offset, OfxRangeD range )
{
	return kOfxStatErrMissingHostFeature;
}

void ParamInstance::setParentInstance( ParamInstance* instance )
{
	_parentInstance = instance;
}

ParamInstance* ParamInstance::getParentInstance()
{
	return _parentInstance;
}

//
// KeyframeParam
//

OfxStatus KeyframeParam::getNumKeys( unsigned int& nKeys ) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus KeyframeParam::getKeyTime( int nth, OfxTime& time ) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus KeyframeParam::getKeyIndex( OfxTime time, int direction, int& index ) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus KeyframeParam::deleteKey( OfxTime time )
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus KeyframeParam::deleteAllKeys()
{
	return kOfxStatErrMissingHostFeature;
}

ParamGroupInstance* ParamGroupInstance::clone() const
{
	return new ParamGroupInstance( *this );
}

/// setChildrens have to clone each source instance recursively

void ParamGroupInstance::setChildrens( const ParamInstanceSet* childrens )
{
	std::list<ParamInstance*> srcList = childrens->getParamList();
	// iterate the params and delete them
	deleteChildrens();

	/// @todo use clone ?
	//for( it = srcList.begin( ); it != srcList.end( ); ++it )
	//{
	//	_paramList.push_back( (*it)->clone(this) );
	//}
}

void ParamGroupInstance::addChildren( ParamInstance* children )
{
	children->setParamSetInstance( this );
	_paramList.push_back( children );
}

ParamInstanceSet* ParamGroupInstance::getChildrens() const
{
	return (ParamInstanceSet*)this;
}

//
// Page Instance
//
ParamPageInstance* ParamPageInstance::clone() const
{
	return new ParamPageInstance( *this );
}

const std::map<int, attribute::ParamInstance*>& ParamPageInstance::getChildren() const
{
	// HACK!!!! this really should be done with a notify hook so we don't force
	// _children to be mutable
	if( _children.size() == 0 )
	{
		int nChildren = getProperties().getDimension( kOfxParamPropPageChild );
		for( int i = 0; i < nChildren; i++ )
		{
			std::string childName           = getProperties().getStringProperty( kOfxParamPropPageChild, i );
			attribute::ParamInstance* child = _paramSetInstance->getParam( childName );
			if( child )
				_children[i] = child;
		}
	}
	return _children;
}

//
// ChoiceInstance
//
/// implementation of var args function

OfxStatus ParamChoiceInstance::getV( va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( *value );
}

/// implementation of var args function

OfxStatus ParamChoiceInstance::getV( OfxTime time, va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( time, *value );
}

/// implementation of var args function

OfxStatus ParamChoiceInstance::setV( va_list arg )
{
	int value = va_arg( arg, int );

	return set( value );
}

/// implementation of var args function

OfxStatus ParamChoiceInstance::setV( OfxTime time, va_list arg )
{
	int value = va_arg( arg, int );

	return set( time, value );
}

//
// IntegerInstance
//

OfxStatus ParamIntegerInstance::derive( OfxTime time, int& )
{
	return kOfxStatErrUnsupported;
}

OfxStatus ParamIntegerInstance::integrate( OfxTime time1, OfxTime time2, int& )
{
	return kOfxStatErrUnsupported;
}

/// implementation of var args function

OfxStatus ParamIntegerInstance::getV( va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( *value );
}

/// implementation of var args function

OfxStatus ParamIntegerInstance::getV( OfxTime time, va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( time, *value );
}

/// implementation of var args function

OfxStatus ParamIntegerInstance::setV( va_list arg )
{
	int value = va_arg( arg, int );

	return set( value );
}

/// implementation of var args function

OfxStatus ParamIntegerInstance::setV( OfxTime time, va_list arg )
{
	int value = va_arg( arg, int );

	return set( time, value );
}

/// implementation of var args function

OfxStatus ParamIntegerInstance::deriveV( OfxTime time, va_list arg )
{
	int* value = va_arg( arg, int* );

	return derive( time, *value );
}

/// implementation of var args function

OfxStatus ParamIntegerInstance::integrateV( OfxTime time1, OfxTime time2, va_list arg )
{
	int* value = va_arg( arg, int* );

	return integrate( time1, time2, *value );
}

//
// DoubleInstance
//
/// implementation of var args function

OfxStatus ParamDoubleInstance::getV( va_list arg )
{
	double* value = va_arg( arg, double* );

	return get( *value );
}

/// implementation of var args function

OfxStatus ParamDoubleInstance::getV( OfxTime time, va_list arg )
{
	double* value = va_arg( arg, double* );

	return get( time, *value );
}

/// implementation of var args function

OfxStatus ParamDoubleInstance::setV( va_list arg )
{
	double value = va_arg( arg, double );

	return set( value );
}

/// implementation of var args function

OfxStatus ParamDoubleInstance::setV( OfxTime time, va_list arg )
{
	double value = va_arg( arg, double );

	return set( time, value );
}

/// implementation of var args function

OfxStatus ParamDoubleInstance::deriveV( OfxTime time, va_list arg )
{
	double* value = va_arg( arg, double* );

	return derive( time, *value );
}

/// implementation of var args function

OfxStatus ParamDoubleInstance::integrateV( OfxTime time1, OfxTime time2, va_list arg )
{
	double* value = va_arg( arg, double* );

	return integrate( time1, time2, *value );
}

//
// BooleanInstance
//
/// implementation of var args function

OfxStatus ParamBooleanInstance::getV( va_list arg )
{
	bool v;
	OfxStatus stat = get( v );

	int* value = va_arg( arg, int* );

	*value = v;
	return stat;
}

/// implementation of var args function

OfxStatus ParamBooleanInstance::getV( OfxTime time, va_list arg )
{
	bool v;
	OfxStatus stat = get( time, v );

	int* value = va_arg( arg, int* );

	*value = v;
	return stat;
}

/// implementation of var args function

OfxStatus ParamBooleanInstance::setV( va_list arg )
{
	bool value = va_arg( arg, int ) != 0;

	return set( value );
}

/// implementation of var args function

OfxStatus ParamBooleanInstance::setV( OfxTime time, va_list arg )
{
	bool value = va_arg( arg, int ) != 0;

	return set( time, value );
}

////////////////////////////////////////////////////////////////////////////////
// string param

OfxStatus ParamStringInstance::getV( va_list arg )
{
	const char** value = va_arg( arg, const char** );
	OfxStatus stat     = get( _returnValue ); // I so don't like this, temp storage should be delegated to the implementation

	*value = _returnValue.c_str();
	return stat;
}

/// implementation of var args function

OfxStatus ParamStringInstance::getV( OfxTime time, va_list arg )
{
	const char** value = va_arg( arg, const char** );

	OfxStatus stat = get( time, _returnValue ); // I so don't like this, temp storage should be delegated to the implementation

	*value = _returnValue.c_str();
	return stat;
}

/// implementation of var args function

OfxStatus ParamStringInstance::setV( va_list arg )
{
	char* value = va_arg( arg, char* );

	return set( value );
}

/// implementation of var args function

OfxStatus ParamStringInstance::setV( OfxTime time, va_list arg )
{
	char* value = va_arg( arg, char* );

	return set( time, value );
}

//////////////////////////////////////////////////////////////////////////////////
// ParamInstanceSet
//

/// ctor
ParamInstanceSet::ParamInstanceSet() {}

/// dtor.

ParamInstanceSet::~ParamInstanceSet()
{
	// iterate the params and delete them
	std::list<ParamInstance*>::iterator i;
	for( i = _paramList.begin(); i != _paramList.end(); ++i )
	{
		if( *i )
			delete ( *i );
	}
}

OfxStatus ParamInstanceSet::addParam( const std::string& name, ParamInstance* instance )
{
	if( _params.find( name ) == _params.end() )
	{
		_params[name] = instance;
		_paramList.push_back( instance );
	}
	else
		return kOfxStatErrExists;

	return kOfxStatOK;
}

////////////////////////////////////////////////////////////////////////////////
// Suite functions below

static OfxStatus paramDefine( OfxParamSetHandle     paramSet,
                              const char*           paramType,
                              const char*           name,
                              OfxPropertySetHandle* propertySet )
{
	ParamDescriptorSet* paramSetDescriptor = reinterpret_cast<ParamDescriptorSet*>( paramSet );

	if( !paramSetDescriptor )
	{
		return kOfxStatErrBadHandle;
	}

	ParamDescriptor* desc = paramSetDescriptor->paramDefine( paramType, name );

	if( desc )
	{
		*propertySet = desc->getPropHandle();
		return kOfxStatOK;
	}
	else
		return kOfxStatErrUnsupported;

	return kOfxStatOK;
}

static OfxStatus paramGetHandle( OfxParamSetHandle     paramSet,
                                 const char*           name,
                                 OfxParamHandle*       param,
                                 OfxPropertySetHandle* propertySet )
{

	ParamAccessorSet* baseSet = reinterpret_cast<ParamAccessorSet*>( paramSet );

	if( !baseSet )
	{
		return kOfxStatErrBadHandle;
	}

	ParamInstanceSet* setInstance = dynamic_cast<ParamInstanceSet*>( baseSet );

	if( setInstance )
	{
		const std::map<std::string, ParamInstance*>& params      = setInstance->getParams();
		std::map<std::string, ParamInstance*>::const_iterator it = params.find( name );

		// if we can't find it return an error...
		if( it == params.end() )
			return kOfxStatErrUnknown;

		// get the param
		*param = ( it->second )->getParamHandle();

		// get the param property set
		if( propertySet )
			*propertySet = ( it->second )->getPropHandle();

		return kOfxStatOK;
	}

	ParamDescriptorSet* setDescriptor = dynamic_cast<ParamDescriptorSet*>( baseSet );

	if( setDescriptor )
	{
		const std::map<std::string, ParamDescriptor*>& params      = setDescriptor->getParams();
		std::map<std::string, ParamDescriptor*>::const_iterator it = params.find( name );

		// if we can't find it return an error...
		if( it == params.end() )
			return kOfxStatErrUnknown;

		// get the param
		*param = ( it->second )->getParamHandle();

		// get the param property set
		if( propertySet )
			*propertySet = ( it->second )->getPropHandle();

		return kOfxStatOK;
	}

	return kOfxStatErrBadHandle;
}

static OfxStatus paramSetGetPropertySet( OfxParamSetHandle     paramSet,
                                         OfxPropertySetHandle* propHandle )
{
	ParamAccessorSet* baseSet = reinterpret_cast<ParamAccessorSet*>( paramSet );

	if( baseSet )
	{
		*propHandle = baseSet->getParamSetProps().getHandle();
		return kOfxStatOK;
	}
	return kOfxStatErrBadHandle;
}

static OfxStatus paramGetPropertySet( OfxParamHandle        param,
                                      OfxPropertySetHandle* propHandle )
{
	attribute::ParamInstance* paramInstance = reinterpret_cast<attribute::ParamInstance*>( param );

	if( paramInstance && paramInstance->verifyMagic() )
	{
		// get the param property set
		*propHandle = paramInstance->getPropHandle();

		return kOfxStatOK;
	}
	else
		return kOfxStatErrBadHandle;
}

/// get the current param value

static OfxStatus paramGetValue( OfxParamHandle paramHandle, ... )
{
	ParamInstance* paramInstance = reinterpret_cast<ParamInstance*>( paramHandle );

	if( !paramInstance || !paramInstance->verifyMagic() )
		return kOfxStatErrBadHandle;

	va_list ap;
	va_start( ap, paramHandle );
	OfxStatus stat = kOfxStatErrUnsupported;

	try
	{
		stat = paramInstance->getV( ap );
	}
	catch(... )
	{
		COUT( "Exception in paramGetValue." );
	}

	va_end( ap );

	return stat;
}

/// get the param value at a time

static OfxStatus paramGetValueAtTime( OfxParamHandle paramHandle,
                                      OfxTime        time,
                                      ... )
{
	ParamInstance* paramInstance = reinterpret_cast<ParamInstance*>( paramHandle );

	if( !paramInstance || !paramInstance->verifyMagic() )
		return kOfxStatErrBadHandle;

	va_list ap;
	va_start( ap, time );
	OfxStatus stat = kOfxStatErrUnsupported;

	try
	{
		stat = paramInstance->getV( time, ap );
	}
	catch(... )
	{}

	va_end( ap );

	return stat;
}

/// get the param's derivative at the given time

static OfxStatus paramGetDerivative( OfxParamHandle paramHandle,
                                     OfxTime        time,
                                     ... )
{
	ParamInstance* paramInstance = reinterpret_cast<ParamInstance*>( paramHandle );

	if( !paramInstance || !paramInstance->verifyMagic() )
		return kOfxStatErrBadHandle;

	va_list ap;
	va_start( ap, time );
	OfxStatus stat = kOfxStatErrUnsupported;

	try
	{
		stat = paramInstance->deriveV( time, ap );
	}
	catch(... )
	{}

	va_end( ap );

	return stat;
}

static OfxStatus paramGetIntegral( OfxParamHandle paramHandle,
                                   OfxTime time1, OfxTime time2,
                                   ... )
{
	ParamInstance* paramInstance = reinterpret_cast<ParamInstance*>( paramHandle );

	if( !paramInstance || !paramInstance->verifyMagic() )
		return kOfxStatErrBadHandle;

	va_list ap;
	va_start( ap, time2 );
	OfxStatus stat = kOfxStatErrUnsupported;

	try
	{
		stat = paramInstance->integrateV( time1, time2, ap );
	}
	catch(... )
	{}

	va_end( ap );

	return stat;
}

/// set the param's value at the 'current' time

static OfxStatus paramSetValue( OfxParamHandle paramHandle,
                                ... )
{
	ParamInstance* paramInstance = reinterpret_cast<ParamInstance*>( paramHandle );

	if( !paramInstance || !paramInstance->verifyMagic() )
		return kOfxStatErrBadHandle;
	va_list ap;
	va_start( ap, paramHandle );
	OfxStatus stat = kOfxStatErrUnsupported;

	try
	{
		stat = paramInstance->setV( ap );
	}
	catch(... )
	{}

	va_end( ap );

	if( stat == kOfxStatOK )
	{
		if( paramInstance->getParamSetInstance() )
			paramInstance->getParamSetInstance()->paramChangedByPlugin( paramInstance );
		else
			stat = kOfxStatErrUnsupported;
	}

	return stat;
}

/// set the param's value at the indicated time, and set a key

static OfxStatus paramSetValueAtTime( OfxParamHandle paramHandle,
                                      OfxTime        time, // time in frames
                                      ... )
{
	ParamInstance* paramInstance = reinterpret_cast<ParamInstance*>( paramHandle );

	if( !paramInstance || !paramInstance->verifyMagic() )
		return kOfxStatErrBadHandle;

	va_list ap;
	va_start( ap, time );
	OfxStatus stat = kOfxStatErrUnsupported;

	try
	{
		stat = paramInstance->setV( time, ap );
	}
	catch(... )
	{}

	va_end( ap );

	if( stat == kOfxStatOK )
	{
		paramInstance->getParamSetInstance()->paramChangedByPlugin( paramInstance );
	}

	return stat;
}

static OfxStatus paramGetNumKeys( OfxParamHandle paramHandle,
                                  unsigned int*  numberOfKeys )
{
	attribute::ParamInstance* pInstance = reinterpret_cast<attribute::ParamInstance*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	KeyframeParam* paramInstance = dynamic_cast<KeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->getNumKeys( *numberOfKeys );
}

static OfxStatus paramGetKeyTime( OfxParamHandle paramHandle,
                                  unsigned int   nthKey,
                                  OfxTime*       time )
{
	attribute::ParamInstance* pInstance = reinterpret_cast<attribute::ParamInstance*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	KeyframeParam* paramInstance = dynamic_cast<KeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->getKeyTime( nthKey, *time );
}

static OfxStatus paramGetKeyIndex( OfxParamHandle paramHandle,
                                   OfxTime        time,
                                   int            direction,
                                   int*           index )
{
	attribute::ParamInstance* pInstance = reinterpret_cast<attribute::ParamInstance*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	KeyframeParam* paramInstance = dynamic_cast<KeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->getKeyIndex( time, direction, *index );
}

static OfxStatus paramDeleteKey( OfxParamHandle paramHandle,
                                 OfxTime        time )
{
	attribute::ParamInstance* pInstance = reinterpret_cast<attribute::ParamInstance*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	KeyframeParam* paramInstance = dynamic_cast<KeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->deleteKey( time );
}

static OfxStatus paramDeleteAllKeys( OfxParamHandle paramHandle )
{
	attribute::ParamInstance* pInstance = reinterpret_cast<attribute::ParamInstance*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	KeyframeParam* paramInstance = dynamic_cast<KeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->deleteAllKeys();
}

static OfxStatus paramCopy( OfxParamHandle paramTo,
                            OfxParamHandle paramFrom,
                            OfxTime dstOffset, OfxRangeD* frameRange )
{
	ParamInstance* paramInstanceTo   = reinterpret_cast<ParamInstance*>( paramTo );
	ParamInstance* paramInstanceFrom = reinterpret_cast<ParamInstance*>( paramFrom );

	if( !paramInstanceTo || !paramInstanceTo->verifyMagic() )
		return kOfxStatErrBadHandle;
	if( !paramInstanceFrom || !paramInstanceFrom->verifyMagic() )
		return kOfxStatErrBadHandle;

	if( !frameRange )
		return paramInstanceTo->copy( *paramInstanceFrom, dstOffset );
	else
		return paramInstanceTo->copy( *paramInstanceFrom, dstOffset, *frameRange );
}

static OfxStatus paramEditBegin( OfxParamSetHandle paramSet, const char* name )
{
	ParamInstanceSet* setInstance = reinterpret_cast<ParamInstanceSet*>( paramSet );

	if( !setInstance )
		return kOfxStatErrBadHandle;
	return setInstance->editBegin( std::string( name ) );
}

static OfxStatus paramEditEnd( OfxParamSetHandle paramSet )
{
	ParamInstanceSet* setInstance = reinterpret_cast<ParamInstanceSet*>( paramSet );

	if( !setInstance )
		return kOfxStatErrBadHandle;
	return setInstance->editEnd();
}

static OfxParameterSuiteV1 gParamSuiteV1 = {
	paramDefine,
	paramGetHandle,
	paramSetGetPropertySet,
	paramGetPropertySet,
	paramGetValue,
	paramGetValueAtTime,
	paramGetDerivative,
	paramGetIntegral,
	paramSetValue,
	paramSetValueAtTime,
	paramGetNumKeys,
	paramGetKeyTime,
	paramGetKeyIndex,
	paramDeleteKey,
	paramDeleteAllKeys,
	paramCopy,
	paramEditBegin,
	paramEditEnd
};

void* GetSuite( int version )
{
	if( version == 1 )
		return &gParamSuiteV1;
	return NULL;
}

}
}
}
}
