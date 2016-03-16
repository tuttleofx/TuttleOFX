#include "ColorBarsPlugin.hpp"
#include "ColorBarsProcess.hpp"
#include "ColorBarsDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorBars
{

ColorBarsPlugin::ColorBarsPlugin(OfxImageEffectHandle handle)
    : GeneratorPlugin(handle)
{
    mode = fetchChoiceParam(kColorBarsLevels);
}

ColorBarsParams ColorBarsPlugin::getProcessParams() const
{
    ColorBarsParams params;

    params.mode = static_cast<EColorBarsLevels>(mode->getValue());
    return params;
}

void ColorBarsPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    GeneratorPlugin::getClipPreferences(clipPreferences);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorBarsPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ColorBarsProcess>(*this, args);
}
}
}
}
