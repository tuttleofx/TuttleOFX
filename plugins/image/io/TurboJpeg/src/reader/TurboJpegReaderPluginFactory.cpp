#include "TurboJpegReaderPluginFactory.hpp"
#include "TurboJpegReaderPlugin.hpp"
#include "TurboJpegReaderDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <tuttle/ioplugin/context/ReaderPluginFactory.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace reader
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void TurboJpegReaderPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleTurboJpegReader", "TurboJpegReader", "Turbo Jpeg file reader");
    desc.setPluginGrouping("tuttle/image/io");

    using namespace boost::assign;
    std::vector<std::string> supportedExtensions;
    supportedExtensions += "jpeg", "jpg", "jpe", "jfif", "jfi";

    desc.setDescription("Optimized JPEG File reader\n"
                        "Plugin is used to read jpeg files.\n\n"
                        "supported extensions: \n" +
                        boost::algorithm::join(supportedExtensions, ", "));

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextReader);
    desc.addSupportedContext(OFX::eContextGenerator);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // add supported extensions
    desc.addSupportedExtensions(supportedExtensions);
    desc.setPluginEvaluation(90);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsMultipleClipDepths(true);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TurboJpegReaderPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    describeReaderParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* optimization = desc.defineChoiceParam(kParamOptimization);
    optimization->setLabel(kParamOptimizationLabel);
    optimization->setHint(kParamOptimizationHint);
    optimization->appendOption(kTurboJpegOptimizationNone);
    optimization->appendOption(kTurboJpegOptimizationMMX);
    optimization->appendOption(kTurboJpegOptimizationSSE);
    optimization->appendOption(kTurboJpegOptimizationSSE2);
    optimization->appendOption(kTurboJpegOptimizationSSE3);
    optimization->setDefault(eTurboJpegOptimizationSSE3);

    OFX::BooleanParamDescriptor* fastupsampling = desc.defineBooleanParam(kParamFastUpsampling);
    fastupsampling->setLabel(kParamFastUpsamplingLabel);
    fastupsampling->setHint(kParamFastUpsamplingHint);
    fastupsampling->setDefault(false);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TurboJpegReaderPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new TurboJpegReaderPlugin(handle);
}
}
}
}
}
