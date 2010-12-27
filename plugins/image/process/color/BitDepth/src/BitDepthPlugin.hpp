#ifndef _BITDEPTH_PLUGIN_HPP_
#define _BITDEPTH_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepth {

/**
 * @brief
 *
 */
class BitDepthPlugin : public ImageEffectGilPlugin
{
public:
	BitDepthPlugin( OfxImageEffectHandle handle );

public:
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void render( const OFX::RenderArguments& args );

private:
	template<class sview_t>
	void setupDestView( const OFX::RenderArguments& args );

public:
	OFX::ChoiceParam* _paramOutBitDepth;      ///< Output bit depth
};

}
}
}

#endif
