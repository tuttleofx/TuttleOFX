
#include "PrintPlugin.hpp"
#include "PrintProcess.hpp"
#include "PrintDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace print
{

PrintPlugin::PrintPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramMode = fetchChoiceParam(kParamMode);
    _paramOutput = fetchChoiceParam(kParamOutput);
    _paramColor = fetchChoiceParam(kParamColor);
    _paramPixel = fetchInt2DParam(kParamPixel);
    _paramRegionMin = fetchInt2DParam(kParamRegionMin);
    _paramRegionMax = fetchInt2DParam(kParamRegionMax);
    _paramOutputColumns = fetchIntParam(kParamColumns);
    _paramFlip = fetchBooleanParam(kParamFlip);
    _paramOpenGlWindow = fetchBooleanParam(kParamOutputOpenGL);

    updateParams();
}

void PrintPlugin::updateParams()
{
    _paramPixel->setIsSecretAndDisabled(true);
    _paramRegionMin->setIsSecretAndDisabled(true);
    _paramRegionMax->setIsSecretAndDisabled(true);
    if(_paramMode->getValue() == eParamModeRegion)
    {
        _paramRegionMin->setIsSecretAndDisabled(false);
        _paramRegionMax->setIsSecretAndDisabled(false);
    }
    if(_paramMode->getValue() == eParamModePixel)
    {
        _paramPixel->setIsSecretAndDisabled(false);
    }
}

PrintProcessParams<PrintPlugin::Scalar> PrintPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    PrintProcessParams<Scalar> params;

    params._mode = static_cast<EParamMode>(_paramMode->getValue());
    params._output = static_cast<EParamOutput>(_paramOutput->getValue());
    OfxPointI pix = _paramPixel->getValue();
    params._pixel.x = pix.x;
    params._pixel.y = pix.y;
    OfxPointI rMin = _paramRegionMin->getValue();
    OfxPointI rMax = _paramRegionMax->getValue();
    params._region.x1 = rMin.x;
    params._region.y1 = rMin.y;
    params._region.x2 = rMax.x;
    params._region.y2 = rMax.y;
    params._flip = _paramFlip->getValue();
    params._openGlViewer = _paramOpenGlWindow->getValue();
    params._colorMode = (EParamColor)_paramColor->getValue();
    params._cols = _paramOutputColumns->getValue();
    return params;
}

void PrintPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    updateParams();
}

bool PrintPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PrintPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<PrintProcess>(*this, args);
}
}
}
}
