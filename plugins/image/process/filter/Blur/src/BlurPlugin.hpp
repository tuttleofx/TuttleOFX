#ifndef _TUTTLE_PLUGIN_BLUR_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_BLUR_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

struct BlurProcessParams
{
	boost::gil::point2<double> _size;
};

/**
 * @brief Blur plugin
 */
class BlurPlugin : public OFX::ImageEffect
{
public:
    BlurPlugin( OfxImageEffectHandle handle );

public:
    void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	
	BlurProcessParams getProcessParams() const;
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _srcClip; ///< Source image clip
    OFX::Clip* _dstClip; ///< Destination image clip

	OFX::Double2DParam* _paramSize;
};

}
}
}

#endif
