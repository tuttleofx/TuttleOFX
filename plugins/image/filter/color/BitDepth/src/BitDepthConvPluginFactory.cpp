/**
 * @file BitDepthConvPluginFactory.cpp
 * @brief
 * @author
 * @date    08/01/10 17:46
 *
 */
 
#include "BitDepthConvPluginFactory.hpp"
#include "BitDepthConvPlugin.hpp"
#include "BitDepthConvDefinitions.hpp"

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
namespace bitDepthConvert {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void
BitDepthConvPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
    // basic labels
    desc.setLabels( "bitDepthConvert", "bitDepthConvert",
                    "Bit depth convertor" );
    desc.setPluginGrouping( "tuttle" );

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext( eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( eBitDepthUByte );
    desc.addSupportedBitDepth( eBitDepthUShort );
    desc.addSupportedBitDepth( eBitDepthFloat );

    // set a few flags
    desc.setSingleInstance( false );
	desc.setHostFrameThreading( true );
    desc.setSupportsMultiResolution( false );
    desc.setSupportsTiles( kSupportTiles );
    desc.setTemporalClipAccess( kSupportTemporalClipAccess );
    desc.setRenderTwiceAlways( false );
    desc.setSupportsMultipleClipPARs( false );
	desc.setSupportsMultipleClipDepths( true );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void
BitDepthConvPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                             OFX::ContextEnum context )
{
    OFX::ClipDescriptor *srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
    srcClip->addSupportedComponent( ePixelComponentRGBA );
    srcClip->addSupportedComponent( ePixelComponentAlpha );
    srcClip->setSupportsTiles( kSupportTiles );

    // Create the mandated output clip
    OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( ePixelComponentRGBA );
    dstClip->addSupportedComponent( ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* outBitDepth = desc.defineChoiceParam( kOutputBitDepth );
	assert( outBitDepth );
	outBitDepth->setLabels(kOutputBitDepthLabel, kOutputBitDepthLabel, kOutputBitDepthLabel);
	outBitDepth->appendOption("byte (8 bits)");
	outBitDepth->appendOption("short (16 bits)");
	outBitDepth->appendOption("float (32 bits)");
	outBitDepth->setDefault(3);

    OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kBitDepthConvHelpButton );
    helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect*
BitDepthConvPluginFactory::createInstance(OfxImageEffectHandle handle,
                                            OFX::ContextEnum context)
{
    return new BitDepthConvPlugin(handle);
}

}
}
}

namespace OFX 
{
    namespace Plugin 
    {
        void getPluginIDs(OFX::PluginFactoryArray &ids)
        {
            static tuttle::plugin::bitDepthConvert::BitDepthConvPluginFactory p("fr.hd3d.tuttle.bitdepthconv", 1, 0);
            ids.push_back(&p);
        }
    }
}
