#include "OfxhParam.hpp"
#include "OfxhParamSet.hpp"
#include "OfxhParamDescriptor.hpp"

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

/**
 * make a parameter, with the given type and name
 */
OfxhParam::OfxhParam( const OfxhParamDescriptor& descriptor, const std::string& name, attribute::OfxhParamSet& setInstance )
	: attribute::OfxhAttribute( descriptor ),
	_paramSetInstance( &setInstance ),
	_parentInstance( NULL ),
	_avoidRecursion( false )
{
	// parameter has to be owned by paramSet
	setInstance.addParam( name, this ); ///< @todo tuttle move this from here (introduce too many problems), no good reason to be here.

	getEditableProperties().addNotifyHook( kOfxParamPropEnabled, this );
	getEditableProperties().addNotifyHook( kOfxParamPropSecret, this );
	getEditableProperties().addNotifyHook( kOfxPropLabel, this );
}

/**
 * the description of a plugin parameter
 */
OfxhParam::~OfxhParam() {}

void OfxhParam::paramChanged( const attribute::EChange change )
{
	_paramSetInstance->paramChanged( *this, change );
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
void OfxhParam::getV( va_list arg ) const OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrUnsupported, std::string("ParamInstance getValue failed (paramName:") + getName() + ")" );
}

/**
 * get a value, implemented by instances to deconstruct var args
 */
void OfxhParam::getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrUnsupported );
}

/**
 * set a value, implemented by instances to deconstruct var args
 */
void OfxhParam::setV( va_list arg ) OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrUnsupported );
}

/**
 * key a value, implemented by instances to deconstruct var args
 */
void OfxhParam::setV( const OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrUnsupported );
}

/**
 * derive a value, implemented by instances to deconstruct var args
 */
void OfxhParam::deriveV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrUnsupported );
}

/**
 * integrate a value, implemented by instances to deconstruct var args
 */
void OfxhParam::integrateV( const OfxTime time1, const OfxTime time2, va_list arg ) const OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrUnsupported );
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
void OfxhParam::copy( const OfxhParam& instance, OfxTime offset ) OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrMissingHostFeature );
}

/**
 * copy one parameter to another, with a range
 */
void OfxhParam::copy( const OfxhParam& instance, OfxTime offset, OfxRangeD range ) OFX_EXCEPTION_SPEC
{
	throw OfxhException( kOfxStatErrMissingHostFeature );
}

void OfxhParam::setParentInstance( OfxhParam* instance )
{
	_parentInstance = instance;
}

OfxhParam* OfxhParam::getParentInstance()
{
	return _parentInstance;
}




}
}
}
}
