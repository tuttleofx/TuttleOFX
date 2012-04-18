#include "JpegWriterPluginFactory.hpp"
#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void JpegWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleJpegWriter", "JpegWriter",
		    "Jpeg file writer" );
    desc.setPluginGrouping( "tuttle/image/io" );

    std::vector<std::string> extension;
    extension.push_back( "jpeg" );
    extension.push_back( "jpg" );
    extension.push_back( "jpe" );
    extension.push_back( "jfif" );
    extension.push_back( "jfi" );

    std::string listOfExt;
    for( unsigned int i=0; i< extension.size(); i++ )
    {
	listOfExt += extension.at(i);
	listOfExt += ", ";
    }
    listOfExt.erase( listOfExt.size()-2, 2 );

    desc.setDescription( "JPEG File reader\n"
			 "Plugin is used to write jpeg files.\n\n"
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
void JpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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
    desc.addClipPreferencesSlaveParam( *filename );

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginBitDepth );
    bitDepth->setLabel( kTuttlePluginBitDepthLabel );
    bitDepth->appendOption( kTuttlePluginBitDepth8 );
    bitDepth->setDefault( eTuttlePluginBitDepth8 );

    OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam( kParamPremult );
    premult->setLabel( "Premult" );
    premult->setDefault( true );

    OFX::IntParamDescriptor* quality = desc.defineIntParam( kParamQuality );
    quality->setLabel( "Quality" );
    quality->setRange( 0, 100 );
    quality->setDisplayRange( 0, 100 );
    quality->setDefault( 80 );

    describeWriterParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* JpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
							   OFX::EContext        context )
{
    return new JpegWriterPlugin( handle );
}

}
}
}
}
