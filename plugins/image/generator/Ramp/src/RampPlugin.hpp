#ifndef _TUTTLE_PLUGIN_RAMP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_RAMP_PLUGIN_HPP_

#include "RampDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace ramp {

struct RampProcessParams
{
    bool direction;
    bool color;
};

/**
 * @brief Ramp plugin
 */
class RampPlugin : public GeneratorPlugin
{
public:
    RampPlugin( OfxImageEffectHandle handle );

public:
    RampProcessParams getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
    void render( const OFX::RenderArguments &args );
	
public:
    OFX::ChoiceParam* _direction;
    OFX::BooleanParam* _color;
};

}
}
}

#endif
