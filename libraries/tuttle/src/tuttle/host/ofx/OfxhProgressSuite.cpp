#include "OfxhProgressSuite.hpp"
#include "OfxhProgress.hpp"


namespace tuttle {
namespace host {
namespace ofx {

static OfxStatus ProgressStart( void*       effectInstance,
                                const char* label )
{
	OfxhIProgress* me = reinterpret_cast<OfxhIProgress*>( effectInstance );

	me->progressStart( label );
	return kOfxStatOK;
}

/// finish progressing

static OfxStatus ProgressEnd( void* effectInstance )
{
	OfxhIProgress* me = reinterpret_cast<OfxhIProgress*>( effectInstance );

	me->progressEnd();
	return kOfxStatOK;
}

/// update progressing

static OfxStatus ProgressUpdate( void* effectInstance, double progress )
{
	OfxhIProgress* me = reinterpret_cast<OfxhIProgress*>( effectInstance );
	bool v       = me->progressUpdate( progress );

	return v ? kOfxStatOK : kOfxStatReplyNo;
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

