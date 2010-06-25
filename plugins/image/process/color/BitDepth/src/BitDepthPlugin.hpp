#ifndef _BITDEPTH_PLUGIN_HPP_
#define _BITDEPTH_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepth {

/**
 * @brief
 *
 */
class BitDepthPlugin : public OFX::ImageEffect
{
private:
	template<class sview_t>
	void setupDestView( const OFX::RenderArguments &args );

public:
    BitDepthPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;

public:
    virtual void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

protected:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip           *_clipSrc;       ///< Source image clip
    OFX::Clip           *_clipDst;       ///< Destination image clip
	OFX::ChoiceParam    *_outBitDepth;   ///< Output bit depth
};

}
}
}

#endif
