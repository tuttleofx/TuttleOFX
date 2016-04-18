#include "ColorCubePlugin.hpp"
#include "ColorCubeProcess.hpp"
#include "ColorCubeDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorCube
{

ColorCubePlugin::ColorCubePlugin(OfxImageEffectHandle handle)
    : GeneratorPlugin(handle)
{
    _step = fetchChoiceParam(kColorCubeStep);
}

ColorCubeProcessParams ColorCubePlugin::getProcessParams() const
{
    ColorCubeProcessParams params;
    int value = 1;
    value = value << (_step->getValue() + 2);
    params.step = value;
    return params;
}

void ColorCubePlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    GeneratorPlugin::getClipPreferences(clipPreferences);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorCubePlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ColorCubeProcess>(*this, args);
}
}
}
}
