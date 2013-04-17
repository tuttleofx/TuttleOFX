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
	
	TUTTLE_TCOUT_X( 20, "TimeDomain--" );
	
    range = _clipSrc->getFrameRange();
	TUTTLE_TCOUT_VAR2( range.min, range.max );
	TUTTLE_TCOUT_VAR( params._offset );
	range.min += params._offset;
	range.max += params._offset;
	
	TUTTLE_TCOUT_VAR2( range.min, range.max );
	
	return true;
}

void TimeShiftPlugin::getFramesNeeded( const OFX::FramesNeededArguments &args, OFX::FramesNeededSetter &frames )
{
	TimeShiftProcessParams<TimeShiftPlugin::Scalar> params = getProcessParams();
	const OfxTime inTime = args.time - params._offset;
	const OfxRangeD range = { inTime, inTime };
	frames.setFramesNeeded( *_clipSrc, range );
	
	TUTTLE_TCOUT_X( 20, "FrameNeeded--" );
	TUTTLE_TCOUT_VAR( args.time );
	TUTTLE_TCOUT_VAR( params._offset );
	TUTTLE_TCOUT_VAR( inTime );
}

bool TimeShiftPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	identityClip = _clipSrc;

	identityTime = args.time - _offset->getValue();
	
	TUTTLE_TCOUT_X( 20, "isIdentity--" );
	TUTTLE_TCOUT_VAR( args.time );
	TUTTLE_TCOUT_VAR( _offset->getValue() );
	TUTTLE_TCOUT_VAR( identityTime );
	
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
