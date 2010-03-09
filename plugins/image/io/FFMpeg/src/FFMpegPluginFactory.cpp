#include "FFMpegPluginFactory.hpp"
#include "FFMpegPlugin.hpp"
#include "FFMpegDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

using namespace OFX;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FFMpegPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
	// basic labels
	desc.setLabels( "ffmpeg", "ffmpeg",
		            "ffmpeg video io" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( eBitDepthUByte );
	desc.addSupportedBitDepth( eBitDepthUShort );
	desc.addSupportedBitDepth( eBitDepthFloat );

	// set a few flags
	desc.setSingleInstance( false );
	desc.setHostFrameThreading( true );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	desc.setTemporalClipAccess( kSupportTemporalClipAccess );
	desc.setRenderTwiceAlways( false );
	desc.setSupportsMultipleClipPARs( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void FFMpegPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                                  OFX::ContextEnum context )
{
	OFX::ClipDescriptor *srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( ePixelComponentRGBA );
	srcClip->addSupportedComponent( ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( ePixelComponentRGBA );
	dstClip->addSupportedComponent( ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kFFMpegHelpButton );
	helpButton->setScriptName( "help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FFMpegPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::ContextEnum context )
{
	return new FFMpegPlugin(handle);
}

}
}
}
}

namespace OFX {
namespace Plugin {

void getPluginIDs(OFX::PluginFactoryArray &ids)
{
	static tuttle::plugin::ffmpeg::reader::FFMpegPluginFactory p("fr.tuttle.ffmpeg", 1, 0);
	ids.push_back(&p);
}

}
}
