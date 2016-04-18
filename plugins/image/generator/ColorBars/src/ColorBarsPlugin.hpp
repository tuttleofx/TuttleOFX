#ifndef _TUTTLE_PLUGIN_COLORBARS_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORBARS_PLUGIN_HPP_

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include "ColorBarsDefinitions.hpp"

namespace tuttle
{
namespace plugin
{
namespace colorBars
{

struct ColorBarsParams
{
    EColorBarsLevels mode;
};

/**
 * @brief ColorBars plugin
 */
class ColorBarsPlugin : public GeneratorPlugin
{
public:
    ColorBarsPlugin(OfxImageEffectHandle handle);

public:
    ColorBarsParams getProcessParams() const;

    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);
    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* mode;
};
}
}
}

#endif
