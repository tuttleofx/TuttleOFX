/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_FFTTRANSFORM_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFTTRANSFORM_PLUGIN_HPP_

#include "FftDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

struct FftTransformProcessParams
{
	EfftType _type;
};

/**
 * @brief FftTransform plugin
 */
class FftPlugin : public ImageEffectGilPlugin
{
public:
	FftPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	FftTransformProcessParams getProcessParams() const;

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc; ///< Source image clip
	OFX::Clip* _clipDst; ///< Destination image clip
	OFX::ChoiceParam* _fftType;
	OFX::ChoiceParam* _fftTypeLong;
};

}
}
}
}

#endif
