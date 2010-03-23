#include "FFMpegWriterPluginFactory.hpp"
#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

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
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FFMpegWriterPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
	desc.setLabels( "TuttleFfmpeg", "Ffmpeg",
		            "Ffmpeg video io" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void FFMpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                                  OFX::ContextEnum context )
{
	VideoFFmpegWriter writer;
	
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
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kFilename );
	filename->setLabel( "filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );

	OFX::ChoiceParamDescriptor* formatLong = desc.defineChoiceParam( kFormatLong );
	formatLong->setLabel( "format" );
	for( std::vector<std::string>::const_iterator it = writer.getFormatsLong().begin(), itEnd = writer.getFormatsLong().end();
		it != itEnd;
		 ++it )
	{
		formatLong->appendOption( *it );
	}
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kFormat );
	format->setIsSecret( true );
	for( std::vector<std::string>::const_iterator it = writer.getFormatsShort().begin(), itEnd = writer.getFormatsShort().end();
		it != itEnd;
		 ++it )
	{
		format->appendOption( *it );
	}
	format->setDefault( 2 );

	OFX::ChoiceParamDescriptor* codecLong = desc.defineChoiceParam( kCodecLong );
	codecLong->setLabel( "codec" );
	for( std::vector<std::string>::const_iterator it = writer.getCodecsLong().begin(), itEnd = writer.getCodecsLong().end();
		it != itEnd;
		 ++it )
	{
		codecLong->appendOption( *it );
	}
	OFX::ChoiceParamDescriptor* codec = desc.defineChoiceParam( kCodec );
	codec->setIsSecret( true );
	for( std::vector<std::string>::const_iterator it = writer.getCodecsShort().begin(), itEnd = writer.getCodecsShort().end();
		it != itEnd;
		 ++it )
	{
		codec->appendOption( *it );
	}
	codec->setDefault( 2 );

	OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kFFMpegHelpButton );
	helpButton->setScriptName( "help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FFMpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::ContextEnum context )
{
	return new FFMpegWriterPlugin(handle);
}

}
}
}
}
