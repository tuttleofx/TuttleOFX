#include "FFMpegWriterPluginFactory.hpp"
#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FFMpegWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleFfmpegWriter", "FfmpegWriter",
	                "Ffmpeg video writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderInstanceSafe );
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
void FFMpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                   OFX::EContext               context )
{
	VideoFFmpegWriter writer;

	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamFilename );
	filename->setLabel( "filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );

	OFX::ChoiceParamDescriptor* formatLong = desc.defineChoiceParam( kParamFormatLong );
	formatLong->setLabel( "format" );
	for( std::vector<std::string>::const_iterator it = writer.getFormatsLong().begin(), itEnd = writer.getFormatsLong().end();
	     it != itEnd;
	     ++it )
	{
		formatLong->appendOption( *it );
	}
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	format->setIsSecret( true );
	for( std::vector<std::string>::const_iterator it = writer.getFormatsShort().begin(), itEnd = writer.getFormatsShort().end();
	     it != itEnd;
	     ++it )
	{
		format->appendOption( *it );
	}
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( 25 );

	OFX::ChoiceParamDescriptor* codecLong = desc.defineChoiceParam( kParamCodecLong );
	codecLong->setLabel( "codec" );
	for( std::vector<std::string>::const_iterator it = writer.getCodecsLong().begin(), itEnd = writer.getCodecsLong().end();
	     it != itEnd;
	     ++it )
	{
		codecLong->appendOption( *it );
	}
	OFX::ChoiceParamDescriptor* codec = desc.defineChoiceParam( kParamCodec );
	codec->setIsSecret( true );
	for( std::vector<std::string>::const_iterator it = writer.getCodecsShort().begin(), itEnd = writer.getCodecsShort().end();
	     it != itEnd;
	     ++it )
	{
		codec->appendOption( *it );
	}
	codec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	codec->setDefault( 18 );

	OFX::IntParamDescriptor* bitrate = desc.defineIntParam( kParamBitrate );
	bitrate->setLabel( "bitrate" );
	bitrate->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitrate->setDefault( 400000 );

	OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam( kParamRenderAlways );
	renderAlways->setLabel( "Render always" );
	renderAlways->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	renderAlways->setDefault( false );

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kParamFFMpegHelpButton );
	helpButton->setScriptName( "help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FFMpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                             OFX::EContext        context )
{
	return new FFMpegWriterPlugin( handle );
}

}
}
}
}
