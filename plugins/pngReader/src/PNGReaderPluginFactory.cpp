/**
 * @author Eloi Du Bois.
 * @date   2009/10/11
 *
 */

#include "PNGReaderPlugin.hpp"
#include "tuttle/plugin/ImageGilProcessor.hpp"
#include "tuttle/plugin/Progress.hpp"
#include "tuttle/plugin/PluginException.hpp"

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


mDeclarePluginFactory(PNGReaderPluginFactory, {}, {});

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void
PNGReaderPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    // basic labels
    desc.setLabels("PNGReaderHd3d", "PNGReaderHd3d",
                   "PNG File reader Hd3d");
    desc.setPluginGrouping("hd3d");

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
PNGReaderPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc,
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
PNGReaderPluginFactory::createInstance(OfxImageEffectHandle handle,
                                            OFX::ContextEnum context)
{
    return new PNGReaderPlugin(handle);
}

}

namespace OFX 
{
    namespace Plugin 
    {
        void getPluginIDs(OFX::PluginFactoryArray &ids)
        {
            static tuttle::PNGReaderPluginFactory p("fr.hd3d.tuttle.pngReader", 1, 0);
            ids.push_back(&p);
        }
    }
}
