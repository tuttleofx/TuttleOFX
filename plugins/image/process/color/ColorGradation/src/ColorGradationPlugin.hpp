#ifndef _TUTTLE_PLUGIN_COLORGRADATION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORGRADATION_PLUGIN_HPP_

#include "ColorGradationDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorGradation
{

template <typename Scalar>
struct ColorGradationProcessParams
{
    EParamGradation _in;
    EParamGradation _out;

    double _GammaValueIn;
    double _BlackPointIn;
    double _WhitePointIn;
    double _GammaSensitoIn;

    double _GammaValueOut;
    double _BlackPointOut;
    double _WhitePointOut;
    double _GammaSensitoOut;

    bool _processAlpha;
};

/**
 * @brief ColorGradation plugin
 */
class ColorGradationPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    ColorGradationPlugin(OfxImageEffectHandle handle);

public:
    ColorGradationProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

private:
    void updateParameters();

public:
    OFX::ChoiceParam* _paramIn;
    OFX::ChoiceParam* _paramOut;

    OFX::DoubleParam* _paramInGamma;
    OFX::DoubleParam* _paramOutGamma;
    OFX::DoubleParam* _paramInBlackPoint;
    OFX::DoubleParam* _paramOutBlackPoint;
    OFX::DoubleParam* _paramInWhitePoint;
    OFX::DoubleParam* _paramOutWhitePoint;
    OFX::DoubleParam* _paramInGammaSensito;
    OFX::DoubleParam* _paramOutGammaSensito;

    OFX::BooleanParam* _paramProcessAlpha;
};
}
}
}

#endif
