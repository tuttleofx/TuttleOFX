/**
 * @file CropPluginFactory.cpp
 * @brief
 * @author
 * @date    01/10/09 14:14
 *
 */

#include <ofxsParam.h>

#include "CropDefinitions.hpp"
#include "CropPlugin.hpp"
#include "CropMargin.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
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

using namespace OFX;

mDeclarePluginFactory( CropPluginFactory, {}, {}
                       );

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void CropPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// basic labels
	desc.setLabels( "Crop", "Crop",
	                "image crop" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( eContextGeneral );
	desc.addSupportedContext( eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( eBitDepthUByte );
	desc.addSupportedBitDepth( eBitDepthUShort );
	desc.addSupportedBitDepth( eBitDepthFloat );

	// set a few flags
	desc.setSingleInstance( false );
	desc.setHostFrameThreading( true );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	desc.setTemporalClipAccess( false );
	desc.setRenderTwiceAlways( false );
	desc.setSupportsMultipleClipPARs( false );

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayDescriptor<CropMarginOverlay, CropMargin >() );
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

	srcClip->addSupportedComponent( ePixelComponentRGBA );
	srcClip->addSupportedComponent( ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( ePixelComponentRGBA );
	dstClip->addSupportedComponent( ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::BooleanParamDescriptor* bop = desc.defineBooleanParam( kParamFillMode );
	bop->setLabels(kParamFillModeLabel, kParamFillModeLabel, kParamFillModeLabel);
	bop->setScriptName( "BandsOperations" );
	bop->setHint("Fill bands with black color or repeat last pixel and reset Rod.");
	bop->setDefault(true);

	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamPresets );
	format->setLabels( kParamPresetsLabel, kParamPresetsLabel, kParamPresetsLabel );
	format->setScriptName( "formats" );
	format->appendOption( "1 : 1,33 (4/3) bands" );
	format->appendOption( "1 : 1,77 (16/9e) bands" );
	format->appendOption( "1 : 1,85 bands" );
	format->appendOption( "1 : 2,35 (Cinemascope) bands" );
	format->appendOption( "1 : 2,40 bands" );
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

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::crop::CropPluginFactory p( "fr.hd3d.tuttle.crop", 1, 0 );

	ids.push_back( &p );
}

}
}
