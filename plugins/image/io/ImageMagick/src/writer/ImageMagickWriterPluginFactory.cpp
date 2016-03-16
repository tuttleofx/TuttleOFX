#include "ImageMagickWriterPluginFactory.hpp"
#include "ImageMagickWriterDefinitions.hpp"
#include "ImageMagickWriterPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace writer
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageMagickWriterPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleImageMagickWriter", "ImageMagickWriter", "ImageMagick file writer");
    desc.setPluginGrouping("tuttle/image/io");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextWriter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // add supported extensions
    using namespace boost::assign;
    std::vector<std::string> supportedExtensions;
    supportedExtensions += "aai", "art", "arw", "avi", "avs", "bmp", "bmp2", "bmp3", "cals", "cgm", "cin", "cmyk", "cmyka",
        "cr2", "crw", "cur", "cut", "dcm", "dcr", "dcx", "dib", "djvu", "dng", "dot", "dpx", "emf", "epdf", "epi", "eps",
        "eps2", "eps3", "epsf", "epsi", "ept", "exr", "fax", "fig", "fits", "fpx", "gif", "gplt", "gray", "hdr", "hpgl",
        "hrz", "html", "ico", "info", "inline", "jbig", "jng", "jp2", "jpc", "jpg", "jpeg", "man", "mat", "miff", "mono",
        "mng", "m2v", "mpeg", "mpc", "mpr", "mrw", "msl", "mtv", "mvg", "nef", "orf", "otb", "p7", "palm", "pam", "pbm",
        "pcd", "pcds", "pcl", "pcx", "pdb", "pdf", "pef", "pfa", "pfb", "pfm", "pgm", "picon", "pict", "pix", "png", "png8",
        "png16", "png32", "pnm", "ppm", "ps", "ps2", "ps3", "psb", "psd", "ptif", "pwp", "rad", "rgb", "rgba", "rla", "rle",
        "sct", "sfw", "sgi", "shtml", "sid", "mrsid", "sun", "svg", "tga", "tiff", "tim", "tif", "txt", "uil", "uyvy",
        "vicar", "viff", "wbmp", "webp", "wmf", "wpg", "x", "xbm", "xcf", "xpm", "xwd", "x3f", "ycbcr", "ycbcra", "yuv";

    desc.addSupportedExtensions(supportedExtensions);
    desc.setPluginEvaluation(0);

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
void ImageMagickWriterPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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
    OFX::StringParamDescriptor* filename = desc.defineStringParam(kTuttlePluginFilename);
    filename->setLabel(kTuttlePluginFilenameLabel);
    filename->setStringType(OFX::eStringTypeFilePath);
    filename->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    desc.addClipPreferencesSlaveParam(*filename);

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam(kTuttlePluginBitDepth);
    bitDepth->setLabel(kTuttlePluginBitDepthLabel);
    bitDepth->appendOption(kTuttlePluginBitDepth8);
    bitDepth->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    bitDepth->setDefault(eTuttlePluginBitDepth8);

    OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam(kParamPremult);
    premult->setLabel("Premult");
    premult->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    premult->setDefault(true);

    OFX::IntParamDescriptor* quality = desc.defineIntParam(kParamQuality);
    quality->setLabel("Quality");
    quality->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    quality->setDefault(80);

    OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam(kParamWriterRender);
    render->setLabels("Render", "Render", "Render step");
    render->setHint("Force render (writing)");

    OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam(kParamWriterRenderAlways);
    renderAlways->setLabel("Render always");
    renderAlways->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    renderAlways->setDefault(false);

    OFX::IntParamDescriptor* forceNewRender = desc.defineIntParam(kParamWriterForceNewRender);
    forceNewRender->setLabel("Force new render");
    forceNewRender->setIsSecret(true);
    forceNewRender->setIsPersistant(false);
    forceNewRender->setAnimates(false);
    forceNewRender->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    forceNewRender->setEvaluateOnChange(true);
    forceNewRender->setDefault(0);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ImageMagickWriterPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ImageMagickWriterPlugin(handle);
}
}
}
}
}
