#include "ColorSuppressPlugin.hpp"
#include "ColorSuppressProcess.hpp"
#include "ColorSuppressDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSuppress
{

using namespace boost::gil;

ColorSuppressPlugin::ColorSuppressPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramApplyOn = fetchChoiceParam(kParamApplyOn);
    _paramRedRate = fetchDoubleParam(kParamRedSuppressRate);
    _paramGreenRate = fetchDoubleParam(kParamGreenSuppressRate);
    _paramBlueRate = fetchDoubleParam(kParamBlueSuppressRate);
    _paramCyanRate = fetchDoubleParam(kParamCyanSuppressRate);
    _paramMagentaRate = fetchDoubleParam(kParamMagentaSuppressRate);
    _paramYellowRate = fetchDoubleParam(kParamYellowSuppressRate);
    _paramPreserveLuma = fetchBooleanParam(kParamPreserveLuma);
    _paramObeyAlpha = fetchBooleanParam(kParamObeyAlpha);
}

ColorSuppressProcessParams ColorSuppressPlugin::getProcessParams() const
{
    ColorSuppressProcessParams params;

    params.output = static_cast<EOutputType>(_paramApplyOn->getValue());
    params.red = _paramRedRate->getValue();
    params.blue = _paramBlueRate->getValue();
    params.green = _paramGreenRate->getValue();
    params.cyan = _paramCyanRate->getValue();
    params.magenta = _paramMagentaRate->getValue();
    params.yellow = _paramYellowRate->getValue();
    params.preserveLuma = _paramPreserveLuma->getValue();
    params.obeyAlpha = _paramObeyAlpha->getValue();
    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorSuppressPlugin::render(const OFX::RenderArguments& args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    const OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth();
    const OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                      << exception::user("Colorsuppress: BitDepthCustom not supported"));
                break;
            }
            case OFX::eBitDepthUShort:
            {
                BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                      << exception::user("Colorsuppress: BitDepthCustom not supported"));
                break;
            }
            case OFX::eBitDepthFloat:
            {
                doGilRender<ColorSuppressProcess, false, boost::gil::rgba_layout_t, boost::gil::bits32f>(*this, args);
                break;
            }
            case OFX::eBitDepthNone:
            case OFX::eBitDepthCustom:
                BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                      << exception::user("Colorsuppress: BitDepthCustom not recognize"));
                return;
        }
    }
}

void ColorSuppressPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kParamApplyOn)
    {
        switch(getOutputType())
        {
            case eOutputTypeImage:
            case eOutputTypeAlphaImage:
                _paramPreserveLuma->setEnabled(true);
                break;
            case eOutputTypeAlpha:
                _paramPreserveLuma->setEnabled(false);
                break;
        }
    }
}
}
}
}
