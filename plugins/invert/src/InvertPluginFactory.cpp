/**
 * @file InvertPluginFactory.cpp
 * @brief   Multithreaded image inverter
 * @author  Eloi Du Bois
 * @date    01/10/09 12:01
 *
 */

#include "InvertPlugin.hpp"
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
namespace invert {

static const bool   kSupportTiles   = true;


mDeclarePluginFactory(InvertPluginFactory, {}, {});

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void
InvertPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    // basic labels
    desc.setLabels("Invert", "Invert",
                   "Image inverter");
    desc.setPluginGrouping("tuttle");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(eContextGeneral);
    desc.addSupportedContext(eContextFilter);

    // add supported pixel depths
    desc.addSupportedBitDepth(eBitDepthUByte);
    desc.addSupportedBitDepth(eBitDepthUShort);
    desc.addSupportedBitDepth(eBitDepthFloat);

    // set a few flags
    desc.setSingleInstance(false);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsTiles(kSupportTiles);
    desc.setTemporalClipAccess(false);
    desc.setRenderTwiceAlways(false);
    desc.setSupportsMultipleClipPARs(false);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void
InvertPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc,
                                          OFX::ContextEnum context)
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

    OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( "Help" );
    helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* InvertPluginFactory::createInstance( OfxImageEffectHandle handle, OFX::ContextEnum context )
{
    return new InvertPlugin( handle );
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
            static tuttle::plugin::invert::InvertPluginFactory p("fr.hd3d.tuttle.invert", 1, 0);
            ids.push_back(&p);
        }
    }
}
