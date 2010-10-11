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
	void setupDestView( const OFX::RenderArguments& args );

public:
	BitDepthPlugin( OfxImageEffectHandle handle );

public:
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void render( const OFX::RenderArguments& args );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc;                 ///< Source image clip
	OFX::Clip* _clipDst;                 ///< Destination image clip
	OFX::ChoiceParam* _paramOutBitDepth;      ///< Output bit depth
};

}
}
}

#endif
