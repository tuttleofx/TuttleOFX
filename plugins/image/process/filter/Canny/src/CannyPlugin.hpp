#ifndef _TUTTLE_PLUGIN_CANNY_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CANNY_PLUGIN_HPP_

#include "CannyDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace canny {

template<typename Scalar>
struct CannyProcessParams
{
	bool _hysteresis;
	double _upperThres;
	double _lowerThres;

	bool _fillAllChannels;

	EParamBorder _border;
};

/**
 * @brief Canny plugin
 */
class CannyPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    CannyPlugin( OfxImageEffectHandle handle );

public:
	CannyProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip

    OFX::BooleanParam* _paramHysteresis;
    OFX::DoubleParam* _paramUpperThres;
    OFX::DoubleParam* _paramLowerThres;
    OFX::BooleanParam* _paramFillAllChannels;
	OFX::ChoiceParam* _paramBorder;
};

}
}
}

#endif
