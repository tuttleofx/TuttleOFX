#include "FFMpegWriterPluginFactory.hpp"
#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <string>
#include <vector>

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
	desc.setLabels(
		"TuttleFfmpegWriter",
		"FfmpegWriter",
		"Ffmpeg video writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

    std::vector<std::string> supportedExtensions;
	{
		AVOutputFormat* oFormat = av_oformat_next( NULL );
		while( oFormat != NULL )
		{
			if( oFormat->extensions != NULL )
			{
				using namespace boost::algorithm;
				const std::string extStr( oFormat->extensions );
				std::vector<std::string> exts;
				split( exts, extStr, is_any_of(",") );
				
				// remove empty extensions...
				for( std::vector<std::string>::iterator it = exts.begin(); it != exts.end(); )
				{
					if( it->size() == 0 )
						it = exts.erase( it );
					else
						++it;
				}
				supportedExtensions.insert( supportedExtensions.end(), exts.begin(), exts.end() );
			}
			oFormat = av_oformat_next( oFormat );
		}
	}
	
	desc.setDescription( "Video writer based on FFMpeg library\n\n"
			"Supported extensions: \n" +
			boost::algorithm::join( supportedExtensions, ", " )
		);
	
	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	
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
	describeWriterParamsInContext( desc, context );
	OFX::ChoiceParamDescriptor* bitDepth = static_cast<OFX::ChoiceParamDescriptor*>( desc.getParamDescriptor( kTuttlePluginBitDepth ) );
	bitDepth->resetOptions();
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->setDefault( eTuttlePluginBitDepth8 );
	bitDepth->setEnabled( false );

	OFX::BooleanParamDescriptor* premult = static_cast<OFX::BooleanParamDescriptor*>( desc.getParamDescriptor( kParamPremultiplied ) );
	premult->setDefault( true );


	int default_format = 0;
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	std::vector<std::string>::const_iterator itLong;
	for( std::vector<std::string>::const_iterator itShort = writer.getFormatsShort().begin(),
		itLong = writer.getFormatsLong().begin(),
		itEnd = writer.getFormatsShort().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		format->appendOption( *itShort, *itLong );
		if (!strcmp(itShort->c_str(), "mp4"))
			default_format = format->getNOptions() - 1;
	}
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( default_format );

	int default_codec = 0;
	OFX::ChoiceParamDescriptor* codec = desc.defineChoiceParam( kParamCodec );
	for( std::vector<std::string>::const_iterator itShort = writer.getCodecsShort().begin(),
		itLong  = writer.getCodecsLong().begin(),
		itEnd = writer.getCodecsShort().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		codec->appendOption( *itShort, *itLong );
		if (!strcmp(itShort->c_str(), "mpeg4"))
			default_codec = codec->getNOptions() - 1;
	}
	codec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	codec->setDefault( default_codec );

	OFX::IntParamDescriptor* bitrate = desc.defineIntParam( kParamBitrate );
	bitrate->setLabel( "Bitrate" );
	bitrate->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitrate->setDefault( 400000 );

	OFX::ChoiceParamDescriptor* colorSpace = desc.defineChoiceParam( kParamColorSpace );
	colorSpace->setLabel( "Color Space" );
	colorSpace->appendOption( kParamColorSpaceRgb         );
	colorSpace->appendOption( kParamColorSpaceBt709       );
	colorSpace->appendOption( kParamColorSpaceUnspecified );
	colorSpace->appendOption( kParamColorSpaceFcc         );
	colorSpace->appendOption( kParamColorSpaceBt470bg     );
	colorSpace->appendOption( kParamColorSpaceSmpte170m   );
	colorSpace->appendOption( kParamColorSpaceSmpte240m   );
	colorSpace->appendOption( kParamColorSpaceNb          );
	colorSpace->setDefault( AVCOL_SPC_UNSPECIFIED );

	OFX::ChoiceParamDescriptor* colorPrimaries = desc.defineChoiceParam( kParamColorPrimaries );
	colorPrimaries->setLabel( "Color Primaries" );
	colorPrimaries->appendOption( kParamColorPrimariesBt709       );
	colorPrimaries->appendOption( kParamColorPrimariesUnspecified );
	colorPrimaries->appendOption( kParamColorPrimariesBt470m      );
	colorPrimaries->appendOption( kParamColorPrimariesBt470bg     );
	colorPrimaries->appendOption( kParamColorPrimariesSmpte170m   );
	colorPrimaries->appendOption( kParamColorPrimariesSmpte240m   );
	colorPrimaries->appendOption( kParamColorPrimariesFilm        );
	colorPrimaries->appendOption( kParamColorPrimariesNb          );
	colorPrimaries->setDefault( AVCOL_PRI_UNSPECIFIED - 1 );

	OFX::ChoiceParamDescriptor* colorTRC = desc.defineChoiceParam( kParamColorTRC );
	colorTRC->setLabel( "Color Transfer Characteristic" );
	colorTRC->appendOption( kParamColorTRCBt709       );
	colorTRC->appendOption( kParamColorTRCUnspecified );
	colorTRC->appendOption( kParamColorTRCGamma22     );
	colorTRC->appendOption( kParamColorTRCGamma28     );
	colorTRC->appendOption( kParamColorTRCNb          );
	colorTRC->setDefault( AVCOL_TRC_UNSPECIFIED - 1 );

    OFX::ChoiceParamDescriptor* avColorRange = desc.defineChoiceParam( kParamAVColorRange );
    avColorRange->setLabel( "Color Range" );
    avColorRange->appendOption( kParamAVCRUnspecified );
    avColorRange->appendOption( kParamAVCRMpeg        );
    avColorRange->appendOption( kParamAVCRJpeg        );
    avColorRange->appendOption( kParamAVCRNb          );
    avColorRange->setDefault ( AVCOL_RANGE_UNSPECIFIED );


    OFX::ChoiceParamDescriptor* avChromaLocation = desc.defineChoiceParam( kParamAVChromaLocation );
    avChromaLocation->setLabel( "Chroma Location" );
    avChromaLocation->appendOption( kParamAVCLUnspecified );
    avChromaLocation->appendOption( kParamAVCLLeft        );
    avChromaLocation->appendOption( kParamAVCLCenter      );
    avChromaLocation->appendOption( kParamAVCLTopLeft     );
    avChromaLocation->appendOption( kParamAVCLTop         );
    avChromaLocation->appendOption( kParamAVCLBottomLeft  );
    avChromaLocation->appendOption( kParamAVCLBottom      );
    avChromaLocation->appendOption( kParamAVCLNb          );
    avChromaLocation->setDefault (AVCHROMA_LOC_UNSPECIFIED );

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
	motionEstimation->setDefault ( ME_EPZS - 1 );

    OFX::ChoiceParamDescriptor* avDiscard = desc.defineChoiceParam( kParamAVDiscard );
    avDiscard->setLabel( "AV Discard" );
    avDiscard->appendOption( kParamAVDNothing  );
    avDiscard->appendOption( kParamAVDPackets  );
    avDiscard->appendOption( kParamAVDNonRef   );
    avDiscard->appendOption( kParamAVDBidir    );
    avDiscard->appendOption( kParamAVDFrames   );
    avDiscard->appendOption( kParamAVDAll      );
    avDiscard->setDefault ( 0 );

    OFX::ChoiceParamDescriptor* avAudioServiceType = desc.defineChoiceParam( kParamAVAudioServiceType );
    avAudioServiceType->setLabel( "Audio Service Type" );
    avAudioServiceType->appendOption( kParamAVASTMain              );
    avAudioServiceType->appendOption( kParamAVASTEffects           );
    avAudioServiceType->appendOption( kParamAVASTVisuallyImpaired  );
    avAudioServiceType->appendOption( kParamAVASTHearingImpaired   );
    avAudioServiceType->appendOption( kParamAVASTDialogue          );
    avAudioServiceType->appendOption( kParamAVASTCommentary        );
    avAudioServiceType->appendOption( kParamAVASTEmergency         );
    avAudioServiceType->appendOption( kParamAVASTVoiceOver         );
    avAudioServiceType->appendOption( kParamAVASTKaraoke           );
    avAudioServiceType->appendOption( kParamAVASTNb                 );
    //avAudioServiceType->setDefault ( AV_AUDIO_SERVICE_TYPE_MAIN );
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
