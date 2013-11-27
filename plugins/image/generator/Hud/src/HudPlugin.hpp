#ifndef _TUTTLE_PLUGIN_HUD_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HUD_PLUGIN_HPP_

#include "HudDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace hud {

template<typename Pixel>
struct HudProcessParams
{
	Pixel _color;
};

/**
 * @brief Hud plugin
 */
class HudPlugin : public ImageEffectGilPlugin
{

public:
    HudPlugin( OfxImageEffectHandle handle );

public:
	template<class Pixel>
	HudProcessParams<Pixel> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip
	OFX::RGBAParam* _color;
};

template<class Pixel>
HudProcessParams<Pixel> HudPlugin::getProcessParams( const OfxPointD& renderScale ) const{
	HudProcessParams<Pixel> params;
	boost::gil::color_convert( ofxToGil( _color->getValue() ), params._color );
	return params;
}

}
}
}

#endif
