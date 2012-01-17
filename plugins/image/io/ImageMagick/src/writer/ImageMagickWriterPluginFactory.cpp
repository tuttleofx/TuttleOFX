#include "ImageMagickWriterPluginFactory.hpp"
#include "ImageMagickWriterDefinitions.hpp"
#include "ImageMagickWriterPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageMagickWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleImageMagickWriter", "ImageMagickWriter",
                    "ImageMagick file writer" );
    desc.setPluginGrouping( "tuttle/image/io" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextWriter );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
    desc.addSupportedExtension( "aai" );
    desc.addSupportedExtension( "art" );
    desc.addSupportedExtension( "arw" );
    desc.addSupportedExtension( "avi" );
    desc.addSupportedExtension( "avs" );
    desc.addSupportedExtension( "bmp" );
    desc.addSupportedExtension( "bmp2" );
    desc.addSupportedExtension( "bmp3" );
    desc.addSupportedExtension( "cals" );
    desc.addSupportedExtension( "cgm" );
    desc.addSupportedExtension( "cin" );
    desc.addSupportedExtension( "cmyk" );
    desc.addSupportedExtension( "cmyka" );
    desc.addSupportedExtension( "cr2" );
    desc.addSupportedExtension( "crw" );
    desc.addSupportedExtension( "cur" );
    desc.addSupportedExtension( "cut" );
    desc.addSupportedExtension( "dcm" );
    desc.addSupportedExtension( "dcr" );
    desc.addSupportedExtension( "dcx" );
    desc.addSupportedExtension( "dib" );
    desc.addSupportedExtension( "djvu" );
    desc.addSupportedExtension( "dng" );
    desc.addSupportedExtension( "dot" );
    desc.addSupportedExtension( "dpx" );
    desc.addSupportedExtension( "emf" );
    desc.addSupportedExtension( "epdf" );
    desc.addSupportedExtension( "epi" );
    desc.addSupportedExtension( "eps" );
    desc.addSupportedExtension( "eps2" );
    desc.addSupportedExtension( "eps3" );
    desc.addSupportedExtension( "epsf" );
    desc.addSupportedExtension( "epsi" );
    desc.addSupportedExtension( "ept" );
    desc.addSupportedExtension( "exr" );
    desc.addSupportedExtension( "fax" );
    desc.addSupportedExtension( "fig" );
    desc.addSupportedExtension( "fits" );
    desc.addSupportedExtension( "fpx" );
    desc.addSupportedExtension( "gif" );
    desc.addSupportedExtension( "gplt" );
    desc.addSupportedExtension( "gray" );
    desc.addSupportedExtension( "hdr" );
    desc.addSupportedExtension( "hpgl" );
    desc.addSupportedExtension( "hrz" );
    desc.addSupportedExtension( "html" );
    desc.addSupportedExtension( "ico" );
    desc.addSupportedExtension( "info" );
    desc.addSupportedExtension( "inline" );
    desc.addSupportedExtension( "jbig" );
    desc.addSupportedExtension( "jng" );
    desc.addSupportedExtension( "jp2" );
    desc.addSupportedExtension( "jpc" );
    desc.addSupportedExtension( "jpg" );
    desc.addSupportedExtension( "man" );
    desc.addSupportedExtension( "mat" );
    desc.addSupportedExtension( "miff" );
    desc.addSupportedExtension( "mono" );
    desc.addSupportedExtension( "mng" );
    desc.addSupportedExtension( "m2v" );
    desc.addSupportedExtension( "mpeg" );
    desc.addSupportedExtension( "mpc" );
    desc.addSupportedExtension( "mpr" );
    desc.addSupportedExtension( "mrw" );
    desc.addSupportedExtension( "msl" );
    desc.addSupportedExtension( "mtv" );
    desc.addSupportedExtension( "mvg" );
    desc.addSupportedExtension( "nef" );
    desc.addSupportedExtension( "orf" );
    desc.addSupportedExtension( "otb" );
    desc.addSupportedExtension( "p7" );
    desc.addSupportedExtension( "palm" );
    desc.addSupportedExtension( "pam" );
    desc.addSupportedExtension( "pbm" );
    desc.addSupportedExtension( "pcd" );
    desc.addSupportedExtension( "pcds" );
    desc.addSupportedExtension( "pcl" );
    desc.addSupportedExtension( "pcx" );
    desc.addSupportedExtension( "pdb" );
    desc.addSupportedExtension( "pdf" );
    desc.addSupportedExtension( "pef" );
    desc.addSupportedExtension( "pfa" );
    desc.addSupportedExtension( "pfb" );
    desc.addSupportedExtension( "pfm" );
    desc.addSupportedExtension( "pgm" );
    desc.addSupportedExtension( "picon" );
    desc.addSupportedExtension( "pict" );
    desc.addSupportedExtension( "pix" );
    desc.addSupportedExtension( "png" );
    desc.addSupportedExtension( "png8" );
    desc.addSupportedExtension( "png16" );
    desc.addSupportedExtension( "png32" );
    desc.addSupportedExtension( "pnm" );
    desc.addSupportedExtension( "ppm" );
    desc.addSupportedExtension( "ps" );
    desc.addSupportedExtension( "ps2" );
    desc.addSupportedExtension( "ps3" );
    desc.addSupportedExtension( "psb" );
    desc.addSupportedExtension( "psd" );
    desc.addSupportedExtension( "ptif" );
    desc.addSupportedExtension( "pwp" );
    desc.addSupportedExtension( "rad" );
    desc.addSupportedExtension( "rgb" );
    desc.addSupportedExtension( "rgba" );
    desc.addSupportedExtension( "rla" );
    desc.addSupportedExtension( "rle" );
    desc.addSupportedExtension( "sct" );
    desc.addSupportedExtension( "sfw" );
    desc.addSupportedExtension( "sgi" );
    desc.addSupportedExtension( "shtml" );
    desc.addSupportedExtension( "sid" );
    desc.addSupportedExtension( "mrsid" );
    desc.addSupportedExtension( "sun" );
    desc.addSupportedExtension( "svg" );
    desc.addSupportedExtension( "tga" );
    desc.addSupportedExtension( "tiff" );
    desc.addSupportedExtension( "tim" );
    desc.addSupportedExtension( "tif" );
    desc.addSupportedExtension( "txt" );
    desc.addSupportedExtension( "uil" );
    desc.addSupportedExtension( "uyvy" );
    desc.addSupportedExtension( "vicar" );
    desc.addSupportedExtension( "viff" );
    desc.addSupportedExtension( "wbmp" );
    desc.addSupportedExtension( "webp" );
    desc.addSupportedExtension( "wmf" );
    desc.addSupportedExtension( "wpg" );
    desc.addSupportedExtension( "x" );
    desc.addSupportedExtension( "xbm" );
    desc.addSupportedExtension( "xcf" );
    desc.addSupportedExtension( "xpm" );
    desc.addSupportedExtension( "xwd" );
    desc.addSupportedExtension( "x3f" );
    desc.addSupportedExtension( "ycbcr" );
    desc.addSupportedExtension( "ycbcra" );
    desc.addSupportedExtension( "yuv" );

    // plugin flags
    desc.setRenderThreadSafety( OFX::eRenderFullySafe );
    desc.setHostFrameThreading( false );
    desc.setSupportsMultiResolution( false );
    desc.setSupportsMultipleClipDepths( true );
    desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ImageMagickWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                        OFX::EContext               context )
{
    OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

    srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
    srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    srcClip->setSupportsTiles( kSupportTiles );

    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    // Controls
    OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamWriterFilename );
    filename->setLabel( "Filename" );
    filename->setStringType( OFX::eStringTypeFilePath );
    filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    desc.addClipPreferencesSlaveParam( *filename );

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kParamWriterBitDepth );
    bitDepth->setLabel( "Bit depth" );
    bitDepth->appendOption( kTuttlePluginBitDepth8 );
    bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    bitDepth->setDefault( 0 );

    OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam( kParamPremult );
    premult->setLabel( "Premult" );
    premult->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    premult->setDefault( true );

    OFX::IntParamDescriptor* quality = desc.defineIntParam( kParamQuality );
    quality->setLabel( "Quality" );
    quality->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    quality->setDefault( 80 );

    OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam( kParamWriterRender );
    render->setLabels( "Render", "Render", "Render step" );
    render->setHint( "Force render (writing)" );

    OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam( kParamWriterRenderAlways );
    renderAlways->setLabel( "Render always" );
    renderAlways->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    renderAlways->setDefault( false );

    OFX::IntParamDescriptor* forceNewRender = desc.defineIntParam( kParamWriterForceNewRender );
    forceNewRender->setLabel( "Force new render" );
    forceNewRender->setIsSecret( true );
    forceNewRender->setIsPersistant( false );
    forceNewRender->setAnimates( false );
    forceNewRender->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    forceNewRender->setEvaluateOnChange( true );
    forceNewRender->setDefault( 0 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ImageMagickWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
    return new ImageMagickWriterPlugin( handle );
}

}
}
}
}
