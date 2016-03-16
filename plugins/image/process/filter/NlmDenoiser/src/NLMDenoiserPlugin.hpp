#ifndef _TUTTLE_PLUGIN_NLMDENOISER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_NLMDENOISER_PLUGIN_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace nlmDenoiser
{

/**
 * \class NLMDenoiserPlugin
 * \brief Class used to denoise with partial derivated equations
 *
 */
class NLMDenoiserPlugin : public OFX::ImageEffect
{
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipDst; ///< Destination image clip
    OFX::Clip* _clipSrc; ///< Source image clip

    OFX::IntParam* _paramDepth;
    OFX::IntParam* _paramRegionRadius;
    OFX::IntParam* _paramPatchRadius;

public:
    NLMDenoiserPlugin(OfxImageEffectHandle handle);

    void getFramesNeeded(const OFX::FramesNeededArguments& args, OFX::FramesNeededSetter& frames);
    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);

    void render(const OFX::RenderArguments& args);
};
}
}
}

#endif
