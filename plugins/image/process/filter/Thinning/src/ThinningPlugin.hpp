#ifndef _TUTTLE_PLUGIN_THINNING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_THINNING_PLUGIN_HPP_

#include "ThinningDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

template<typename Scalar>
struct ThinningProcessParams
{
	EParamBorder _border;
};

/**
 * @brief Thinning plugin
 */
class ThinningPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
	ThinningPlugin( OfxImageEffectHandle handle );

public:
	ThinningProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

	void render( const OFX::RenderArguments &args );
	
public:
	OFX::ChoiceParam* _paramBorder;
};

}
}
}

#endif
