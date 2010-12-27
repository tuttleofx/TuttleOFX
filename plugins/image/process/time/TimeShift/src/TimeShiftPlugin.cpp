#include "TimeShiftPlugin.hpp"
#include "TimeShiftDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace timeShift {

TimeShiftPlugin::TimeShiftPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_offset = fetchDoubleParam( kOffset );
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
	
    range = _clipSrc->getFrameRange();
	range.min += params._offset;
	range.max += params._offset;
	return true;
}

void TimeShiftPlugin::getFramesNeeded( const OFX::FramesNeededArguments &args, OFX::FramesNeededSetter &frames )
{
	TimeShiftProcessParams<TimeShiftPlugin::Scalar> params = getProcessParams();
	const OfxTime inTime = args.time + params._offset;
	const OfxRangeD range = { inTime, inTime };
	frames.setFramesNeeded( *_clipSrc, range );
}

bool TimeShiftPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	identityClip = _clipSrc;
	identityTime = args.time + _offset->getValue();
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
