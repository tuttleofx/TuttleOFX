#ifndef _TUTTLE_PLUGIN_COLORWHEEL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORWHEEL_PLUGIN_HPP_

#include "ColorWheelDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorWheel
{

struct ColorWheelProcessParams
{
    EColorWheelMode mode;
};

/**
 * @brief ColorWheel plugin
 */
class ColorWheelPlugin : public GeneratorPlugin
{
public:
    ColorWheelPlugin(OfxImageEffectHandle handle);

public:
    ColorWheelProcessParams getProcessParams() const;

    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);
    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _mode;
};
}
}
}

#endif
