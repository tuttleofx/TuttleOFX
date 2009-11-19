/**
 * @author Eloi Du Bois.
 * @date   2009/10/11
 *
 */

#include "DPXReaderPlugin.hpp"
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

static const bool   kSupportTiles                 = false;

using namespace OFX;

mDeclarePluginFactory(DPXReaderPluginFactory, {}, {});

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void
DPXReaderPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    // basic labels
    desc.setLabels("DPXReaderHd3d", "DPXReaderHd3d",
                   "DPX File reader Hd3d");
    desc.setPluginGrouping("OFX");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(eContextGenerator);

    // add supported pixel depths
    desc.addSupportedBitDepth(eBitDepthFloat);
    desc.addSupportedBitDepth(eBitDepthUByte);
    desc.addSupportedBitDepth(eBitDepthUShort);

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
DPXReaderPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc,
                                          OFX::ContextEnum context)
{
    // Create the mandated output clip
    ClipDescriptor *dstClip = desc.defineClip("Output");
    dstClip->addSupportedComponent(ePixelComponentRGBA);
    dstClip->addSupportedComponent(ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    // Controls
    StringParamDescriptor *filename = desc.defineStringParam("Input filename");
    filename->setScriptName("filename");
    filename->setStringType(eStringTypeFilePath);
    filename->setCacheInvalidation(eCacheInvalidateValueAll);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect*
DPXReaderPluginFactory::createInstance(OfxImageEffectHandle handle,
                                            OFX::ContextEnum context)
{
    return new DPXReaderPlugin(handle);
}

}

namespace OFX
{
    namespace Plugin 
    {
        void getPluginIDs(OFX::PluginFactoryArray &ids)
        {
            static OFX::DPXReaderPluginFactory p("fr.hd3d.tuttle.dpxreader", 1, 0);
            ids.push_back(&p);
        }
    }
}
