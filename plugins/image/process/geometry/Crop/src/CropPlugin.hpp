#ifndef _TUTTLE_PLUGIN_CROP_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CROP_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include "CropDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace crop {

template<class Pixel>
struct CropProcessParams
{
	EParamMode _mode;
	OfxRectI _cropRegion;
//	boost::gil::rgba32f_pixel_t _color;
	Pixel _color;
};

/**
 * @brief
 *
 */
class CropPlugin : public ImageEffectGilPlugin
{
public:
	CropPlugin( OfxImageEffectHandle handle );

public:
	template<class Pixel>
	CropProcessParams<Pixel> getProcessParams( const OfxTime time, const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	OfxRectI getCropRegionValue() const;
	OfxRectI computeCropRegion( const OfxTime time, const bool fromRatio = false ) const;

	void         changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool         getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	
	void render( const OFX::RenderArguments& args );

public:
	OFX::ChoiceParam* _paramMode;
	OFX::RGBAParam* _paramFillColor;
	OFX::ChoiceParam* _paramAxis;
	OFX::ChoiceParam* _paramSymmetric;
	OFX::BooleanParam* _paramFixedRatio;
	OFX::DoubleParam* _paramRatio;
	OFX::ChoiceParam* _paramPreset;
	OFX::BooleanParam* _paramOverlay;
	OFX::GroupParam* _paramCropRegion;
	OFX::IntParam* _paramXMin;
	OFX::IntParam* _paramYMin;
	OFX::IntParam* _paramXMax;
	OFX::IntParam* _paramYMax;
	
private:
	OFX::InstanceChangedArgs _changedArgs;
};

template<class Pixel>
CropProcessParams<Pixel> CropPlugin::getProcessParams( const OfxTime time, const OfxPointD& renderScale ) const
{
	CropProcessParams<Pixel> params;

	params._mode = static_cast<EParamMode>(_paramMode->getValue());

	OfxRGBAColourD color = _paramFillColor->getValue();
	color_convert( ofxToGil( color ), params._color );

	params._cropRegion = computeCropRegion( time );

	return params;
}

}
}
}

#endif
