#include "FFMpegWriterPluginFactory.hpp"
#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>
#include <tuttle/plugin/context/WriterPluginFactory.hpp>

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
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamWriterFilename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kParamWriterBitDepth );
	bitDepth->setLabel( "Bit depth" );
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitDepth->setDefault( 0 );


	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
        std::vector<std::string>::const_iterator itLong;
        for( std::vector<std::string>::const_iterator itShort = writer.getFormatsShort().begin(),
             itLong = writer.getFormatsLong().begin(),
             itEnd = writer.getFormatsShort().end();
             itShort != itEnd;
             ++itShort,
             ++itLong )
	{
            std::string name = *itShort;
            name.resize ( 20,' ');
            name += *itLong ;
            format->appendOption( name );
	}
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( 25 );

	OFX::ChoiceParamDescriptor* codec = desc.defineChoiceParam( kParamCodec );
        for( std::vector<std::string>::const_iterator itShort = writer.getCodecsShort().begin(),
             itLong  = writer.getCodecsLong().begin(),
             itEnd = writer.getCodecsShort().end();
             itShort != itEnd;
             ++itShort,
             ++itLong )
	{
            std::string name = *itShort;
            name.resize ( 20,' ');
            name += *itLong ;
            codec->appendOption( name );
	}
	codec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	codec->setDefault( 18 );

	OFX::IntParamDescriptor* bitrate = desc.defineIntParam( kParamBitrate );
	bitrate->setLabel( "Bitrate" );
	bitrate->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitrate->setDefault( 400000 );

        OFX::ChoiceParamDescriptor* motionEstimation = desc.defineChoiceParam( kParamMotionEstimation );
        motionEstimation->setLabel( "Motion Estimation" );
        motionEstimation->appendOption( kParamMEZero  );
        motionEstimation->appendOption( kParamMEFull  );
        motionEstimation->appendOption( kParamMELog   );
        motionEstimation->appendOption( kParamMEPhods );
        motionEstimation->appendOption( kParamMEEpzs  );
        motionEstimation->appendOption( kParamMEXl    );
        motionEstimation->appendOption( kParamMEHex   );
        motionEstimation->appendOption( kParamMEUmh   );
        motionEstimation->appendOption( kParamMEIter  );
        motionEstimation->appendOption( kParamMETesa  );
        motionEstimation->setDefault ( eParamMEEpzs );

        describeWriterParamsInContext( desc, context );
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
