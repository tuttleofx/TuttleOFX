#ifndef _TUTTLE_PLUGIN_FLOODFILL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FLOODFILL_PLUGIN_HPP_

#include "FloodFillDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace floodFill {

template<typename Scalar>
struct FloodFillProcessParams
{
	Scalar _upperThres;
	Scalar _lowerThres;
	bool _relativeMinMax;
	EParamMethod _method;
};

/**
 * @brief FloodFill plugin
 */
class FloodFillPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    FloodFillPlugin( OfxImageEffectHandle handle );

public:
	FloodFillProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

//	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
//	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip

    OFX::DoubleParam* _paramUpperThres;
    OFX::DoubleParam* _paramLowerThres;
    OFX::BooleanParam* _paramRelativeMinMax;
    OFX::ChoiceParam* _paramMethod;
};

}
}
}

#endif
