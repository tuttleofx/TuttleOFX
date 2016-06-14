#include "OpenImageIOWriterPluginFactory.hpp"
#include "OpenImageIOWriterPluginFactory.hpp"
#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <tuttle/ioplugin/context/WriterPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace writer
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OpenImageIOWriterPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleOpenImageIOWriter", "OpenImageIOWriter", "OpenImageIO file writer");
    desc.setPluginGrouping("tuttle/image/io");

    using namespace boost::assign;
    std::vector<std::string> supportedExtensions;
    supportedExtensions += "bmp", "cin", "dds", "dpx", "exr", "fits", "hdr", "ico", "j2k", "j2c", "jp2", "jpeg", "jpg",
        "jpe", "jfif", "jfi", "pbm", "pgm", "png", "pnm", "ppm", "pic",
        //"psd",
        "rgbe", "sgi", "tga", "tif", "tiff", "tpic",
        //"tx",
        "webp";

    desc.setDescription("OpenImageIO Writer"
                        "\n\n"
                        "supported extensions: \n" +
                        boost::algorithm::join(supportedExtensions, ", "));

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextWriter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // add supported extensions
    desc.addSupportedExtensions(supportedExtensions);
    desc.setPluginEvaluation(40);

    // plugin flags
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void OpenImageIOWriterPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(kSupportTiles);

    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    // Controls
    describeWriterParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* bitDepth =
        static_cast<OFX::ChoiceParamDescriptor*>(desc.getParamDescriptor(kTuttlePluginBitDepth));
    bitDepth->resetOptions();
    bitDepth->appendOption(kTuttlePluginBitDepthAuto);
    bitDepth->appendOption(kTuttlePluginBitDepth8);
    bitDepth->appendOption(kTuttlePluginBitDepth10);
    bitDepth->appendOption(kTuttlePluginBitDepth12);
    bitDepth->appendOption(kTuttlePluginBitDepth16);
    bitDepth->appendOption(kTuttlePluginBitDepth16f);
    bitDepth->appendOption(kTuttlePluginBitDepth32);
    bitDepth->appendOption(kTuttlePluginBitDepth32f);
    bitDepth->setDefault(eTuttlePluginBitDepthAuto);

    OFX::IntParamDescriptor* quality = desc.defineIntParam(kParamOutputQuality);
    quality->setLabel(kParamOutputQualityLabel);
    quality->setRange(0, 100);
    quality->setDisplayRange(0, 100);
    quality->setDefault(80);

    OFX::ChoiceParamDescriptor* subsampling = desc.defineChoiceParam(kParamOutputSubsampling);
    subsampling->setLabel(kParamOutputSubsamplingLabel);
    subsampling->setHint(kParamOutputSubsamplingHint);
    subsampling->appendOption(kParamOutputSubsampling420);
    subsampling->appendOption(kParamOutputSubsampling422);
    subsampling->appendOption(kParamOutputSubsampling411);
    subsampling->appendOption(kParamOutputSubsampling444);
    subsampling->setDefault(0);

    OFX::ChoiceParamDescriptor* orientation = desc.defineChoiceParam(kParamOutputOrientation);
    orientation->setLabel(kParamOutputOrientationLabel);
    orientation->appendOption(kParamOutputOrientationNormal);
    orientation->appendOption(kParamOutputOrientationFlop);
    orientation->appendOption(kParamOutputOrientationR180);
    orientation->appendOption(kParamOutputOrientationFlip);
    orientation->appendOption(kParamOutputOrientationTransposed);
    orientation->appendOption(kParamOutputOrientationR90Clockwise);
    orientation->appendOption(kParamOutputOrientationTransverse);
    orientation->appendOption(kParamOutputOrientationR90CounterClockwise);
    orientation->setDefault(0);

    OFX::ChoiceParamDescriptor* compression = desc.defineChoiceParam(kParamOutputCompression);
    compression->setLabel(kParamOutputOrientationLabel);
#if(TUTTLE_EXPERIMENTAL)
    compression->appendOption(kParamOutputCompressionNone);
#endif
    compression->appendOption(kParamOutputCompressionZip);
#if(TUTTLE_EXPERIMENTAL)
    compression->appendOption(kParamOutputCompressionZips);
    compression->appendOption(kParamOutputCompressionRle);
    compression->appendOption(kParamOutputCompressionPiz);
    compression->appendOption(kParamOutputCompressionPxr24);
    compression->appendOption(kParamOutputCompressionB44);
    compression->appendOption(kParamOutputCompressionB44a);
    compression->setDefault(eParamCompressionNone);
#else
    compression->setDefault(0);
#endif
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* OpenImageIOWriterPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new OpenImageIOWriterPlugin(handle);
}
}
}
}
}
