#include "BitDepthPluginFactory.hpp"
#include "BitDepthPlugin.hpp"
#include "BitDepthDefinitions.hpp"

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
namespace bitDepth {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void BitDepthPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
    desc.setLabels( "TuttleBitDepth", "BitDepth",
                    "Bit depth convertor" );
    desc.setPluginGrouping( "tuttle/image/process/color" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // plugin flags
    desc.setSupportsTiles( kSupportTiles );
	desc.setSupportsMultipleClipDepths( true );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void BitDepthPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                                   OFX::EContext context )
{
    OFX::ClipDescriptor *srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
    srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    srcClip->setSupportsTiles( kSupportTiles );

    // Create the mandated output clip
    OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* outBitDepth = desc.defineChoiceParam( kOutputBitDepth );
	assert( outBitDepth );
	outBitDepth->setLabels(kOutputBitDepthLabel, kOutputBitDepthLabel, kOutputBitDepthLabel);
	outBitDepth->appendOption("auto");
	outBitDepth->appendOption("byte (8 bits)");
	outBitDepth->appendOption("short (16 bits)");
	outBitDepth->appendOption("float (32 bits)");
	outBitDepth->setDefault(3);

    OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kBitDepthHelpButton );
    helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* BitDepthPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                             OFX::EContext context )
{
    return new BitDepthPlugin( handle );
}

}
}
}
