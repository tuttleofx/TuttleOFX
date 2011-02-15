#include "CropPluginFactory.hpp"
#include "CropDefinitions.hpp"
#include "CropPlugin.hpp"
#include "CropMargin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsParam.h>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <limits>

namespace tuttle {
namespace plugin {
namespace crop {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void CropPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleCrop", "Crop",
	                "Image crop" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

	desc.setDescription( "Crop\n"
	                     "Plugin is used to crop an image." );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<CropEffectOverlay>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void CropPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                           OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );
	
	OFX::ChoiceParamDescriptor* mode = desc.defineChoiceParam( kParamMode );
	mode->setLabel( "Mode" );
	mode->appendOption( kParamModeCrop );
	mode->appendOption( kParamModeFillColor );
//	mode->appendOption( kParamModeResize ); // good idea or not?
	mode->setDefault( eParamModeFillColor );

	OFX::RGBAParamDescriptor* fillColor = desc.defineRGBAParam( kParamFillColor );
	fillColor->setLabel( "Color" );
	fillColor->setHint( "Color to fill bands" );
	fillColor->setDefault( 0.0, 0.0, 0.0, 1.0 );

	OFX::ChoiceParamDescriptor* axis = desc.defineChoiceParam( kParamAxis );
	axis->setLabel( "Axis" );
	axis->appendOption( kParamAxisXY );
	axis->appendOption( kParamAxisX );
	axis->appendOption( kParamAxisY );
	axis->setDefault( eParamAxisY );
	axis->setEvaluateOnChange( false );

	OFX::ChoiceParamDescriptor* symmetric = desc.defineChoiceParam( kParamSymmetric );
	symmetric->setLabel( "Symmetric" );
	symmetric->appendOption( kParamSymmetricNone );
	symmetric->appendOption( kParamSymmetricXY );
	symmetric->appendOption( kParamSymmetricX );
	symmetric->appendOption( kParamSymmetricY );
	symmetric->setHint( "Is the crop region symmetric around image center?" );
	symmetric->setDefault( true );
	symmetric->setEvaluateOnChange( false );

	OFX::BooleanParamDescriptor* fixedRatio = desc.defineBooleanParam( kParamFixedRatio );
	fixedRatio->setLabel( "Fixed ratio" );
	fixedRatio->setHint( "Constrain the cropped region to this ratio." );
	fixedRatio->setDefault( true );
	fixedRatio->setEvaluateOnChange( false );

	OFX::ChoiceParamDescriptor* preset = desc.defineChoiceParam( kParamPreset );
	preset->setLabel( "Preset" );
	preset->appendOption( kParamPreset_custom );
	preset->appendOption( kParamPreset_1_33 );
	preset->appendOption( kParamPreset_1_77 );
	preset->appendOption( kParamPreset_1_85 );
	preset->appendOption( kParamPreset_2_35 );
	preset->appendOption( kParamPreset_2_40 );
	preset->setDefault( 0 );
	preset->setEvaluateOnChange( false );

	OFX::DoubleParamDescriptor* ratio = desc.defineDoubleParam( kParamRatio );
	ratio->setLabel( "Ratio" );
	ratio->setRange( 0, std::numeric_limits<double>::max() );
	ratio->setDisplayRange( 0, 3 );
	ratio->setDefault( 2.0 );
	ratio->setHint( "Ratio X/Y of the cropped region." );

	OFX::BooleanParamDescriptor* overlay = desc.defineBooleanParam( kParamOverlay );
	overlay->setLabel( "Overlay" );
	overlay->setHint( "Display overlay rectangle" );
	overlay->setDefault( false );
	overlay->setEvaluateOnChange( false );

	OFX::GroupParamDescriptor* cropRegion = desc.defineGroupParam( kParamGroupCropRegion );

	OFX::IntParamDescriptor* xMin       = desc.defineIntParam( kParamXMin );
	xMin->setLabel( "X min" );
//	xMin->setRange( 0, std::numeric_limits<int>::max() );
	xMin->setDisplayRange( 0, 3000 );
	xMin->setDefault( 0 );
	xMin->setParent( *cropRegion );

	OFX::IntParamDescriptor* yMin = desc.defineIntParam( kParamYMin );
	yMin->setLabel( "Y min" );
//	yMin->setRange( 0, std::numeric_limits<int>::max() );
	yMin->setDisplayRange( 0, 3000 );
	yMin->setDefault( 0 );
	yMin->setParent( *cropRegion );

	OFX::IntParamDescriptor* xMax = desc.defineIntParam( kParamXMax );
	xMax->setLabel( "X max" );
//	xMax->setRange( 0, std::numeric_limits<int>::max() );
	xMax->setDisplayRange( 0, 3000 );
	xMax->setDefault( 0 );
	xMax->setParent( *cropRegion );

	OFX::IntParamDescriptor* yMax = desc.defineIntParam( kParamYMax );
	yMax->setLabel( "Y max" );
//	yMax->setRange( 0, std::numeric_limits<int>::max() );
	yMax->setDisplayRange( 0, 3000 );
	yMax->setDefault( 0 );
	yMax->setParent( *cropRegion );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CropPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                     OFX::EContext        context )
{
	return new CropPlugin( handle );
}

}
}
}
