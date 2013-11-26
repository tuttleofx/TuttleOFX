#ifndef _TUTTLE_PLUGIN_HUD_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_HUD_PLUGIN_HPP_

#include "HudDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace hud {

template<typename Scalar>
struct HudProcessParams
{
	
};

/**
 * @brief Hud plugin
 */
class HudPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    HudPlugin( OfxImageEffectHandle handle );

public:
	HudProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
//    OFX::Clip* _clipSrcMatte; ///< Matte source image clip
};

}
}
}

#endif
