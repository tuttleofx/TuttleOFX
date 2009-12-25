#include "OfxhParameterSuite.hpp"
#include "OfxhParam.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


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

void* getParameterSuite( const int version )
{
	if( version == 1 )
		return &gParamSuiteV1;
	return NULL;
}

}
}
}
}
