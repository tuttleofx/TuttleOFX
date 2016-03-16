#ifndef _TUTTLE_PLUGIN_TENSORS_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TENSORS_PLUGIN_HPP_

#include "AnisotropicTensorsMargin.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

/**
 * @class TensorsPlugin
 * @brief Class used to render the effect
 */
class TensorsPlugin : public ImageEffectGilPlugin
{
public:
    TensorsPlugin(OfxImageEffectHandle handle);
    int getMargin();

    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);

    void render(const OFX::RenderArguments& args);

public:
    OFX::BooleanParam* _paramDisplayMargin; ///< Display margin boolean
    OFX::DoubleParam* _paramAlpha;
    OFX::DoubleParam* _paramSigma;

    OfxRectD _renderRect;    ///< Render zone
    OfxRectD _overSizedRect; ///< Over sized render zone
};
}
}
}
}

#endif
