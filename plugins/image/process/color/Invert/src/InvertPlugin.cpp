#include "InvertPlugin.hpp"
#include "InvertProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace invert
{

using namespace boost::gil;

InvertPlugin::InvertPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramProcessGroup = fetchGroupParam(kParamProcessGroup);
    _paramProcessR = fetchBooleanParam(kParamProcessR);
    _paramProcessG = fetchBooleanParam(kParamProcessG);
    _paramProcessB = fetchBooleanParam(kParamProcessB);
    _paramProcessA = fetchBooleanParam(kParamProcessA);
    _paramProcessGray = fetchBooleanParam(kParamProcessGray);
}

InvertProcessParams InvertPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    InvertProcessParams params;

    params._red = _paramProcessR->getValue();
    params._green = _paramProcessG->getValue();
    params._blue = _paramProcessB->getValue();
    params._alpha = _paramProcessA->getValue();
    params._gray = _paramProcessGray->getValue();

    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void InvertPlugin::render(const OFX::RenderArguments& args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    const OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    const OFX::EPixelComponent components = _clipDst->getPixelComponents();

    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            doGilRender<InvertProcess, false, boost::gil::rgba_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentRGB:
        {
            doGilRender<InvertProcess, false, boost::gil::rgb_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentAlpha:
        {
            doGilRender<InvertProcess, false, boost::gil::gray_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentCustom:
        case OFX::ePixelComponentNone:
        {
            BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Pixel components (" +
                                                                  mapPixelComponentEnumToString(components) +
                                                                  ") not supported by the plugin.");
        }
    }
    BOOST_THROW_EXCEPTION(exception::Unknown());
}
}
}
}
