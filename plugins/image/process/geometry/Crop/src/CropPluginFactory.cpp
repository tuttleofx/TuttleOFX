#include <ofxsParam.h>

#include "CropDefinitions.hpp"
#include "CropPlugin.hpp"
#include "CropMargin.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace crop {


mDeclarePluginFactory( CropPluginFactory, {}, {} );

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void CropPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleCrop", "Crop",
	                "Image crop" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<CropMarginOverlay>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void CropPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                           OFX::ContextEnum            context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::BooleanParamDescriptor* bop = desc.defineBooleanParam( kParamFillMode );
	bop->setLabels( kParamFillModeLabel, kParamFillModeLabel, kParamFillModeLabel );
	bop->setScriptName( "BandsOperations" );
	bop->setHint( "Fill bands with black color or repeat last pixel and reset Rod." );
	bop->setDefault( true );

	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamPresets );
	format->setLabels( kParamPresetsLabel, kParamPresetsLabel, kParamPresetsLabel );
	format->setScriptName( "formats" );
	format->appendOption( "1.33 (4/3) bands" );
	format->appendOption( "1.77 (16/9e) bands" );
	format->appendOption( "1.85 bands" );
	format->appendOption( "2.35 (Cinemascope) bands" );
	format->appendOption( "2.40 bands" );
	format->setDefault( 0 );

	OFX::BooleanParamDescriptor* shape = desc.defineBooleanParam( kParamDisplayRect );
	shape->setLabels( kParamDisplayRectLabel, kParamDisplayRectLabel, kParamDisplayRectLabel );
	shape->setDefault( false );

	OFX::BooleanParamDescriptor* anamorphic = desc.defineBooleanParam( kParamAnamorphic );
	anamorphic->setLabels( kParamAnamorphicLabel, kParamAnamorphicLabel, "Anamorphic (stretch)" );
	anamorphic->setDefault( false );
	anamorphic->setIsSecret( true );

	OFX::GroupParamDescriptor* bandsGroup = desc.defineGroupParam( "Bands sizes" );
	OFX::IntParamDescriptor* upBand       = desc.defineIntParam( kParamUp );
	upBand->setLabels( kParamUpLabel, kParamUpLabel, kParamUpLabel );
	upBand->setParent( *bandsGroup );

	OFX::IntParamDescriptor* downBand = desc.defineIntParam( kParamDown );
	downBand->setLabels( kParamDownLabel, kParamDownLabel, kParamDownLabel );
	downBand->setParent( *bandsGroup );

	OFX::IntParamDescriptor* leftBand = desc.defineIntParam( kParamLeft );
	leftBand->setLabels( kParamLeftLabel, kParamLeftLabel, kParamLeftLabel );
	leftBand->setParent( *bandsGroup );

	OFX::IntParamDescriptor* rightBand = desc.defineIntParam( kParamRight );
	rightBand->setLabels( kParamRightLabel, kParamRightLabel, kParamRightLabel );
	rightBand->setParent( *bandsGroup );

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kCropHelpButton );
	helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CropPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                     OFX::ContextEnum     context )
{
	return new CropPlugin( handle );
}

}
}
}

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::crop::CropPluginFactory p( "fr.tuttle.crop", 1, 0 );

	ids.push_back( &p );
}

}
}
