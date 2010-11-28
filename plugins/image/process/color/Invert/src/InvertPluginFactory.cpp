#include "InvertPluginFactory.hpp"
#include "InvertPlugin.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

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
namespace invert {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void InvertPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleInvert", "Invert",
	                "Image inverter" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void InvertPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	/// @todo tuttle: support Alpha images.
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::GroupParamDescriptor* processGroup = desc.defineGroupParam( kParamProcessGroup );
	processGroup->setLabel( "Process" );

	OFX::BooleanParamDescriptor* processR = desc.defineBooleanParam( kParamProcessR );
	processR->setLabel( "R" );
	processR->setDefault( true );
	processR->setParent( processGroup );

	OFX::BooleanParamDescriptor* processG = desc.defineBooleanParam( kParamProcessG );
	processG->setLabel( "G" );
	processG->setDefault( true );
	processG->setParent( processGroup );

	OFX::BooleanParamDescriptor* processB = desc.defineBooleanParam( kParamProcessB );
	processB->setLabel( "B" );
	processB->setDefault( true );
	processB->setParent( processGroup );

	OFX::BooleanParamDescriptor* processA = desc.defineBooleanParam( kParamProcessA );
	processA->setLabel( "A" );
	processA->setDefault( true );
	processA->setParent( processGroup );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* InvertPluginFactory::createInstance( OfxImageEffectHandle handle, OFX::EContext context )
{
	return new InvertPlugin( handle );
}

}
}
}
