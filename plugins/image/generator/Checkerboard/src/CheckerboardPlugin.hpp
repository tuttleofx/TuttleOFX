#ifndef _TUTTLE_PLUGIN_CHECKERBOARD_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CHECKERBOARD_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace checkerboard {

/**
 * @brief
 */
class CheckerboardPlugin : public ImageEffectGilPlugin
{
public:
	CheckerboardPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	//    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

public:
	OFX::Int2DParam* _boxes;
	OFX::RGBAParam* _color1;
	OFX::RGBAParam* _color2;
};

}
}
}

#endif
