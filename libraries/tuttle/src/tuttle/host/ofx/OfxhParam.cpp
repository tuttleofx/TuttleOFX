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
#include "OfxhBinary.hpp"
#include "OfxhPropertySuite.hpp"
#include "OfxhParam.hpp"
#include "OfxhImageEffect.hpp"

#include <cassert>
#include <cfloat>
#include <climits>
#include <cstdarg>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

OfxhParamAccessor::OfxhParamAccessor()
{}

OfxhParamAccessor::~OfxhParamAccessor() {}

const std::string& OfxhParamAccessor::getParamType() const
{
	return getProperties().getStringProperty( kOfxParamPropType );
}

const std::string& OfxhParamAccessor::getParentName() const
{
	return getProperties().getStringProperty( kOfxParamPropParent );
}

const std::string& OfxhParamAccessor::getScriptName() const
{
	return getProperties().getStringProperty( kOfxParamPropScriptName );
}

const std::string& OfxhParamAccessor::getHint() const
{
	return getProperties().getStringProperty( kOfxParamPropHint, 0 );
}

const std::string& OfxhParamAccessor::getDoubleType() const
{
	return getProperties().getStringProperty( kOfxParamPropDoubleType, 0 );
}

const std::string& OfxhParamAccessor::getCacheInvalidation() const
{
	return getProperties().getStringProperty( kOfxParamPropCacheInvalidation, 0 );
}

bool OfxhParamAccessor::getEnabled() const
{
	return getProperties().getIntProperty( kOfxParamPropEnabled, 0 ) != 0;
}

bool OfxhParamAccessor::getSecret() const
{
	return getProperties().getIntProperty( kOfxParamPropSecret, 0 ) != 0;
}

bool OfxhParamAccessor::getEvaluateOnChange() const
{
	return getProperties().getIntProperty( kOfxParamPropEvaluateOnChange, 0 ) != 0;
}

bool OfxhParamAccessor::getCanUndo() const
{
	if( getProperties().hasProperty( kOfxParamPropCanUndo ) )
	{
		return getProperties().getIntProperty( kOfxParamPropCanUndo ) != 0;
	}
	return false;
}

bool OfxhParamAccessor::getCanAnimate() const
{
	if( getProperties().hasProperty( kOfxParamPropAnimates ) )
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
	property::TypeEnum propType;
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
	{ kOfxParamTypeInteger, property::eInt, 1 },
	{ kOfxParamTypeDouble, property::eDouble, 1 },
	{ kOfxParamTypeBoolean, property::eInt, 1 },
	{ kOfxParamTypeChoice, property::eInt, 1 },
	{ kOfxParamTypeRGBA, property::eDouble, 4 },
	{ kOfxParamTypeRGB, property::eDouble, 3 },
	{ kOfxParamTypeDouble2D, property::eDouble, 2 },
	{ kOfxParamTypeInteger2D, property::eInt, 2 },
	{ kOfxParamTypeDouble3D, property::eDouble, 3 },
	{ kOfxParamTypeInteger3D, property::eInt, 3 },
	{ kOfxParamTypeString, property::eString, 1 },
	{ kOfxParamTypeCustom, property::eString, 1 },
	{ kOfxParamTypeGroup, property::eNone },
	{ kOfxParamTypePage, property::eNone },
	{ kOfxParamTypePushButton, property::eNone },
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

bool findType( const std::string paramType, property::TypeEnum& propType, int& propDim )
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
OfxhParamDescriptor::OfxhParamDescriptor( const std::string& type, const std::string& name )
	: attribute::OfxhAttributeDescriptor( property::OfxhSet() )
{
	const char* ctype = type.c_str();
	const char* cname = name.c_str();

	static const property::OfxhPropSpec paramDescriptorProps[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeParameter },
		{ kOfxParamPropSecret, property::eInt, 1, false, "0" },
		{ kOfxParamPropHint, property::eString, 1, false, "" },
		{ kOfxParamPropParent, property::eString, 1, false, "" },
		{ kOfxParamPropEnabled, property::eInt, 1, false, "1" },
		{ kOfxParamPropDataPtr, property::ePointer, 1, false, 0 },
		{ 0 }
	};

	const property::OfxhPropSpec dynamicParamDescriptorProps[] = {
		{ kOfxParamPropType, property::eString, 1, true, ctype },
		{ kOfxParamPropScriptName, property::eString, 1, false, cname }, ///< @todo TUTTLE_TODO : common property for all Attributes
		{ 0 }
	};

	getEditableProperties().addProperties( paramDescriptorProps );
	getEditableProperties().addProperties( dynamicParamDescriptorProps );

	setAllNames( name );

	getEditableProperties().setStringProperty( kOfxParamPropType, type );
	assert( ctype );
}

