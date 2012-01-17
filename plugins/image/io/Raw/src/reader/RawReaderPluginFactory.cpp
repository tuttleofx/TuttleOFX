#include "RawReaderPluginFactory.hpp"
#include "RawReaderDefinitions.hpp"
#include "RawReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void RawReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleRawReader", "RawReader",
                    "Raw file reader" );
    desc.setPluginGrouping( "tuttle/image/io" );

    desc.setDescription( "Raw File reader\n"
                         "Plugin is used to read raw files." );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextReader );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );

    // add supported extensions
    desc.addSupportedExtension( "3fr" );
    desc.addSupportedExtension( "ari" );
    desc.addSupportedExtension( "arw" );
    desc.addSupportedExtension( "srf" );
    desc.addSupportedExtension( "sr2" );
    desc.addSupportedExtension( "bay" );
    desc.addSupportedExtension( "crw" );
    desc.addSupportedExtension( "cr2" );
    desc.addSupportedExtension( "cap" );
    desc.addSupportedExtension( "iiq" );
    desc.addSupportedExtension( "eip" );
    desc.addSupportedExtension( "dcs" );
    desc.addSupportedExtension( "dcr" );
    desc.addSupportedExtension( "drf" );
    desc.addSupportedExtension( "k25" );
    desc.addSupportedExtension( "kdc" );
    desc.addSupportedExtension( "dng" );
    desc.addSupportedExtension( "erf" );
    desc.addSupportedExtension( "fff" );
    desc.addSupportedExtension( "mef" );
    desc.addSupportedExtension( "mos" );
    desc.addSupportedExtension( "mrw" );
    desc.addSupportedExtension( "nef" );
    desc.addSupportedExtension( "nrw" );
    desc.addSupportedExtension( "mos" );
    desc.addSupportedExtension( "orf" );
    desc.addSupportedExtension( "pef" );
    desc.addSupportedExtension( "ptx" );
    desc.addSupportedExtension( "pxn" );
    desc.addSupportedExtension( "r3d" );
    desc.addSupportedExtension( "rad" );
    desc.addSupportedExtension( "raf" );
    desc.addSupportedExtension( "rw2" );
    desc.addSupportedExtension( "raw" );
    desc.addSupportedExtension( "rwl" );
    desc.addSupportedExtension( "dng" );
    desc.addSupportedExtension( "rwz" );
    desc.addSupportedExtension( "obm" );
    desc.addSupportedExtension( "srw" );
    desc.addSupportedExtension( "x3f" );

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
void RawReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext               context )
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    describeReaderParamsInContext( desc, context );

    OFX::ChoiceParamDescriptor* filtering = desc.defineChoiceParam( kParamFiltering );
    filtering->setLabel( "Filtering" );
    filtering->appendOption( kParamFilteringAuto );
    filtering->appendOption( kParamFilteringNone );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context  Application context
 * @return  plugin instance
 */
OFX::ImageEffect* RawReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext        context )
{
    return new RawReaderPlugin( handle );
}

}
}
}
}
