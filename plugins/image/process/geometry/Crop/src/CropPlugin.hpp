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

	params._cropRegion.x1 = _paramXMin->getValue();
	params._cropRegion.y1 = _paramYMin->getValue();
	params._cropRegion.x2 = _paramXMax->getValue();
	params._cropRegion.y2 = _paramYMax->getValue();

	if( ! _clipSrc->isConnected() )
	{
		return params;
	}
	OfxRectI sRod = _clipSrc->getPixelRod( time );
	OfxPointI sRodSize = _clipSrc->getPixelRodSize( time );
	
	EParamAxis axis = static_cast<EParamAxis>( _paramAxis->getValue() );
	EParamSymmetric symmetric = static_cast<EParamSymmetric>( _paramSymmetric->getValue() );
	switch( axis )
	{
		case eParamAxisXY:
		{

			if( symmetric == eParamSymmetricNone )
					break;

			if( symmetric == eParamSymmetricX ||
			    symmetric == eParamSymmetricXY )
			{
					params._cropRegion.x1 %= int( sRodSize.x * 0.5 );
					params._cropRegion.x2 = sRodSize.x - params._cropRegion.x1;
			}
			if( symmetric == eParamSymmetricY ||
			    symmetric == eParamSymmetricXY )
			{
					params._cropRegion.y1 %= int( sRodSize.y * 0.5 );
					params._cropRegion.y2 = sRodSize.y - params._cropRegion.y1;
			}
			break;
		}
		case eParamAxisX:
		{
			// don't modify Y axis
			params._cropRegion.y1 = sRod.y1;
			params._cropRegion.y2 = sRod.y2;
			if( symmetric == eParamSymmetricX ||
			    symmetric == eParamSymmetricXY )
			{
				params._cropRegion.x1 %= int( sRodSize.x * 0.5 );
				params._cropRegion.x2 = sRodSize.x - params._cropRegion.x1;
			}
			break;
		}
		case eParamAxisY:
		{
			// don't modify X axis
			params._cropRegion.x1 = sRod.x1;
			params._cropRegion.x2 = sRod.x2;
			if( symmetric == eParamSymmetricY ||
			    symmetric == eParamSymmetricXY )
			{
				params._cropRegion.y1 %= int( sRodSize.y * 0.5 );
				params._cropRegion.y2 = sRodSize.y - params._cropRegion.y1;
			}
			break;
		}
	}
	bool fixedRatio = _paramFixedRatio->getValue();
	if( fixedRatio )
	{
		OfxRectI& rec = params._cropRegion;

		double ratio = _paramRatio->getValue();
		if( ratio == 0.0 )
			ratio = 1.0;

		const double xD2 = (rec.x2 - rec.x1) * 0.5;
		const double yD2 = (rec.y2 - rec.y1) * 0.5;
		const double yCenter = rec.y1 + yD2;
		const double nyD2 = xD2 / ratio;
		params._cropRegion.y1 = int( yCenter-nyD2 );
		params._cropRegion.y2 = int( yCenter+nyD2 );
	}

	return params;
}


}
}
}

#endif
