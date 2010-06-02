#include "TimeShiftPlugin.hpp"
#include "TimeShiftDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace timeShift {

using namespace boost::gil;

TimeShiftPlugin::TimeShiftPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_offset = fetchDoubleParam( kOffset );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void TimeShiftPlugin::render( const OFX::RenderArguments &args )
{
}

bool TimeShiftPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	identityClip = _clipSrc;
	identityTime = args.time + _offset->getValue();
	return true;
}

}
}
}
