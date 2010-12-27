#ifndef _TUTTLE_PLUGIN_LOG2LIN_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_LOG2LIN_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

/**
 * @brief
 *
 */
class Log2LinPlugin : public ImageEffectGilPlugin
{
public:
	Log2LinPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
};

}
}
}
}

#endif
