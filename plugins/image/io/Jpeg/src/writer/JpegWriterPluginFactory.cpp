#include "JpegWriterPluginFactory.hpp"
#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"
#include "tuttle/plugin/context/Definition.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

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

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderUnsafe );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void JpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::ContextEnum            context )
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
	bitDepth->setDefault( 0 );
	bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *bitDepth );

	OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam( kParamPremult );
	premult->setLabel( "Premult" );
	premult->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	premult->setDefault( true );

	OFX::IntParamDescriptor* quality = desc.defineIntParam( kParamQuality );
	quality->setLabel( "Quality" );
	quality->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	quality->setDefault( 80 );

	OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam( kTuttlePluginWriterParamRenderAlways );
	renderAlways->setLabel( "Render always" );
	renderAlways->setDefault( true );

	OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam( kTuttlePluginWriterParamRender );
	render->setLabels( "Render", "Render", "Render step" );
	render->setHint("Force render (writing)");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* JpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::ContextEnum     context )
{
	return new JpegWriterPlugin( handle );
}

}
}
}
}
