#include "OpenImageIOWriterPluginFactory.hpp"
#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OpenImageIOWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleOpenImageIOWriter", "OpenImageIOWriter",
		    "OpenImageIO file writer" );
    desc.setPluginGrouping( "tuttle/image/io" );

    desc.setDescription(
	"OpenImageIO Writer"
	"\n\n"
	"supported formats:\n"
	"TIFF\nJPEG/JFIF\nOpenEXR\nPNG\nHDR/RGBE\nTarga\nJPEG-2000\nDPX\nCineon\nFITS\nBMP\nICO\nRMan Zfile\nSoftimagePIC\nDDS\nSGI\nPNM\nPPM\nPGM\nPBM\nField3d\nWebP"
	"\n" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextWriter );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

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
    desc.setHostFrameThreading         ( false );
    desc.setSupportsMultiResolution    ( false );
    desc.setSupportsMultipleClipDepths ( true );
    desc.setSupportsTiles              ( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void OpenImageIOWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

    OFX::ChoiceParamDescriptor* components = desc.defineChoiceParam( kParamOutputComponents );
    components->setLabel( "Components" );
    components->appendOption( kParamOutputComponentsRGBA );
    components->appendOption( kParamOutputComponentsRGB );
    components->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    components->setDefault( 0 );

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kParamWriterBitDepth );
    bitDepth->setLabel( "Bit depth" );
    bitDepth->appendOption( kTuttlePluginBitDepth8 );
    bitDepth->appendOption( kTuttlePluginBitDepth16 );
    bitDepth->appendOption( kTuttlePluginBitDepth32f );
    bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    bitDepth->setDefault( 1 );

    OFX::ChoiceParamDescriptor* compression = desc.defineChoiceParam( kParamOutputCompression );
    compression->setLabel( "Compression" );
    compression->appendOption( kParamOutputCompressionNone );
    compression->appendOption( kParamOutputCompressionZip );
    compression->appendOption( kParamOutputCompressionZips );
    compression->appendOption( kParamOutputCompressionRle );
    compression->appendOption( kParamOutputCompressionPiz );
    compression->appendOption( kParamOutputCompressionPxr24 );
    compression->appendOption( kParamOutputCompressionB44 );
    compression->appendOption( kParamOutputCompressionB44a );
    bitDepth->setDefault( eParamCompressionNone );

    describeWriterParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* OpenImageIOWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
								  OFX::EContext        context )
{
    return new OpenImageIOWriterPlugin( handle );
}

}
}
}
}
