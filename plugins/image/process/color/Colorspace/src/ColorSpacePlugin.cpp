
#include "ColorSpacePlugin.hpp"
#include "ColorSpaceProcess.hpp"
#include "ColorSpaceDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorspace
{

using namespace boost::gil;

ColorSpacePlugin::ColorSpacePlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramInGradationLaw = fetchChoiceParam(kColorSpaceGradationLawIn);
    _paramOutGradationLaw = fetchChoiceParam(kColorSpaceGradationLawOut);
    _paramInGamma = fetchDoubleParam(kColorSpaceInGammaValue);
    _paramOutGamma = fetchDoubleParam(kColorSpaceOutGammaValue);
    _paramInBlackPoint = fetchDoubleParam(kColorSpaceInBlackPoint);
    _paramOutBlackPoint = fetchDoubleParam(kColorSpaceOutBlackPoint);
    _paramInWhitePoint = fetchDoubleParam(kColorSpaceInWhitePoint);
    _paramOutWhitePoint = fetchDoubleParam(kColorSpaceOutWhitePoint);
    _paramInGammaSensito = fetchDoubleParam(kColorSpaceInGammaSensito);
    _paramOutGammaSensito = fetchDoubleParam(kColorSpaceOutGammaSensito);

    _paramInLayout = fetchChoiceParam(kColorSpaceLayoutIn);
    _paramOutLayout = fetchChoiceParam(kColorSpaceLayoutOut);
    _paramInColorTemp = fetchChoiceParam(kColorSpaceTempColorIn);
    _paramOutColorTemp = fetchChoiceParam(kColorSpaceTempColorOut);

    updateInParams();
    updateOutParams();
}

void ColorSpacePlugin::updateInParams()
{
    _paramInGamma->setIsSecretAndDisabled(true);
    _paramInBlackPoint->setIsSecretAndDisabled(true);
    _paramInWhitePoint->setIsSecretAndDisabled(true);
    _paramInGammaSensito->setIsSecretAndDisabled(true);

    switch(_paramInGradationLaw->getValue())
    {
        case ttlc::eParamGamma:
            _paramInGamma->setIsSecretAndDisabled(false);
            break;
        case ttlc::eParamCineon:
            _paramInBlackPoint->setIsSecretAndDisabled(false);
            _paramInWhitePoint->setIsSecretAndDisabled(false);
            _paramInGammaSensito->setIsSecretAndDisabled(false);
            break;
        default:
            break;
    }
}

void ColorSpacePlugin::updateOutParams()
{
    _paramOutGamma->setIsSecretAndDisabled(true);
    _paramOutBlackPoint->setIsSecretAndDisabled(true);
    _paramOutWhitePoint->setIsSecretAndDisabled(true);
    _paramOutGammaSensito->setIsSecretAndDisabled(true);

    switch(_paramOutGradationLaw->getValue())
    {
        case ttlc::eParamGamma:
            _paramOutGamma->setIsSecretAndDisabled(false);
            break;
        case ttlc::eParamCineon:
            _paramOutBlackPoint->setIsSecretAndDisabled(false);
            _paramOutWhitePoint->setIsSecretAndDisabled(false);
            _paramOutGammaSensito->setIsSecretAndDisabled(false);
            break;
        default:
            break;
    }
}

ColorSpaceProcessParams ColorSpacePlugin::getProcessParams() const
{
    using namespace tuttle::plugin::color;
    ColorSpaceProcessParams params;

    params._gradationIn = static_cast<EParamGradationLaw>(_paramInGradationLaw->getValue());

    if(params._gradationIn == ttlc::eParamGamma)
    {
        params._sGammaIn.value = _paramInGamma->getValue();
    }

    if(params._gradationIn == ttlc::eParamCineon)
    {
        params._sCineonIn.blackPoint = _paramInBlackPoint->getValue();
        params._sCineonIn.whitePoint = _paramInWhitePoint->getValue();
        params._sCineonIn.gammaSensito = _paramInGammaSensito->getValue();
    }

    params._gradationOut = static_cast<EParamGradationLaw>(_paramOutGradationLaw->getValue());

    if(params._gradationOut == ttlc::eParamGamma)
    {
        params._sGammaOut.value = _paramOutGamma->getValue();
    }

    if(params._gradationOut == ttlc::eParamCineon)
    {
        params._sCineonOut.blackPoint = _paramOutBlackPoint->getValue();
        params._sCineonOut.whitePoint = _paramOutWhitePoint->getValue();
        params._sCineonOut.gammaSensito = _paramOutGammaSensito->getValue();
    }

    params._layoutIn = static_cast<EParamLayout>(_paramInLayout->getValue());
    params._layoutOut = static_cast<EParamLayout>(_paramOutLayout->getValue());

    params._tempColorIn = static_cast<EColorTemperature>(_paramInColorTemp->getValue());
    params._tempColorOut = static_cast<EColorTemperature>(_paramOutColorTemp->getValue());

    return params;
}

bool ColorSpacePlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    /*
    ColorSpaceProcessParams params = getProcessParams();
    if(
            params._BlackPointIn	== params._BlackPointOut &&
            params._GammaSensitoIn	== params._GammaSensitoOut &&
            params._GammaValueIn	== params._GammaValueOut &&
            params._WhitePointIn	== params._WhitePointOut &&
            params._gradationIn	== params._gradationOut &&
            params._layoutIn	== params._layoutOut &&
            params._tempColorIn	== params._tempColorOut
     )
    {
            identityClip = _clipSrc;
            identityTime = args.time;
            return true;
    }*/
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorSpacePlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ColorSpaceProcess>(*this, args);
}

void ColorSpacePlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kColorSpaceGradationLawIn)
    {
        updateInParams();
    }
    else if(paramName == kColorSpaceGradationLawOut)
    {
        updateOutParams();
    }
}
}
}
}
