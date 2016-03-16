#ifndef _TUTTLE_PLUGIN_COLORTRANSFER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFER_PLUGIN_HPP_

#include "ColorTransferDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransfer
{

template <typename Scalar>
struct ColorTransferProcessParams
{
    EColorspace _colorspace;
    Scalar _averageCoef;
    Scalar _dynamicCoef;
};

/**
 * @brief ColorTransfer plugin
 */
class ColorTransferPlugin : public ImageEffectGilPlugin
{
public:
    typedef double Scalar;

public:
    ColorTransferPlugin(OfxImageEffectHandle handle);

public:
    ColorTransferProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);

    void render(const OFX::RenderArguments& args);

public:
    OFX::Clip* _clipSrcRef; ///< Source reference
    OFX::Clip* _clipDstRef; ///< Dest reference

    OFX::ChoiceParam* _paramColospace;
    OFX::DoubleParam* _paramAverageCoef;
    OFX::DoubleParam* _paramDynamicCoef;
};
}
}
}

#endif
