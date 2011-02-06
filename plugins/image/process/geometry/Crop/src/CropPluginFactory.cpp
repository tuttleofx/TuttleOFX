#include "CropPluginFactory.hpp"
#include "CropDefinitions.hpp"
#include "CropPlugin.hpp"
#include "CropMargin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsParam.h>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

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

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<CropMarginOverlay>() );
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

	OFX::BooleanParamDescriptor* bop = desc.defineBooleanParam( kParamFillMode );
	bop->setLabel( "Fill bands with black" );
	bop->setScriptName( "BandsOperations" );
	bop->setHint( "Fill bands with black color or repeat last pixel and reset Rod." );
	bop->setDefault( true );

	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamPresets );
	format->setLabel( "Pre-defined formats" );
	format->setScriptName( "formats" );
	format->appendOption( "1.33 (4/3) bands" );
	format->appendOption( "1.77 (16/9e) bands" );
	format->appendOption( "1.85 bands" );
	format->appendOption( "2.35 (Cinemascope) bands" );
	format->appendOption( "2.40 bands" );
	format->setDefault( 0 );

	OFX::BooleanParamDescriptor* overlay = desc.defineBooleanParam( kParamDisplayRect );
	overlay->setLabel( "Display overlay rectangle" );
	overlay->setDefault( false );

	OFX::BooleanParamDescriptor* anamorphic = desc.defineBooleanParam( kParamAnamorphic );
	anamorphic->setLabels( "Anamorphic", "Anamorphic", "Anamorphic (stretch)" );
	anamorphic->setDefault( false );
	anamorphic->setIsSecret( true );

	OFX::GroupParamDescriptor* bandsGroup = desc.defineGroupParam( "Bands sizes" );

	OFX::IntParamDescriptor* upBand       = desc.defineIntParam( kParamUp );
	upBand->setLabel( "Up" );
	upBand->setParent( *bandsGroup );

	OFX::IntParamDescriptor* downBand = desc.defineIntParam( kParamDown );
	downBand->setLabel( "Down" );
	downBand->setParent( *bandsGroup );

	OFX::IntParamDescriptor* leftBand = desc.defineIntParam( kParamLeft );
	leftBand->setLabel( "Left" );
	leftBand->setParent( *bandsGroup );

	OFX::IntParamDescriptor* rightBand = desc.defineIntParam( kParamRight );
	rightBand->setLabel( "Right" );
	rightBand->setParent( *bandsGroup );
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
