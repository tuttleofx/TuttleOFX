#include "OfxhTimelineSuite.hpp"
#include "OfxhTimeline.hpp"


namespace tuttle {
namespace host {
namespace ofx {

/**
 * timeline suite function
 */
static OfxStatus TimelineGetTime( void* effectInstance, double* time )
{
	OfxhITimeline* me = reinterpret_cast<OfxhITimeline*>( effectInstance );

	*time = me->timelineGetTime();
	return kOfxStatOK;
}

/**
 * timeline suite function
 */
static OfxStatus TimelineGotoTime( void* effectInstance, double time )
{
	OfxhITimeline* me = reinterpret_cast<OfxhITimeline*>( effectInstance );

	me->timelineGotoTime( time );
	return kOfxStatOK;
}

/**
 * timeline suite function
 */
static OfxStatus TimelineGetBounds( void* effectInstance, double* firstTime, double* lastTime )
{
	OfxhITimeline* me = reinterpret_cast<OfxhITimeline*>( effectInstance );

	me->timelineGetBounds( *firstTime, *lastTime );
	return kOfxStatOK;
}

/**
 * our progress suite
 */
struct OfxTimeLineSuiteV1 gTimelineSuite =
{
	TimelineGetTime,
	TimelineGotoTime,
	TimelineGetBounds
};


void* getTimelineSuite( const int version )
{
    if( version == 1 )
        return &gTimelineSuite;
    return NULL;
}

}
}
}

