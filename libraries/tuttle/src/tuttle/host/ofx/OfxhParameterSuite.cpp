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
	try
	{
		OfxhParamDescriptorSet* paramSetDescriptor = reinterpret_cast<OfxhParamDescriptorSet*>( paramSet );
		if( !paramSetDescriptor )
			return kOfxStatErrBadHandle;

		OfxhParamDescriptor* desc = paramSetDescriptor->paramDefine( paramType, name );
		if( !desc )
			return kOfxStatErrUnsupported;

		*propertySet = desc->getPropHandle();

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramGetHandle( OfxParamSetHandle     paramSet,
                                 const char*           name,
                                 OfxParamHandle*       param,
                                 OfxPropertySetHandle* propertySet )
{
	try
	{
		OfxhParamAccessorSet* baseSet = reinterpret_cast<OfxhParamAccessorSet*>( paramSet );
		if( !baseSet )
			return kOfxStatErrBadHandle;

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
		if( !setDescriptor )
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
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramSetGetPropertySet( OfxParamSetHandle     paramSet,
                                         OfxPropertySetHandle* propHandle )
{
	try
	{
		OfxhParamAccessorSet* baseSet = reinterpret_cast<OfxhParamAccessorSet*>( paramSet );
		if( !baseSet )
			return kOfxStatErrBadHandle;

		*propHandle = baseSet->getParamSetProps().getHandle();

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramGetPropertySet( OfxParamHandle        param,
                                      OfxPropertySetHandle* propHandle )
{
	try
	{
		attribute::OfxhParam* paramInstance = reinterpret_cast<attribute::OfxhParam*>( param );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		// get the param property set
		*propHandle = paramInstance->getPropHandle();

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

/**
 * get the current param value
 */
static OfxStatus paramGetValue( OfxParamHandle paramHandle, ... )
{
	try
	{
		OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		va_list ap;
		va_start( ap, paramHandle );
		OfxStatus stat = kOfxStatOK;

		try
		{
			paramInstance->getV( ap );
		}
		catch( OfxhException& e )
		{
			stat = e.getStatus();
		}
		catch( ... )
		{
			stat = kOfxStatErrUnknown;
		}

		va_end( ap );

		return stat;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

/**
 * get the param value at a time
 */
static OfxStatus paramGetValueAtTime( OfxParamHandle paramHandle,
                                      OfxTime        time,
                                      ... )
{
	try
	{
		OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		va_list ap;
		va_start( ap, time );
		OfxStatus stat = kOfxStatOK;

		try
		{
			paramInstance->getV( time, ap );
		}
		catch( OfxhException& e )
		{
			stat = e.getStatus();
		}
		catch( ... )
		{
			stat = kOfxStatErrUnknown;
		}

		va_end( ap );

		return stat;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

/**
 * get the param's derivative at the given time
 */
static OfxStatus paramGetDerivative( OfxParamHandle paramHandle,
                                     OfxTime        time,
                                     ... )
{
	try
	{
		OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		va_list ap;
		va_start( ap, time );
		OfxStatus stat = kOfxStatOK;

		try
		{
			paramInstance->deriveV( time, ap );
		}
		catch( OfxhException& e )
		{
			stat = e.getStatus();
		}
		catch( ... )
		{
			stat = kOfxStatErrUnknown;
		}

		va_end( ap );

		return stat;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramGetIntegral( OfxParamHandle paramHandle,
                                   OfxTime time1, OfxTime time2,
                                   ... )
{
	try
	{
		OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		va_list ap;
		va_start( ap, time2 );
		OfxStatus stat = kOfxStatOK;

		try
		{
			paramInstance->integrateV( time1, time2, ap );
		}
		catch( OfxhException& e )
		{
			stat = e.getStatus();
		}
		catch( ... )
		{
			stat = kOfxStatErrUnknown;
		}

		va_end( ap );

		return stat;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

/**
 * set the param's value at the 'current' time
 */
static OfxStatus paramSetValue( OfxParamHandle paramHandle,
                                ... )
{
	try
	{
		OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		va_list ap;
		va_start( ap, paramHandle );
		OfxStatus stat = kOfxStatOK;

		try
		{
			paramInstance->setV( ap );
		}
		catch( OfxhException& e )
		{
			stat = e.getStatus();
		}
		catch( ... )
		{
			stat = kOfxStatErrUnknown;
		}

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
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

/**
 * set the param's value at the indicated time, and set a key
 */
static OfxStatus paramSetValueAtTime( OfxParamHandle paramHandle,
                                      OfxTime        time, // time in frames
                                      ... )
{
	try
	{
		OfxhParam* paramInstance = reinterpret_cast<OfxhParam*>( paramHandle );
		if( !paramInstance || !paramInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		va_list ap;
		va_start( ap, time );
		OfxStatus stat = kOfxStatOK;

		try
		{
			paramInstance->setV( time, ap );
		}
		catch( OfxhException& e )
		{
			stat = e.getStatus();
		}
		catch( ... )
		{
			stat = kOfxStatErrUnknown;
		}

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
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramGetNumKeys( OfxParamHandle paramHandle,
                                  unsigned int*  numberOfKeys )
{
	try
	{
		attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );
		if( !pInstance || !pInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
		if( !paramInstance )
			return kOfxStatErrBadHandle;
		
		paramInstance->getNumKeys( *numberOfKeys );

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramGetKeyTime( OfxParamHandle paramHandle,
                                  unsigned int   nthKey,
                                  OfxTime*       time )
{
	try
	{
		attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );
		if( !pInstance || !pInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
		if( !paramInstance )
			return kOfxStatErrBadHandle;

		paramInstance->getKeyTime( nthKey, *time );
		
		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramGetKeyIndex( OfxParamHandle paramHandle,
                                   OfxTime        time,
                                   int            direction,
                                   int*           index )
{
	try
	{
		attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );
		if( !pInstance || !pInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
		if( !paramInstance )
			return kOfxStatErrBadHandle;
		
		paramInstance->getKeyIndex( time, direction, *index );

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramDeleteKey( OfxParamHandle paramHandle,
                                 OfxTime        time )
{
	try
	{
		attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );
		if( !pInstance || !pInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
		if( !paramInstance )
			return kOfxStatErrBadHandle;

		paramInstance->deleteKey( time );

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramDeleteAllKeys( OfxParamHandle paramHandle )
{
	try
	{
		attribute::OfxhParam* pInstance = reinterpret_cast<attribute::OfxhParam*>( paramHandle );
		if( !pInstance || !pInstance->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhKeyframeParam* paramInstance = dynamic_cast<OfxhKeyframeParam*>( pInstance );
		if( !paramInstance )
			return kOfxStatErrBadHandle;
		
		paramInstance->deleteAllKeys();

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramCopy( OfxParamHandle paramTo,
                            OfxParamHandle paramFrom,
                            OfxTime dstOffset, OfxRangeD* frameRange )
{
	try
	{
		OfxhParam* paramInstanceTo   = reinterpret_cast<OfxhParam*>( paramTo );
		if( !paramInstanceTo || !paramInstanceTo->verifyMagic() )
			return kOfxStatErrBadHandle;

		OfxhParam* paramInstanceFrom = reinterpret_cast<OfxhParam*>( paramFrom );
		if( !paramInstanceFrom || !paramInstanceFrom->verifyMagic() )
			return kOfxStatErrBadHandle;

		if( !frameRange )
			paramInstanceTo->copy( *paramInstanceFrom, dstOffset );
		else
			paramInstanceTo->copy( *paramInstanceFrom, dstOffset, *frameRange );

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramEditBegin( OfxParamSetHandle paramSet, const char* name )
{
	try
	{
		OfxhParamSet* setInstance = reinterpret_cast<OfxhParamSet*>( paramSet );
		if( !setInstance )
			return kOfxStatErrBadHandle;

		setInstance->editBegin( std::string( name ) );
		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
}

static OfxStatus paramEditEnd( OfxParamSetHandle paramSet )
{
	try
	{
		OfxhParamSet* setInstance = reinterpret_cast<OfxhParamSet*>( paramSet );
		if( !setInstance )
			return kOfxStatErrBadHandle;
		
		setInstance->editEnd();

		return kOfxStatOK;
	}
	catch( OfxhException& e )
	{
		return e.getStatus();
	}
	catch( ... )
	{
		return kOfxStatErrUnknown;
	}
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
