#include "DPXWriterPluginFactory.hpp"
#include "DPXWriterPlugin.hpp"
#include "DPXWriterDefinitions.hpp"

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void DPXWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleDpxWriter", "DpxWriter",
		    "Dpx file writer" );
    desc.setPluginGrouping( "tuttle/image/io" );

    desc.setDescription( "Digital Picture Exchange (DPX), ANSI/SMPTE standard (268M-2003)" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextWriter );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
    desc.addSupportedExtension( "dpx" );

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
void DPXWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						OFX::EContext               context )
{
    OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
    srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
    srcClip->setSupportsTiles( kSupportTiles );

    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->setSupportsTiles( kSupportTiles );

    // Controls
    OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginFilename );
    filename->setLabel( kTuttlePluginFilenameLabel );
    filename->setStringType( OFX::eStringTypeFilePath );
    filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    desc.addClipPreferencesSlaveParam( *filename );

    OFX::ChoiceParamDescriptor* componentsType = desc.defineChoiceParam( kTuttlePluginComponents );
    componentsType->setLabel( kTuttlePluginComponentsLabel );
    componentsType->appendOption( kTuttlePluginComponentsRGB );
    componentsType->appendOption( kTuttlePluginComponentsRGBA );
    componentsType->appendOption( kTuttlePluginComponentsABGR );
    componentsType->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    componentsType->setDefault( 0 );

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginBitDepth );
    bitDepth->setLabel( kTuttlePluginBitDepthLabel );
    bitDepth->appendOption( kTuttlePluginBitDepth8 );
    bitDepth->appendOption( kTuttlePluginBitDepth10 );
    bitDepth->appendOption( kTuttlePluginBitDepth12 );
    bitDepth->appendOption( kTuttlePluginBitDepth16 );
    bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    bitDepth->setDefault( eTuttlePluginBitDepth10 );


    OFX::BooleanParamDescriptor* compressed = desc.defineBooleanParam( kParamPacket );
    compressed->setLabel( kParamPacketLabel );
    compressed->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    compressed->setDefault( true );

    describeWriterParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DPXWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
							  OFX::EContext        context )
{
    return new DPXWriterPlugin( handle );
}

}
}
}
}
