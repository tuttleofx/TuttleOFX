#include "AnisotropicTensorsDefinition.hpp"
#include "AnisotropicTensorsPlugin.hpp"
#include "AnisotropicTensorsPluginFactory.hpp"
#include "AnisotropicTensorsProcess.hpp"

#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

using namespace boost::gil;

TensorsPlugin::TensorsPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramDisplayMargin = fetchBooleanParam(kParamDisplayEffectMargin);
    _paramAlpha = fetchDoubleParam(kParamAlpha);
    _paramSigma = fetchDoubleParam(kParamSigma);
}

int TensorsPlugin::getMargin()
{
    // Compute newton approximation to get the margin pixel size (alpha)
    float a = 1.695f / _paramAlpha->getValue();
    float k = std::pow((1.0f - std::exp(-a)), 2.0f) / (1.0f + 2.0f * a * std::exp(-a) - std::exp(-2.0f * a));
    float x = 1.5f;
    // S is the computation precision
    const float S = 0.000001f;
    // 20 iterations should be enough
    for(int n = 1; n <= 20; ++n)
    {
        x = x + 1.0f - (1.0f / (1.0f - x)) + (S / (k * std::exp(1.0f))) * std::exp(x) / (1.0f - x);
    }
    int fAdd1 = (int)(std::max((x - 1.0f) / a, 2.0f));
    // Computation for sigma
    a = static_cast<float>(1.695f / _paramSigma->getValue());
    k = std::pow((1.0f - std::exp(-a)), 2.0f) / (1.0f + 2.0f * a * std::exp(-a) - std::exp(-2.0f * a));
    x = 1.5f;
    // Compute newton approximation to get the margin pixel size (sigma)
    for(int n = 1; n <= 20; ++n)
    {
        x = x + 1.0f - (1.0f / (1.0f - x)) + (S / (k * std::exp(1.0f))) * std::exp(x) / (1.0f - x);
    }
    int fAdd2 = (int)(std::max((x - 1.0f) / a, 2.0f));
    // fAdd is the margin in pixels
    int margin = fAdd1 + fAdd2;
    if(margin < 0)
        margin = 0;
    return margin;
}

void TensorsPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois)
{
    const double margin = (double)getMargin() * _clipSrc->getPixelAspectRatio();

    _overSizedRect.x1 = double(args.regionOfInterest.x1 - margin);
    _overSizedRect.y1 = double(args.regionOfInterest.y1 - margin);
    _overSizedRect.x2 = double(args.regionOfInterest.x2 + margin + 1);
    _overSizedRect.y2 = double(args.regionOfInterest.y2 + margin + 1);

    rois.setRegionOfInterest(*_clipSrc, _overSizedRect);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void TensorsPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<AnisotropicTensorsProcess>(*this, args);
}
}
}
}
}
