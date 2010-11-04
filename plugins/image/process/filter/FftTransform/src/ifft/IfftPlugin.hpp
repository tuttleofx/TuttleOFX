/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef _TUTTLE_PLUGIN_IFFT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IFFT_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

struct FftTransformProcessParams
{};

/**
 * @brief FftTransform plugin
 */
class IfftPlugin : public OFX::ImageEffect
{
public:
	IfftPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClipRe() const;
	OFX::Clip* getSrcClipIm() const;
	OFX::Clip* getDstClip() const;

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

	FftTransformProcessParams getProcessParams() const;

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrcMod; ///< Real part source image clip
	OFX::Clip* _clipSrcPhase; ///< Imaginary part source image clip
	OFX::Clip* _clipDst; ///< Destination image clip
};

}
}
}
}

#endif
