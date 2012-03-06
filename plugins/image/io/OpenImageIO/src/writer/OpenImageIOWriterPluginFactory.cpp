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

    std::vector<std::string> extension;
    extension.push_back( "bmp" );
    extension.push_back( "cin" );
    extension.push_back( "dds" );
    extension.push_back( "dpx" );
    extension.push_back( "exr" );
    extension.push_back( "fits" );
    extension.push_back( "hdr" );
    extension.push_back( "ico" );
    extension.push_back( "j2k" );
    extension.push_back( "j2c" );
    extension.push_back( "jp2" );
    extension.push_back( "jpeg" );
    extension.push_back( "jpg" );
    extension.push_back( "jpe" );
    extension.push_back( "jfif" );
    extension.push_back( "jfi" );
    extension.push_back( "pbm" );
    extension.push_back( "pgm" );
    extension.push_back( "png" );
    extension.push_back( "pnm" );
    extension.push_back( "ppm" );
    extension.push_back( "pic" );
    extension.push_back( "rgbe" );
    extension.push_back( "sgi" );
    extension.push_back( "tga" );
    extension.push_back( "tif" );
    extension.push_back( "tiff" );
    extension.push_back( "tpic" );
    extension.push_back( "webp" );

    std::string listOfExt;
    for( unsigned int i=0; i< extension.size(); i++ )
    {
	listOfExt += extension.at(i);
	listOfExt += ", ";
    }
    listOfExt.erase( listOfExt.size()-2, 2 );

    desc.setDescription(
	"OpenImageIO Writer"
	"\n\n"
	"supported extensions: \n" +
	listOfExt
    );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextWriter );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
    for( unsigned int i=0; i< extension.size(); i++ )
    {
	desc.addSupportedExtension( extension.at(i) );
    }

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
    OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginFilename );
    filename->setLabel( kTuttlePluginFilenameLabel );
    filename->setStringType( OFX::eStringTypeFilePath );
    filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    desc.addClipPreferencesSlaveParam( *filename );

    OFX::ChoiceParamDescriptor* components = desc.defineChoiceParam( kTuttlePluginComponents );
    components->setLabel( kTuttlePluginComponentsLabel );
    components->appendOption( kTuttlePluginComponentsGray );
    components->appendOption( kTuttlePluginComponentsRGB );
    components->appendOption( kTuttlePluginComponentsRGBA );
    components->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    components->setDefault( eTuttlePluginComponentsRGBA );

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginBitDepth );
    bitDepth->setLabel( kTuttlePluginBitDepthLabel );
    bitDepth->appendOption( kTuttlePluginBitDepth8 );
    bitDepth->appendOption( kTuttlePluginBitDepth16 );
    bitDepth->appendOption( kTuttlePluginBitDepth16f );
    bitDepth->appendOption( kTuttlePluginBitDepth32 );
    bitDepth->appendOption( kTuttlePluginBitDepth32f );
    bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    bitDepth->setDefault( eTuttlePluginBitDepth16 );

    OFX::ChoiceParamDescriptor* compression = desc.defineChoiceParam( kParamOutputCompression );
    compression->setLabel( "Compression" );
#ifndef TUTTLE_PRODUCTION
    compression->appendOption( kParamOutputCompressionNone );
#endif
    compression->appendOption( kParamOutputCompressionZip );
#ifndef TUTTLE_PRODUCTION
    compression->appendOption( kParamOutputCompressionZips );
    compression->appendOption( kParamOutputCompressionRle );
    compression->appendOption( kParamOutputCompressionPiz );
    compression->appendOption( kParamOutputCompressionPxr24 );
    compression->appendOption( kParamOutputCompressionB44 );
    compression->appendOption( kParamOutputCompressionB44a );
    bitDepth->setDefault( eParamCompressionNone );
#else
    bitDepth->setDefault( 0 );
#endif

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
