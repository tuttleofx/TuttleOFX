#include "ColorGradationPlugin.hpp"
#include "ColorGradationProcess.hpp"
#include "ColorGradationDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorGradation
{

ColorGradationPlugin::ColorGradationPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramIn = fetchChoiceParam(kParamIn);
    _paramOut = fetchChoiceParam(kParamOut);

    _paramInGamma = fetchDoubleParam(kColorSpaceInGammaValue);
    _paramOutGamma = fetchDoubleParam(kColorSpaceOutGammaValue);
    _paramInBlackPoint = fetchDoubleParam(kColorSpaceInBlackPoint);
    _paramOutBlackPoint = fetchDoubleParam(kColorSpaceOutBlackPoint);
    _paramInWhitePoint = fetchDoubleParam(kColorSpaceInWhitePoint);
    _paramOutWhitePoint = fetchDoubleParam(kColorSpaceOutWhitePoint);
    _paramInGammaSensito = fetchDoubleParam(kColorSpaceInGammaSensito);
    _paramOutGammaSensito = fetchDoubleParam(kColorSpaceOutGammaSensito);

    _paramProcessAlpha = fetchBooleanParam(kParamProcessAlpha);
    updateParameters();
}

ColorGradationProcessParams<ColorGradationPlugin::Scalar>
ColorGradationPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    ColorGradationProcessParams<Scalar> params;
    params._in = static_cast<EParamGradation>(_paramIn->getValue());
    params._out = static_cast<EParamGradation>(_paramOut->getValue());

    params._GammaValueIn = _paramInGamma->getValue();
    params._BlackPointIn = _paramInBlackPoint->getValue();
    params._WhitePointIn = _paramInWhitePoint->getValue();
    params._GammaSensitoIn = _paramInGammaSensito->getValue();

    params._GammaValueOut = _paramOutGamma->getValue();
    params._BlackPointOut = _paramOutBlackPoint->getValue();
    params._WhitePointOut = _paramOutWhitePoint->getValue();
    params._GammaSensitoOut = _paramOutGammaSensito->getValue();

    params._processAlpha = _paramProcessAlpha->getValue();
    return params;
}

void ColorGradationPlugin::updateParameters()
{
    _paramInGamma->setIsSecretAndDisabled(true);
    _paramInBlackPoint->setIsSecretAndDisabled(true);
    _paramInWhitePoint->setIsSecretAndDisabled(true);
    _paramInGammaSensito->setIsSecretAndDisabled(true);
    _paramOutGamma->setIsSecretAndDisabled(true);
    _paramOutBlackPoint->setIsSecretAndDisabled(true);
    _paramOutWhitePoint->setIsSecretAndDisabled(true);
    _paramOutGammaSensito->setIsSecretAndDisabled(true);
    switch((EParamGradation)_paramIn->getValue())
    {
        case eParamGradation_gamma: // gamma
            _paramInGamma->setIsSecretAndDisabled(false);
            break;
        case eParamGradation_cineon: // cineon
            _paramInBlackPoint->setIsSecretAndDisabled(false);
            _paramInWhitePoint->setIsSecretAndDisabled(false);
            _paramInGammaSensito->setIsSecretAndDisabled(false);
            break;
        default:
            break;
    }
    switch((EParamGradation)_paramOut->getValue())
    {
        case eParamGradation_gamma: // gamma
            _paramOutGamma->setIsSecretAndDisabled(false);
            break;
        case eParamGradation_cineon: // cineon
            _paramOutBlackPoint->setIsSecretAndDisabled(false);
            _paramOutWhitePoint->setIsSecretAndDisabled(false);
            _paramOutGammaSensito->setIsSecretAndDisabled(false);
            break;
        default:
            break;
    }
}

void ColorGradationPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kParamInvert)
    {
        int in = _paramIn->getValue();
        _paramIn->setValue(_paramOut->getValue());
        _paramOut->setValue(in);
    }
    updateParameters();
}

bool ColorGradationPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    /*
    ColorGradationProcessParams<Scalar> params = getProcessParams();
    if( params._in == params._out )
    {
            if( ( params._in == 2 ) &&
                ( params._GammaValueIn != params._GammaValueOut ) )
                    return false;
            if( ( params._in == 3 ) &&
                ( params._BlackPointIn != params._BlackPointOut ) &&
                ( params._WhitePointIn != params._WhitePointOut ) &&
                ( params._GammaSensitoIn != params._GammaSensitoOut ) )
                    return false;
            identityClip = _clipSrc;
            identityTime = args.time;
            return true;
    }
    */
    return false; // by default, we are not an identity operation
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorGradationPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ColorGradationProcess>(*this, args);
}
}
}
}
