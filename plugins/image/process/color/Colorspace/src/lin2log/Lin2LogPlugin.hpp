/**
 * @file Lin2LogPlugin.hpp
 * @brief
 * @author
 * @date    08/01/10 15:19
 *
 */

#ifndef LIN2LOG_PLUGIN_H
#define LIN2LOG_PLUGIN_H

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

/**
 * @brief
 *
 */
class Lin2LogPlugin : public OFX::ImageEffect
{
public:
    Lin2LogPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;

public:
    virtual void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

protected:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip           *_clipSrc;       ///< Source image clip
    OFX::Clip           *_clipDst;       ///< Destination image clip
};

}
}
}
}

#endif  // LIN2LOG_PLUGIN_H
