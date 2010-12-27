#ifndef _TUTTLE_PLUGIN_LIN2LOG_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_LIN2LOG_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

/**
 * @brief
 */
class Lin2LogPlugin : public ImageEffectGilPlugin
{
public:
	Lin2LogPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
};

}
}
}
}

#endif
