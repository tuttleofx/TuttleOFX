#include "RampPlugin.hpp"
#include "RampProcess.hpp"
#include "RampDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace ramp {


RampPlugin::RampPlugin( OfxImageEffectHandle handle )
: GeneratorPlugin( handle )
{
    _direction = fetchChoiceParam( kRampDirection );
    _color = fetchBooleanParam( kRampColor );
}

RampProcessParams RampPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
    RampProcessParams params;

    params.direction = ( _direction->getValue() == 0 );
    params.color = _color->getValue();

	return params;
}

void RampPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
    GeneratorPlugin::getClipPreferences( clipPreferences );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void RampPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<RampProcess>( *this, args );
}


}
}
}
