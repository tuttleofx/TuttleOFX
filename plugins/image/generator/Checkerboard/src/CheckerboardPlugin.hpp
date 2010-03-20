#ifndef _TUTTLE_PLUGIN_CHECKERBOARD_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CHECKERBOARD_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsParam.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace checkerboard {

/**
 * @brief
 *
 */
class CheckerboardPlugin : public OFX::ImageEffect
{
public:
    CheckerboardPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;

public:
    virtual void render( const OFX::RenderArguments &args );
//    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip           *_srcClip;       ///< Source image clip
    OFX::Clip           *_dstClip;       ///< Destination image clip
	OFX::Int2DParam     *_boxes;
	OFX::RGBAParam      *_color1;
	OFX::RGBAParam      *_color2;
};

}
}
}

#endif
