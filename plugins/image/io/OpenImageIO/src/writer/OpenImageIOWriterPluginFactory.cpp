#include "OpenImageIOWriterPluginFactory.hpp"
#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

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

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

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
OFX::ImageEffect* OpenImageIOWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
	return new OpenImageIOWriterPlugin( handle );
}

}
}
}
}
