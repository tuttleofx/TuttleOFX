#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

#include "ImageStatisticsDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace imageStatistics {

struct ImageStatisticsProcessParams
{
	OfxRectI _rect; ///< the selected rectangle, clipped to the image rod
	EParamChooseOutput _chooseOutput;
};

/**
 * @brief ImageStatistics plugin
 */
class ImageStatisticsPlugin : public OFX::ImageEffect
{
public:
    ImageStatisticsPlugin( OfxImageEffectHandle handle );

public:
    void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

    void getRegionsOfInterest( const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois );

	ImageStatisticsProcessParams getProcessParams( const OfxRectI& srcRod ) const;
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip
	
	OFX::ChoiceParam* _paramCoordinateSystem;
	OFX::Double2DParam* _paramRectCenter;
	OFX::Double2DParam* _paramRectSize;
	OFX::ChoiceParam* _paramChooseOutput;
	
	OFX::RGBAParam* _paramOutputAverage;
	OFX::RGBAParam* _paramOutputChannelMin;
	OFX::RGBAParam* _paramOutputChannelMax;
	OFX::RGBAParam* _paramOutputLuminosityMin;
	OFX::RGBAParam* _paramOutputLuminosityMax;
	OFX::RGBAParam* _paramOutputKurtosis;
	OFX::RGBAParam* _paramOutputSkewness;
	
	OFX::Double3DParam* _paramOutputAverageHSL;
	OFX::Double3DParam* _paramOutputChannelMinHSL;
	OFX::Double3DParam* _paramOutputChannelMaxHSL;
	OFX::Double3DParam* _paramOutputLuminosityMinHSL;
	OFX::Double3DParam* _paramOutputLuminosityMaxHSL;
	OFX::Double3DParam* _paramOutputKurtosisHSL;
	OFX::Double3DParam* _paramOutputSkewnessHSL;
};

}
}
}

#endif
