#include "EXRWriterPluginFactory.hpp"
#include "EXRWriterDefinitions.hpp"
#include "EXRWriterPlugin.hpp"

#include <tuttle/ioplugin/context/WriterPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void EXRWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleExrWriter", "ExrWriter",
			"Exr file writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	desc.setDescription( "EXR File writer\n"
			     "Plugin is used to write exr files." );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// add supported extensions
	desc.addSupportedExtension( "exr" );
	desc.setPluginEvaluation( 90 );

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
void EXRWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	describeWriterParamsInContext( desc, context );
	
	OFX::ChoiceParamDescriptor* storageType = desc.defineChoiceParam( kParamStorageType );
	storageType->setLabel( "Storage type" );
	storageType->appendOption( kParamStorageScanLine );
#if(TUTTLE_EXPERIMENTAL)
	storageType->appendOption( kParamStorageTiles );
#endif
	storageType->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	storageType->setDefault( eParamStorageScanLine );

	OFX::ChoiceParamDescriptor* bitDepth = static_cast<OFX::ChoiceParamDescriptor*>( desc.getParamDescriptor( kTuttlePluginBitDepth ) );
	bitDepth->resetOptions();
	bitDepth->appendOption( kTuttlePluginBitDepth16f );
	bitDepth->appendOption( kTuttlePluginBitDepth32 );
	bitDepth->appendOption( kTuttlePluginBitDepth32f );
	bitDepth->setDefault( eTuttlePluginFileBitDepth32f );

	OFX::ChoiceParamDescriptor* fileBitDepth = desc.defineChoiceParam( kParamFileBitDepth );
	fileBitDepth->setLabel( "File Bit Depth" );
	fileBitDepth->appendOption( kTuttlePluginBitDepth16f );
	fileBitDepth->appendOption( kTuttlePluginBitDepth32 );
	fileBitDepth->appendOption( kTuttlePluginBitDepth32f );
	fileBitDepth->setDefault( eTuttlePluginFileBitDepth32f );
	fileBitDepth->setHint(
		"Read-only information about the image bit depth stored in the file.\n"
		"Data type is per channel in EXR. So we read the type of the first channel.");

	OFX::ChoiceParamDescriptor* compression = desc.defineChoiceParam( kParamCompression );
	compression->setLabel( "Compression" );
	compression->appendOption( kParamCompressionNone, kParamCompressionHintNone );
	compression->appendOption( kParamCompressionRLE, kParamCompressionHintRLE );
	compression->appendOption( kParamCompressionZIPS, kParamCompressionHintZIPS );
	compression->appendOption( kParamCompressionZIP, kParamCompressionHintZIP );
	compression->appendOption( kParamCompressionPIZ, kParamCompressionHintPIZ );
	compression->appendOption( kParamCompressionPXR24, kParamCompressionHintPXR24 );
	compression->appendOption( kParamCompressionB44, kParamCompressionHintB44 );
	compression->appendOption( kParamCompressionB44A, kParamCompressionHintB44A );
	compression->setDefault( eParamCompression_ZIP );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* EXRWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
							  OFX::EContext        context )
{
	return new EXRWriterPlugin( handle );
}

}
}
}
}

