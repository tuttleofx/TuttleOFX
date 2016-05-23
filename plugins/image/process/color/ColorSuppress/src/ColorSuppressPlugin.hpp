#ifndef _TUTTLE_PLUGIN_COLORSUPPRESS_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORSUPPRESS_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/common/utils/global.hpp>

#include "ColorSuppressDefinitions.hpp"

namespace tuttle
{
namespace plugin
{
namespace colorSuppress
{

struct ColorSuppressProcessParams
{
    EOutputType output;
    double red, green, blue, cyan, magenta, yellow;
    bool obeyAlpha;
    bool preserveLuma;
};

/**
 * @brief ColorSuppress plugin
 */
class ColorSuppressPlugin : public ImageEffectGilPlugin
{

public:
    ColorSuppressPlugin(OfxImageEffectHandle handle);

public:
    void render(const OFX::RenderArguments& args);
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    ColorSuppressProcessParams getProcessParams() const;

public:
    OFX::ChoiceParam* _paramApplyOn;
    OFX::DoubleParam* _paramRedRate;
    OFX::DoubleParam* _paramGreenRate;
    OFX::DoubleParam* _paramBlueRate;
    OFX::DoubleParam* _paramCyanRate;
    OFX::DoubleParam* _paramMagentaRate;
    OFX::DoubleParam* _paramYellowRate;
    OFX::BooleanParam* _paramObeyAlpha;
    OFX::BooleanParam* _paramPreserveLuma;

    EOutputType getOutputType() const { return static_cast<EOutputType>(_paramApplyOn->getValue()); }
};
}
}
}

#endif
