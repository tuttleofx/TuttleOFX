#include "EXRWriterPluginFactory.hpp"
#include "EXRWriterPlugin.hpp"
#include "EXRWriterDefinitions.hpp"

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

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

	OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginFilename );
	filename->setLabel( kTuttlePluginFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* componentsType = desc.defineChoiceParam( kTuttlePluginComponents );
	componentsType->setLabel( kTuttlePluginComponentsLabel );
	componentsType->appendOption( kTuttlePluginComponentsGray );
	componentsType->appendOption( kTuttlePluginComponentsRGB );
	componentsType->appendOption( kTuttlePluginComponentsRGBA );
	componentsType->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	componentsType->setDefault( eTuttlePluginComponentsRGBA );

	OFX::ChoiceParamDescriptor* storageType = desc.defineChoiceParam( kParamStorageType );
	storageType->setLabel( "Storage type" );
	storageType->appendOption( kParamStorageScanLine );
#ifndef TUTTLE_PRODUCTION
	storageType->appendOption( kParamStorageTiles    );
#endif
	storageType->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	storageType->setDefault( eParamStorageScanLine );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginBitDepth );
	bitDepth->setLabel( kTuttlePluginBitDepthLabel );
	bitDepth->appendOption( kTuttlePluginBitDepth16f );
	bitDepth->appendOption( kTuttlePluginBitDepth32 );
	bitDepth->appendOption( kTuttlePluginBitDepth32f );
	bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitDepth->setDefault( 2 );

	describeWriterParamsInContext( desc, context );
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

