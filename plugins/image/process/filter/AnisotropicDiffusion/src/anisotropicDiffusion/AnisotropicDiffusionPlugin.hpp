#ifndef _TUTTLE_PLUGIN_PDE_DENOISER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PDE_DENOISER_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace diffusion
{

/**
 * @brief Class used to denoise with partial derivated equations
 *
 */
class AnisotropicDiffusionPlugin : public ImageEffectGilPlugin
{
public:
    AnisotropicDiffusionPlugin(OfxImageEffectHandle handle);

public:
    int getMargin();

    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);

    void render(const OFX::RenderArguments& args);

public:
    // do not need to delete these, the ImageEffect is managing them for us
    OfxRectD _overSizedRect;
    OFX::RGBParam* _paramAmplitude; ///< Amplitude control parameter

    OFX::Clip* _clipSrcTensors; ///< Tensors source image clip
};
}
}
}
}

#endif
