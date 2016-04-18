#ifndef _TUTTLE_PLUGIN_CONSTANT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONSTANT_PLUGIN_HPP_

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace constant
{

/**
 * @brief
 */
class ConstantPlugin : public GeneratorPlugin
{
public:
    ConstantPlugin(OfxImageEffectHandle handle);

public:
    void render(const OFX::RenderArguments& args);
    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);

public:
    OFX::RGBAParam* _color;
};
}
}
}

#endif
