#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_PLUGIN_HPP_

#include "ImageStatisticsDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

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
class ImageStatisticsPlugin : public ImageEffectGilPlugin
{
public:
	ImageStatisticsPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );

	ImageStatisticsProcessParams getProcessParams( const OfxRectI& srcRod ) const;

public:
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
