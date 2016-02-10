#include "ColorTransformPluginFactory.hpp"
#include "ColorTransformPlugin.hpp"
#include "ColorTransformDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace colorTransform {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorTransformPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleColorTransform", "ColorTransform",
		            "ColorTransform" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	desc.setDescription( "ColorTransform\n"
			 "Plugin is used to apply color transformations \n"
			 "(saturation, constrast, brightness...) on images." );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorTransformPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	
	OFX::DoubleParamDescriptor* paramHueShift = desc.defineDoubleParam( kParamHueShift );
	paramHueShift->setLabel( "Hue Shift" );
	paramHueShift->setDefault( 0.0 );
	paramHueShift->setDisplayRange( 0.0, 1.0 );
	
	OFX::DoubleParamDescriptor* paramSaturation = desc.defineDoubleParam( kParamSaturation );
	paramSaturation->setLabel( "Saturation" );
	paramSaturation->setDefault( 1.0 );
	paramSaturation->setDisplayRange( 0.0, 1.0 );
	
	////
	
	OFX::DoubleParamDescriptor* paramContrastRGB = desc.defineDoubleParam( kParamContrastRGB );
	paramContrastRGB->setLabel( "Global Contrast RGB" );
	paramContrastRGB->setHint( "Global manipulation of RGB contrast." );
	paramContrastRGB->setDefault( 1.0 );
	paramContrastRGB->setDisplayRange( 0.0, 1.0 );
	
	OFX::RGBAParamDescriptor* paramContrast = desc.defineRGBAParam( kParamContrast );
	paramContrast->setLabel( "Contrast" );
	paramContrast->setDefault( 1.0, 1.0, 1.0, 1.0 );
	
	
	OFX::DoubleParamDescriptor* paramBrightnessRGB = desc.defineDoubleParam( kParamBrightnessRGB );
	paramBrightnessRGB->setLabel( "Global Brightness RGB" );
	paramBrightnessRGB->setHint( "Global manipulation of RGB brightness." );
	paramBrightnessRGB->setDefault( 1.0 );
	paramBrightnessRGB->setDisplayRange( 0.0, 1.0 );
	
	OFX::RGBAParamDescriptor* paramBrightness = desc.defineRGBAParam( kParamBrightness );
	paramBrightness->setLabel( "Brightness" );
	paramBrightness->setDefault( 1.0, 1.0, 1.0, 1.0 );
	
	
	OFX::DoubleParamDescriptor* paramOffsetRGB = desc.defineDoubleParam( kParamOffsetRGB );
	paramOffsetRGB->setLabel( "Global Offset RGB" );
	paramOffsetRGB->setHint( "Global manipulation of RGB offset." );
	paramOffsetRGB->setDefault( 0.0 );
	paramOffsetRGB->setDisplayRange( 0.0, 1.0 );
	
	OFX::RGBAParamDescriptor* paramOffset = desc.defineRGBAParam( kParamOffset );
	paramOffset->setLabel( "Offset" );
	paramOffset->setDefault( 0.0, 0.0, 0.0, 0.0 );
	
	
	OFX::DoubleParamDescriptor* paramPivotRGB = desc.defineDoubleParam( kParamPivotRGB );
	paramPivotRGB->setLabel( "Global Pivot RGB" );
	paramPivotRGB->setHint( "Global manipulation of RGB pivot." );
	paramPivotRGB->setDefault( 0.5 );
	paramPivotRGB->setDisplayRange( 0.0, 1.0 );
	
	OFX::RGBAParamDescriptor* paramPivot = desc.defineRGBAParam( kParamPivot );
	paramPivot->setLabel( "Pivot" );
	paramPivot->setDefault( 0.5, 0.5, 0.5, 0.5 );
	
	
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorTransformPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new ColorTransformPlugin( handle );
}

}
}
}

