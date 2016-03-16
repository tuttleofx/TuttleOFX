#ifndef _TUTTLE_PLUGIN_COLORCUBE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORCUBE_PLUGIN_HPP_

#include "ColorCubeDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorCube
{

struct ColorCubeProcessParams
{
    size_t step;
};

/**
 * @brief ColorCube plugin
 */
class ColorCubePlugin : public GeneratorPlugin
{
public:
    ColorCubePlugin(OfxImageEffectHandle handle);

public:
    ColorCubeProcessParams getProcessParams() const;

    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);
    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _step;
};
}
}
}

#endif
