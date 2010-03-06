#include "PNGWriterPluginFactory.hpp"
#include "PNGWriterDefinitions.hpp"
#include "PNGWriterPlugin.hpp"
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
namespace png {
namespace writer {

using namespace OFX;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void PNGWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// basic labels
	desc.setLabels( "PNGWriter", "PNGWriter",
	                "PNG File writer" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( eBitDepthUByte );
	desc.addSupportedBitDepth( eBitDepthUShort );
	desc.addSupportedBitDepth( eBitDepthFloat );

	// set a few flags
	desc.setSingleInstance( false );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	desc.setTemporalClipAccess( false );
	desc.setRenderTwiceAlways( false );
	desc.setSupportsMultipleClipPARs( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void PNGWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::ContextEnum            context )
{
	ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	assert(srcClip);
	srcClip->addSupportedComponent( ePixelComponentRGBA );
	srcClip->addSupportedComponent( ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	assert(dstClip);
	dstClip->addSupportedComponent( ePixelComponentRGBA );
	dstClip->addSupportedComponent( ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	StringParamDescriptor* filename = desc.defineStringParam( kOutputFilename );
	assert(filename);
	filename->setLabels( kOutputFilenameLabel, kOutputFilenameLabel, kOutputFilenameLabel );
	filename->setStringType( eStringTypeFilePath );
	filename->setCacheInvalidation( eCacheInvalidateValueAll );

	PushButtonParamDescriptor* renderButton = desc.definePushButtonParam( kRender );
	assert(renderButton);
	renderButton->setLabels( kRenderLabel, kRenderLabel, "Render step" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* PNGWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::ContextEnum     context )
{
	return new PNGWriterPlugin( handle );
}

}
}
}
}
