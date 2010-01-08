#include "OfxhTimelineSuite.hpp"
#include "OfxhTimeline.hpp"
#include "OfxhException.hpp"

namespace tuttle {
namespace host {
namespace ofx {

/**
 * timeline suite function
 */
static OfxStatus TimelineGetTime( void* effectInstance, double* time )
{
	try
	{
		OfxhITimeline* me = reinterpret_cast<OfxhITimeline*>( effectInstance );
		if( !me )
			return kOfxStatErrBadHandle;

		*time = me->timelineGetTime();

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
 * timeline suite function
 */
static OfxStatus TimelineGotoTime( void* effectInstance, double time )
{
	try
	{
		OfxhITimeline* me = reinterpret_cast<OfxhITimeline*>( effectInstance );
		if( !me )
			return kOfxStatErrBadHandle;

		me->timelineGotoTime( time );

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
 * timeline suite function
 */
static OfxStatus TimelineGetBounds( void* effectInstance, double* firstTime, double* lastTime )
{
	try
	{
		OfxhITimeline* me = reinterpret_cast<OfxhITimeline*>( effectInstance );
		if( !me )
			return kOfxStatErrBadHandle;

		me->timelineGetBounds( *firstTime, *lastTime );

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
