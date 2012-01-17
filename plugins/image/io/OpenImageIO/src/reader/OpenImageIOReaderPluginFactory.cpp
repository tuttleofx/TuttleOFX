#include "OpenImageIOReaderPluginFactory.hpp"
#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OpenImageIOReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleOpenImageIOReader", "OpenImageIOReader",
                    "OpenImageIO file reader" );
    desc.setPluginGrouping( "tuttle/image/io" );

    desc.setDescription(
        "OpenImageIO Reader"
        "\n\n"
        "supported formats:\n"
        "TIFF\nJPEG/JFIF\nOpenEXR\nPNG\nHDR/RGBE\nTarga\nJPEG-2000\nDPX\nCineon\nFITS\nBMP\nICO\nRMan Zfile\nSoftimagePIC\nDDS\nSGI\nPNM\nPPM\nPGM\nPBM\nField3d\nWebP"
        "\n" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextReader );
    desc.addSupportedContext( OFX::eContextGenerator );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );

    // add supported extensions
        // tiff
    desc.addSupportedExtension( "tif" );
    desc.addSupportedExtension( "tiff" );
        // jpeg
    desc.addSupportedExtension( "jpeg" );
    desc.addSupportedExtension( "jpg" );
    desc.addSupportedExtension( "jpe" );
    desc.addSupportedExtension( "jfif" );
    desc.addSupportedExtension( "jfi" );
        // exr
    desc.addSupportedExtension( "exr" );
        // hrd
    desc.addSupportedExtension( "hdr" );
    desc.addSupportedExtension( "rgbe" );
        // png
    desc.addSupportedExtension( "png" );
        // targa
    desc.addSupportedExtension( "tga" );
    desc.addSupportedExtension( "tpic" );
        // jpeg-2000
    desc.addSupportedExtension( "j2k" );
    desc.addSupportedExtension( "jp2" );
    desc.addSupportedExtension( "j2c" );
        // dpx
    desc.addSupportedExtension( "dpx" );
        // cineon
    desc.addSupportedExtension( "cin" );
        // fits
    desc.addSupportedExtension( "fits" );
        // bmp
    desc.addSupportedExtension( "bmp" );
        // ico
    desc.addSupportedExtension( "ico" );
        // SoftimagePIC
    desc.addSupportedExtension( "pic" );
        // DDS
    desc.addSupportedExtension( "dds" );
        // sgi
    desc.addSupportedExtension( "sgi" );
        // pnm
    desc.addSupportedExtension( "pnm" );
        // ppm
    desc.addSupportedExtension( "ppm" );
        // pgm
    desc.addSupportedExtension( "pgm" );
        // pbm
    desc.addSupportedExtension( "pbm" );
        // webP
    desc.addSupportedExtension( "webp" );

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
void OpenImageIOReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                        OFX::EContext               context )
{
    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    describeReaderParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* OpenImageIOReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
    return new OpenImageIOReaderPlugin( handle );
}

}
}
}
}
