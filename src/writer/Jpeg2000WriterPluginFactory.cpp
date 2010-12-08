#include "Jpeg2000WriterPluginFactory.hpp"
#include "Jpeg2000WriterPlugin.hpp"
#include "Jpeg2000WriterDefinitions.hpp"

#include <openjpeg/J2KWriter.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void Jpeg2000WriterPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
	desc.setLabels( "privateDuJpeg2000Writer", "Jpeg2000Writer",
		            "Jpeg 2000 image writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void Jpeg2000WriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                                     OFX::EContext context )
{
	OFX::ClipDescriptor *srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kWriterParamFilename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kWriterParamBitDepth );
	bitDepth->setLabel( "Precision" );
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->appendOption( kTuttlePluginBitDepth12 );
	bitDepth->appendOption( kTuttlePluginBitDepth16 );
	bitDepth->appendOption( kTuttlePluginBitDepth32 );
	bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitDepth->setDefault( 1 );
	desc.addClipPreferencesSlaveParam( *bitDepth );

	OFX::BooleanParamDescriptor* lossless = desc.defineBooleanParam( kParamLossless );
	lossless->setLabel( "lossless" );
	lossless->setHint("When no cinema profile is selected, set compression to lossless.");
	lossless->setDefault( false );

	OFX::ChoiceParamDescriptor* cineProfil = desc.defineChoiceParam( kParamCinemaProfil );
	cineProfil->appendOption( "Not Digital Cinema" );
	cineProfil->appendOption( "2K Digital Cinema at 24 fps" );
	cineProfil->appendOption( "2K Digital Cinema at 48 fps" );
	cineProfil->appendOption( "4K Digital Cinema at 24 fps" );
	cineProfil->setDefault( 0 );

	OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam( kWriterParamRender );
	render->setLabels( "Render", "Render", "Render step" );
	render->setHint("Force render (writing)");

	OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam( kWriterParamRenderAlways );
	renderAlways->setLabel( "Render always" );
	renderAlways->setDefault( false );

	OFX::IntParamDescriptor* forceNewRender = desc.defineIntParam( kWriterParamForceNewRender );
	forceNewRender->setLabel( "Force new render" );
	forceNewRender->setIsSecret( true );
//	forceNewRender->setIsPersistant( false );
	forceNewRender->setAnimates( false );
	forceNewRender->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	forceNewRender->setEvaluateOnChange( true );
	forceNewRender->setDefault( 0 );
	OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kJpeg2000HelpButton );
	helpButton->setScriptName( "help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* Jpeg2000WriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new Jpeg2000WriterPlugin(handle);
}

}
}
}
}
