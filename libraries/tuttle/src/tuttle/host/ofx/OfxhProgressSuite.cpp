#include "OfxhProgressSuite.hpp"
#include "OfxhProgress.hpp"
#include "OfxhException.hpp"

namespace tuttle {
namespace host {
namespace ofx {

static OfxStatus ProgressStart( void*       effectInstance,
                                const char* label )
{
	try
	{
		OfxhIProgress* me = reinterpret_cast<OfxhIProgress*>( effectInstance );
		if( !me )
			return kOfxStatErrBadHandle;
		
		me->progressStart( label );

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

/// finish progressing

static OfxStatus ProgressEnd( void* effectInstance )
{
	try
	{
		OfxhIProgress* me = reinterpret_cast<OfxhIProgress*>( effectInstance );
		if( !me )
			return kOfxStatErrBadHandle;

		me->progressEnd();

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

/// update progressing

static OfxStatus ProgressUpdate( void* effectInstance, double progress )
{
	try
	{
		OfxhIProgress* me = reinterpret_cast<OfxhIProgress*>( effectInstance );
		if( !me )
			return kOfxStatErrBadHandle;
		
		bool v            = me->progressUpdate( progress );

		return v ? kOfxStatOK : kOfxStatReplyNo;
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

/// our progress suite
struct OfxProgressSuiteV1 gProgressSuite =
{
	ProgressStart,
	ProgressUpdate,
	ProgressEnd
};

void* getProgressSuite( const int version )
{
	if( version == 1 )
		return &gProgressSuite;
	return NULL;
}

}
}
}

