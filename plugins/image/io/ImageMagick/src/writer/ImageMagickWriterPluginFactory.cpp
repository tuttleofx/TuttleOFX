#include "ImageMagickWriterPluginFactory.hpp"
#include "ImageMagickWriterDefinitions.hpp"
#include "ImageMagickWriterPlugin.hpp"
#include "tuttle/plugin/context/Definition.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <cassert>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageMagickWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleImageMagickWriter", "ImageMagickWriter",
	                "ImageMagick file writer" );
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
void ImageMagickWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext            context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginWriterParamFilename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kTuttlePluginWriterParamBitDepth );
	bitDepth->setLabel( "Bit depth" );
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitDepth->setDefault( 0 );

	OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam( kParamPremult );
	premult->setLabel( "Premult" );
	premult->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	premult->setDefault( true );

	OFX::IntParamDescriptor* quality = desc.defineIntParam( kParamQuality );
	quality->setLabel( "Quality" );
	quality->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	quality->setDefault( 80 );

	OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam( kTuttlePluginWriterParamRender );
	render->setLabels( "Render", "Render", "Render step" );
	render->setHint("Force render (writing)");

	OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam( kTuttlePluginWriterParamRenderAlways );
	renderAlways->setLabel( "Render always" );
	renderAlways->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	renderAlways->setDefault( false );

	OFX::IntParamDescriptor* forceNewRender = desc.defineIntParam( kTuttlePluginWriterParamForceNewRender );
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
OFX::ImageEffect* ImageMagickWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext     context )
{
	return new ImageMagickWriterPlugin( handle );
}

}
}
}
}
