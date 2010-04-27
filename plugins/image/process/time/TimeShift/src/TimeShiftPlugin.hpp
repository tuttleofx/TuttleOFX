#ifndef _TUTTLE_PLUGIN_TIMESHIFT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TIMESHIFT_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace timeShift {

/**
 * @brief TimeShift plugin
 */
class TimeShiftPlugin : public OFX::ImageEffect
{
public:
    TimeShiftPlugin( OfxImageEffectHandle handle );

public:
    void render( const OFX::RenderArguments &args );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _srcClip;       ///< Source image clip
    OFX::Clip* _dstClip;       ///< Destination image clip
    OFX::DoubleParam* _offset; ///< Time offset
};

}
}
}

#endif