/// make a parameter, with the given type and name

void OfxhParamDescriptor::addStandardParamProps( const std::string& type )
{
	property::TypeEnum propType = property::eString;
	int propDim                 = 1;

	findType( type, propType, propDim );

	static property::OfxhPropSpec allString[] = {
		{ kOfxParamPropStringMode, property::eString, 1, false, kOfxParamStringIsSingleLine },
		{ kOfxParamPropStringFilePathExists, property::eInt, 1, false, "1" },
		{ 0 }
	};

	static property::OfxhPropSpec allChoice[] = {
		{ kOfxParamPropChoiceOption, property::eString, 0, false, "" },
		{ 0 }
	};

	static property::OfxhPropSpec allCustom[] = {
		{ kOfxParamPropCustomInterpCallbackV1, property::ePointer, 1, false, 0 },
		{ 0 },
	};

	static property::OfxhPropSpec allPage[] = {
		{ kOfxParamPropPageChild, property::eString, 0, false, "" },
		{ 0 }
	};

	if( propType != property::eNone )
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

void OfxhParamDescriptor::addInteractParamProps( const std::string& type )
{
	static property::OfxhPropSpec allButGroupPageProps[] = {
		{ kOfxParamPropInteractV1, property::ePointer, 1, false, 0 },
		{ kOfxParamPropInteractSize, property::eDouble, 2, false, "0" },
		{ kOfxParamPropInteractSizeAspect, property::eDouble, 1, false, "1" },
		{ kOfxParamPropInteractMinimumSize, property::eInt, 2, false, "10" },
		{ kOfxParamPropInteractPreferedSize, property::eInt, 2, false, "10" },
		{ 0 }
	};

	getEditableProperties().addProperties( allButGroupPageProps );
}

/// add standard properties to a value holding param

void OfxhParamDescriptor::addValueParamProps( const std::string& type, property::TypeEnum valueType, int dim )
{
	static property::OfxhPropSpec invariantProps[] = {
		{ kOfxParamPropAnimates, property::eInt, 1, false, "1" },
		{ kOfxParamPropIsAnimating, property::eInt, 1, false, "0" },
		{ kOfxParamPropIsAutoKeying, property::eInt, 1, false, "0" },
		{ kOfxParamPropPersistant, property::eInt, 1, false, "1" },
		{ kOfxParamPropEvaluateOnChange, property::eInt, 1, false, "1" },
		{ kOfxParamPropPluginMayWrite, property::eInt, 1, false, "0" },
		{ kOfxParamPropCanUndo, property::eInt, 1, false, "1" },
		{ kOfxParamPropCacheInvalidation, property::eString, 1, false, kOfxParamInvalidateValueChange },
		{ 0 }
	};

	property::OfxhPropSpec variantProps[] = {
		{ kOfxParamPropDefault, valueType, dim, false, valueType == property::eString ? "" : "0" },
		{ 0 }
	};

	getEditableProperties().addProperties( invariantProps );
	getEditableProperties().addProperties( variantProps );
}

/// add standard properties to a value holding param

void OfxhParamDescriptor::addNumericParamProps( const std::string& type, property::TypeEnum valueType, int dim )
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

	property::OfxhPropSpec allNumeric[] = {
		{ kOfxParamPropDisplayMin, valueType, dim, false, isColourParam( type ) ? "0" : ( valueType == property::eDouble ? dbl_minstr : int_minstr ).c_str() },
		{ kOfxParamPropDisplayMax, valueType, dim, false, isColourParam( type ) ? "1" : ( valueType == property::eDouble ? dbl_maxstr : int_maxstr ).c_str() },
		{ kOfxParamPropMin, valueType, dim, false, ( valueType == property::eDouble ? dbl_minstr : int_minstr ).c_str() },
		{ kOfxParamPropMax, valueType, dim, false, ( valueType == property::eDouble ? dbl_maxstr : int_maxstr ).c_str() },
		{ 0 }
	};

	getEditableProperties().addProperties( allNumeric );

	/// if any double or a colour
	if( valueType == property::eDouble )
	{
		static property::OfxhPropSpec allDouble[] = {
			{ kOfxParamPropIncrement, property::eDouble, 1, false, "1" },
			{ kOfxParamPropDigits, property::eInt, 1, false, "2" },
			{ 0 }
		};
		getEditableProperties().addProperties( allDouble );
	}

	/// if a double param type
	if( isDoubleParam( type ) )
	{
		static property::OfxhPropSpec allDouble[] = {
			{ kOfxParamPropDoubleType, property::eString, 1, false, kOfxParamDoubleTypePlain },
			{ 0 }
		};
		getEditableProperties().addProperties( allDouble );

		if( dim == 1 )
		{
			static property::OfxhPropSpec allDouble1D[] = {
				{ kOfxParamPropShowTimeMarker, property::eInt, 1, false, "0" },
				{ 0 }
			};

			getEditableProperties().addProperties( allDouble1D );
		}
	}

	/// if a multi dimensional param
	if( isDoubleParam( type ) && ( dim == 2 || dim == 3 ) )
	{
		property::OfxhPropSpec all2D3D[] = {
			{ kOfxParamPropDimensionLabel, property::eString, dim, false, "" },
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
		property::OfxhPropSpec allColor[] = {
			{ kOfxParamPropDimensionLabel, property::eString, dim, false, "" },
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

OfxhParamAccessorSet::~OfxhParamAccessorSet() {}

/// obtain a handle on this set for passing to the C api

OfxhParamDescriptorSet::OfxhParamDescriptorSet() {}

OfxhParamDescriptorSet::~OfxhParamDescriptorSet()
{}

void OfxhParamDescriptorSet::addParam( const std::string& name, OfxhParamDescriptor* p )
{
	_paramList.push_back( p );
	_paramMap[name] = p;
}

/// define a param on this effect

OfxhParamDescriptor* OfxhParamDescriptorSet::paramDefine( const char* paramType,
                                                          const char* name )
{
	if( !isStandardType( paramType ) )
		return NULL;                                                                                                                                                                                                                                                                                              /// << EEK! This is bad.

	OfxhParamDescriptor* desc = new OfxhParamDescriptor( paramType, name );
	desc->addStandardParamProps( paramType );
	addParam( name, desc );
	return desc;
}

////////////////////////////////////////////////////////////////////////////////
//
// Instance
//

/**
 * the description of a plugin parameter
 */
OfxhParam::~OfxhParam() {}

/**
 * make a parameter, with the given type and name
 */
OfxhParam::OfxhParam( const OfxhParamDescriptor& descriptor, attribute::OfxhParamSet& setInstance )
	: attribute::AttributeInstance( descriptor ),
	_paramSetInstance( &setInstance ),
	_parentInstance( 0 )
{
	// parameter has to be owned by paramSet
	setInstance.addParam( descriptor.getName(), this ); ///< @todo tuttle move this from here (introduce too many problems), no good reason to be here.

	getEditableProperties().addNotifyHook( kOfxParamPropEnabled, this );
	getEditableProperties().addNotifyHook( kOfxParamPropSecret, this );
	getEditableProperties().addNotifyHook( kOfxPropLabel, this );
}

/**
 * callback which should set enabled state as appropriate
 */
void OfxhParam::setEnabled() {}

/**
 * callback which should set secret state as appropriate
 */
void OfxhParam::setSecret() {}

/**
 * callback which should update label
 */
void OfxhParam::setLabel() {}

/**
 * callback which should set
 */
void OfxhParam::setDisplayRange() {}

/**
 * get a value, implemented by instances to deconstruct var args
 */
OfxStatus OfxhParam::getV( va_list arg )
{
	COUT_WITHINFOS( "ParamInstance getValue failed !" );
	COUT( "(paramName:" << getName() << ")" );
	return kOfxStatErrUnsupported;
}

/**
 * get a value, implemented by instances to deconstruct var args
 */
OfxStatus OfxhParam::getV( OfxTime time, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/**
 * set a value, implemented by instances to deconstruct var args
 */
OfxStatus OfxhParam::setV( va_list arg )
{
	return kOfxStatErrUnsupported;
}

/**
 * key a value, implemented by instances to deconstruct var args
 */
OfxStatus OfxhParam::setV( OfxTime time, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/**
 * derive a value, implemented by instances to deconstruct var args
 */
OfxStatus OfxhParam::deriveV( OfxTime time, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/**
 * integrate a value, implemented by instances to deconstruct var args
 */
OfxStatus OfxhParam::integrateV( OfxTime time1, OfxTime time2, va_list arg )
{
	return kOfxStatErrUnsupported;
}

/**
 * overridden from Property::NotifyHook
 */
void OfxhParam::notify( const std::string& name, bool single, int num ) OFX_EXCEPTION_SPEC
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

/**
 * copy one parameter to another
 */
OfxStatus OfxhParam::copy( const OfxhParam& instance, OfxTime offset )
{
	return kOfxStatErrMissingHostFeature;
}

/**
 * copy one parameter to another, with a range
 */
OfxStatus OfxhParam::copy( const OfxhParam& instance, OfxTime offset, OfxRangeD range )
{
	return kOfxStatErrMissingHostFeature;
}

void OfxhParam::setParentInstance( OfxhParam* instance )
{
	_parentInstance = instance;
}

OfxhParam* OfxhParam::getParentInstance()
{
	return _parentInstance;
}

//
// KeyframeParam
//

OfxStatus OfxhKeyframeParam::getNumKeys( unsigned int& nKeys ) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus OfxhKeyframeParam::getKeyTime( int nth, OfxTime& time ) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus OfxhKeyframeParam::getKeyIndex( OfxTime time, int direction, int& index ) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus OfxhKeyframeParam::deleteKey( OfxTime time )
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus OfxhKeyframeParam::deleteAllKeys()
{
	return kOfxStatErrMissingHostFeature;
}

OfxhParamGroup* OfxhParamGroup::clone() const
{
	return new OfxhParamGroup( *this );
}

/**
 * setChildrens have to clone each source instance recursively
 */
void OfxhParamGroup::setChildrens( const OfxhParamSet* childrens )
{
	deleteChildrens();

	/// @todo use clone ?
	for( ParamList::const_iterator it = childrens->getParamList().begin(), itEnd = childrens->getParamList().end();
	     it != itEnd;
	     ++it )
	{
		_paramList.push_back( it->clone( /*this*/ ) );
	}
}

void OfxhParamGroup::addChildren( OfxhParam* children )
{
	children->setParamSetInstance( this );
	_paramList.push_back( children );
}

OfxhParamSet* OfxhParamGroup::getChildrens() const
{
	return (OfxhParamSet*)this;
}

//
// Page Instance
//
ParamPageInstance* ParamPageInstance::clone() const
{
	return new ParamPageInstance( *this );
}

const std::map<int, attribute::OfxhParam*>& ParamPageInstance::getChildren() const
{
	// HACK!!!! this really should be done with a notify hook so we don't force
	// _children to be mutable
	if( _children.size() == 0 )
	{
		int nChildren = getProperties().getDimension( kOfxParamPropPageChild );
		for( int i = 0; i < nChildren; i++ )
		{
			std::string childName       = getProperties().getStringProperty( kOfxParamPropPageChild, i );
			attribute::OfxhParam* child = &_paramSetInstance->getParam( childName );
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

/**
 * implementation of var args function
 */
OfxStatus ParamChoiceInstance::getV( OfxTime time, va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( time, *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamChoiceInstance::setV( va_list arg )
{
	int value = va_arg( arg, int );

	return set( value );
}

/**
 * implementation of var args function
 */
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

/**
 * implementation of var args function
 */
OfxStatus ParamIntegerInstance::getV( va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamIntegerInstance::getV( OfxTime time, va_list arg )
{
	int* value = va_arg( arg, int* );

	return get( time, *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamIntegerInstance::setV( va_list arg )
{
	int value = va_arg( arg, int );

	return set( value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamIntegerInstance::setV( OfxTime time, va_list arg )
{
	int value = va_arg( arg, int );

	return set( time, value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamIntegerInstance::deriveV( OfxTime time, va_list arg )
{
	int* value = va_arg( arg, int* );

	return derive( time, *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamIntegerInstance::integrateV( OfxTime time1, OfxTime time2, va_list arg )
{
	int* value = va_arg( arg, int* );

	return integrate( time1, time2, *value );
}

//
// DoubleInstance
//

/**
 * implementation of var args function
 */
OfxStatus ParamDoubleInstance::getV( va_list arg )
{
	double* value = va_arg( arg, double* );

	return get( *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamDoubleInstance::getV( OfxTime time, va_list arg )
{
	double* value = va_arg( arg, double* );

	return get( time, *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamDoubleInstance::setV( va_list arg )
{
	double value = va_arg( arg, double );

	return set( value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamDoubleInstance::setV( OfxTime time, va_list arg )
{
	double value = va_arg( arg, double );

	return set( time, value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamDoubleInstance::deriveV( OfxTime time, va_list arg )
{
	double* value = va_arg( arg, double* );

	return derive( time, *value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamDoubleInstance::integrateV( OfxTime time1, OfxTime time2, va_list arg )
{
	double* value = va_arg( arg, double* );

	return integrate( time1, time2, *value );
}

//
// BooleanInstance
//

/**
 * implementation of var args function
 */
OfxStatus ParamBooleanInstance::getV( va_list arg )
{
	bool v;
	OfxStatus stat = get( v );

	int* value = va_arg( arg, int* );

	*value = v;
	return stat;
}

/**
 * implementation of var args function
 */
OfxStatus ParamBooleanInstance::getV( OfxTime time, va_list arg )
{
	bool v;
	OfxStatus stat = get( time, v );

	int* value = va_arg( arg, int* );

	*value = v;
	return stat;
}

/**
 * implementation of var args function
 */
OfxStatus ParamBooleanInstance::setV( va_list arg )
{
	bool value = va_arg( arg, int ) != 0;

	return set( value );
}

/**
 * implementation of var args function
 */
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

/**
 * implementation of var args function
 */
OfxStatus ParamStringInstance::getV( OfxTime time, va_list arg )
{
	const char** value = va_arg( arg, const char** );

	OfxStatus stat = get( time, _returnValue ); // I so don't like this, temp storage should be delegated to the implementation

	*value = _returnValue.c_str();
	return stat;
}

/**
 * implementation of var args function
 */
OfxStatus ParamStringInstance::setV( va_list arg )
{
	char* value = va_arg( arg, char* );

	return set( value );
}

/**
 * implementation of var args function
 */
OfxStatus ParamStringInstance::setV( OfxTime time, va_list arg )
{
	char* value = va_arg( arg, char* );

	return set( time, value );
}

//////////////////////////////////////////////////////////////////////////////////
// ParamInstanceSet
//

OfxhParamSet::OfxhParamSet()
{}

OfxhParamSet::OfxhParamSet( const OfxhParamSet& other )
	: _paramList( other._paramList.clone() )
{
	initMapFromList();
}

void OfxhParamSet::initMapFromList()
{
	for( ParamList::iterator it = _paramList.begin(), itEnd = _paramList.end();
	     it != itEnd;
	     ++it )
	{
		_params[it->getName()] = &( *it );
	}
}

OfxhParamSet::~OfxhParamSet()
{}

OfxStatus OfxhParamSet::addParam( const std::string& name, OfxhParam* instance )
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
	OfxhParamDescriptorSet* paramSetDescriptor = reinterpret_cast<OfxhParamDescriptorSet*>( paramSet );

	if( !paramSetDescriptor )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhParamDescriptor* desc = paramSetDescriptor->paramDefine( paramType, name );

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

	OfxhParamAccessorSet* baseSet = reinterpret_cast<OfxhParamAccessorSet*>( paramSet );

	if( !baseSet )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhParamSet* setInstance = dynamic_cast<OfxhParamSet*>( baseSet );

	if( setInstance )
	{
		const std::map<std::string, OfxhParam*>& params      = setInstance->getParams();
		std::map<std::string, OfxhParam*>::const_iterator it = params.find( name );

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

	OfxhParamDescriptorSet* setDescriptor = dynamic_cast<OfxhParamDescriptorSet*>( baseSet );

	if( setDescriptor )
	{
		const std::map<std::string, OfxhParamDescriptor*>& params      = setDescriptor->getParams();
		std::map<std::string, OfxhParamDescriptor*>::const_iterator it = params.find( name );

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
	OfxhParamAccessorSet* baseSet = reinterpret_cast<OfxhParamAccessorSet*>( paramSet );

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
	attribute::OfxhParam* paramInstance = reinterpret_cast<attribute::OfxhParam*>( param );

	if( paramInstance && paramInstance->verifyMagic() )
	{
		// get the param property set
		*propHandle = paramInstance->getPropHandle();

		return kOfxStatOK;
	}
	else
		return kOfxStatErrBadHandle;
}

/**
 * get the current param value
 */
static OfxStatus paramGetValue( OfxParamHandle paramHandle, ... )
{
	OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );

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

/**
 * get the param value at a time
 */
static OfxStatus paramGetValueAtTime( OfxParamHandle paramHandle,
                                      OfxTime        time,
                                      ... )
{
	OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );

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

/**
 * get the param's derivative at the given time
 */
static OfxStatus paramGetDerivative( OfxParamHandle paramHandle,
                                     OfxTime        time,
                                     ... )
{
	OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );

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
	OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );

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

/**
 * set the param's value at the 'current' time
 */
static OfxStatus paramSetValue( OfxParamHandle paramHandle,
                                ... )
{
	OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );

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

/**
 * set the param's value at the indicated time, and set a key
 */
static OfxStatus paramSetValueAtTime( OfxParamHandle paramHandle,
                                      OfxTime        time, // time in frames
                                      ... )
{
	OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );

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
	attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->getNumKeys( *numberOfKeys );
}

static OfxStatus paramGetKeyTime( OfxParamHandle paramHandle,
                                  unsigned int   nthKey,
                                  OfxTime*       time )
{
	attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->getKeyTime( nthKey, *time );
}

static OfxStatus paramGetKeyIndex( OfxParamHandle paramHandle,
                                   OfxTime        time,
                                   int            direction,
                                   int*           index )
{
	attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->getKeyIndex( time, direction, *index );
}

static OfxStatus paramDeleteKey( OfxParamHandle paramHandle,
                                 OfxTime        time )
{
	attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->deleteKey( time );
}

static OfxStatus paramDeleteAllKeys( OfxParamHandle paramHandle )
{
	attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );

	if( !pInstance || !pInstance->verifyMagic() )
	{
		return kOfxStatErrBadHandle;
	}

	OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
	if( !paramInstance )
		return kOfxStatErrBadHandle;
	return paramInstance->deleteAllKeys();
}

static OfxStatus paramCopy( OfxParamHandle paramTo,
                            OfxParamHandle paramFrom,
                            OfxTime dstOffset, OfxRangeD* frameRange )
{
	OfxhParam* paramInstanceTo   = reinterpret_cast<OfxhParam*>( paramTo );
	OfxhParam* paramInstanceFrom = reinterpret_cast<OfxhParam*>( paramFrom );

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
	OfxhParamSet* setInstance = reinterpret_cast<OfxhParamSet*>( paramSet );

	if( !setInstance )
		return kOfxStatErrBadHandle;
	return setInstance->editBegin( std::string( name ) );
}

static OfxStatus paramEditEnd( OfxParamSetHandle paramSet )
{
	OfxhParamSet* setInstance = reinterpret_cast<OfxhParamSet*>( paramSet );

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
