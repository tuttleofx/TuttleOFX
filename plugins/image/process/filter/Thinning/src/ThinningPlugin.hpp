#ifndef _TUTTLE_PLUGIN_THINNING_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_THINNING_PLUGIN_HPP_

#include "ThinningDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

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
class ThinningPlugin : public OFX::ImageEffect
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
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip
	
    OFX::ChoiceParam* _paramBorder;
};

}
}
}

#endif
