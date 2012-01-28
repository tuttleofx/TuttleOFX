#ifndef _TUTTLE_PLUGIN_CONSTANT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONSTANT_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace constant {

/**
 * @brief
 */
class ConstantPlugin : public ImageEffectGilPlugin
{
public:
        ConstantPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	//    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

public:
        OFX::RGBAParam* _color;
};

}
}
}

#endif
