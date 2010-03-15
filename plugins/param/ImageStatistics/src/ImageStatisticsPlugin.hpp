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
	OfxRectD _rect; ///< the selected rectangle, clipped to the image rod
	EChooseOutput _chooseOutput;
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
//	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void getRegionsOfInterest( const OFX::RegionsOfInterestArguments &args, OFX::RegionOfInterestSetter &rois );

	ImageStatisticsProcessParams getProcessParams( const OfxRectD& srcRod ) const;
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _srcClip; ///< Source image clip
    OFX::Clip* _dstClip; ///< Destination image clip
	
	OFX::Double2DParam* _cornerA;
	OFX::Double2DParam* _cornerB;
	OFX::ChoiceParam* _chooseOutput;
	OFX::RGBAParam* _outputAverage;
	OFX::RGBAParam* _outputChannelMin;
	OFX::RGBAParam* _outputChannelMax;
	OFX::RGBAParam* _outputLuminosityMin;
	OFX::RGBAParam* _outputLuminosityMax;
};

}
}
}

#endif
