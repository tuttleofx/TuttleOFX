/**
 * @file BitDepthConvPlugin.hpp
 * @brief
 * @author
 * @date    08/01/10 17:46
 *
 */

#ifndef BITDEPTHCONV_PLUGIN_H
#define BITDEPTHCONV_PLUGIN_H

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepthConvert {

/**
 * @brief
 *
 */
class BitDepthConvPlugin : public OFX::ImageEffect
{
private:
	template<class sview_t>
	void setupDestView( const OFX::RenderArguments &args );

public:
    BitDepthConvPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;

public:
    virtual void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

protected:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip           *_srcClip;       ///< Source image clip
    OFX::Clip           *_dstClip;       ///< Destination image clip
	OFX::ChoiceParam    *_outBitDepth;   ///< Output bit depth
};

}
}
}

#endif  // BITDEPTHCONV_PLUGIN_H
