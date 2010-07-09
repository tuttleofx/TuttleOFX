#ifndef _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace invert {

/**
 * @brief
 *
 */
class InvertPlugin : public OFX::ImageEffect
{
public:
	InvertPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc;      ///< Source image clip
	OFX::Clip* _clipDst;      ///< Destination image clip
};

}
}
}

#endif
