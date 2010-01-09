/**
 * @file Lin2LogPluginFactory.cpp
 * @brief
 * @author
 * @date    08/01/10 15:19
 *
 */

#include "Lin2LogPluginFactory.hpp"
#include "Lin2LogPlugin.hpp"
#include "Lin2LogDefinitions.hpp"

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
namespace colorspace {
namespace lin2log {

static const bool   kSupportTiles                 = false;
static const bool   kSupportTemporalClipAccess    = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void
Lin2LogPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
    // basic labels
    desc.setLabels( "lin2log", "lin2log",
                    "Linear to logarithmic convertion" );
    desc.setPluginGrouping( "tuttle" );

    // add the supported contexts, only filter at the moment
//    desc.addSupportedContext(eContextGenerator);
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
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void
Lin2LogPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
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

    OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kLin2LogHelpButton );
    helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect*
Lin2LogPluginFactory::createInstance(OfxImageEffectHandle handle,
                                            OFX::ContextEnum context)
{
    return new Lin2LogPlugin(handle);
}

}
}
}
}
