#include "AnisotropicDiffusionDefinition.hpp"
#include "AnisotropicDiffusionPlugin.hpp"
#include "AnisotropicDiffusionPluginFactory.hpp"
#include "AnisotropicDiffusionProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace diffusion
{

AnisotropicDiffusionPlugin::AnisotropicDiffusionPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _clipSrcTensors = fetchClip(kClipInputTensors);

    _paramAmplitude = fetchRGBParam(kParamAmplitude);
}

int AnisotropicDiffusionPlugin::getMargin()
{
    OfxRGBColourD color = _paramAmplitude->getValue();
    return (int)std::ceil(
        std::sqrt(2.0f) *
        std::ceil(std::max(std::max(std::sqrt(2.0f * color.r), std::sqrt(2.0f * color.g)), std::sqrt(2.0f * color.b))));
}

void AnisotropicDiffusionPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args,
                                                      OFX::RegionOfInterestSetter& rois)
{
    const double margin = (double)getMargin() * _clipSrc->getPixelAspectRatio();

    _overSizedRect.x1 = double(args.regionOfInterest.x1 - margin);
    _overSizedRect.y1 = double(args.regionOfInterest.y1 - margin);
    _overSizedRect.x2 = double(args.regionOfInterest.x2 + margin + 1);
    _overSizedRect.y2 = double(args.regionOfInterest.y2 + margin + 1);

    rois.setRegionOfInterest(*_clipSrc, _overSizedRect);
    rois.setRegionOfInterest(*_clipSrcTensors, _overSizedRect);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AnisotropicDiffusionPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<AnisotropicDiffusionProcess>(*this, args);
}
}
}
}
}
