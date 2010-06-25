/**
 * @file Log2LinPlugin.hpp
 * @brief
 * @author
 * @date    08/01/10 15:39
 *
 */

#ifndef LOG2LIN_PLUGIN_H
#define LOG2LIN_PLUGIN_H

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

/**
 * @brief
 *
 */
class Log2LinPlugin : public OFX::ImageEffect
{
public:
    Log2LinPlugin( OfxImageEffectHandle handle );
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

#endif  // LOG2LIN_PLUGIN_H
