#include "ColorWheelPlugin.hpp"
#include "ColorWheelProcess.hpp"
#include "ColorWheelDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorWheel {


ColorWheelPlugin::ColorWheelPlugin( OfxImageEffectHandle handle )
: GeneratorPlugin( handle )
{
}

ColorWheelProcessParams ColorWheelPlugin::getProcessParams( ) const
{
	ColorWheelProcessParams params;
	return params;
}

void ColorWheelPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	GeneratorPlugin::getClipPreferences( clipPreferences );
}


/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorWheelPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<ColorWheelProcess>( *this, args );
}


}
}
}
