#ifndef _TUTTLE_PLUGIN_FLIP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FLIP_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include "FlipDefinitions.hpp"

namespace tuttle
{
namespace plugin
{
namespace flip
{

struct FlipProcessParams
{

    bool flip;
    bool flop;
};

/**
 * @brief
 *
 */
class FlipPlugin : public ImageEffectGilPlugin
{

public:
    FlipPlugin(OfxImageEffectHandle handle);

public:
    FlipProcessParams getProcessParams(const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    OfxRectI getFlipRegionValue() const;
    OfxRectI computeFlipRegion(const OfxTime time, const bool fromRatio = false) const;
    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

private:
    OFX::BooleanParam* _paramFlip;
    OFX::BooleanParam* _paramFlop;
};
}
}
}

#endif
