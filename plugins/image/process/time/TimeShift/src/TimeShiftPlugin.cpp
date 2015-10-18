#include "TimeShiftPlugin.hpp"
#include "TimeShiftDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace timeShift {

TimeShiftPlugin::TimeShiftPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	switch( getContext() )
	{
		case OFX::eContextRetimer:
			// @todo tuttle: do something else...
			_offset = fetchDoubleParam( kOfxImageEffectRetimerParamName );
			break;
		default:
			_offset = fetchDoubleParam( kParamOffset );
			break;
	}
}

TimeShiftProcessParams<TimeShiftPlugin::Scalar> TimeShiftPlugin::getProcessParams() const
{
	TimeShiftProcessParams<TimeShiftPlugin::Scalar> params;
	params._offset = _offset->getValue();
	return params;
}

bool TimeShiftPlugin::getTimeDomain( OfxRangeD& range )
{
	TimeShiftProcessParams<TimeShiftPlugin::Scalar> params = getProcessParams();
	if( params._offset == 0 )
		return false;
	
	TUTTLE_LOG_INFO( "[Time Shift] TimeDomain--" );
	
	range = _clipSrc->getFrameRange();
	TUTTLE_LOG_VAR2( TUTTLE_INFO, range.min, range.max );
	TUTTLE_LOG_VAR( TUTTLE_INFO, params._offset );
	range.min += params._offset;
	range.max += params._offset;
	
	TUTTLE_LOG_VAR2( TUTTLE_INFO, range.min, range.max );
	
	return true;
}

void TimeShiftPlugin::getFramesNeeded( const OFX::FramesNeededArguments &args, OFX::FramesNeededSetter &frames )
{
	TimeShiftProcessParams<TimeShiftPlugin::Scalar> params = getProcessParams();
	const OfxTime inTime = args.time - params._offset;
	const OfxRangeD range = { inTime, inTime };
	frames.setFramesNeeded( *_clipSrc, range );
	
	TUTTLE_LOG_INFO( "[Time Shift] FrameNeeded--" );
	TUTTLE_LOG_VAR( TUTTLE_INFO, args.time );
	TUTTLE_LOG_VAR( TUTTLE_INFO, params._offset );
	TUTTLE_LOG_VAR( TUTTLE_INFO, inTime );
}

bool TimeShiftPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	identityClip = _clipSrc;

	identityTime = args.time - _offset->getValue();
	
	TUTTLE_LOG_INFO( "[Time Shift] isIdentity--" );
	TUTTLE_LOG_VAR( TUTTLE_INFO, args.time );
	TUTTLE_LOG_VAR( TUTTLE_INFO, _offset->getValue() );
	TUTTLE_LOG_VAR( TUTTLE_INFO, identityTime );
	
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void TimeShiftPlugin::render( const OFX::RenderArguments& args )
{
}


}
}
}
