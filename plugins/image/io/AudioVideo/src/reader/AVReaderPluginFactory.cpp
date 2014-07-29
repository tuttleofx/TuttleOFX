#include "AVReaderPluginFactory.hpp"
#include "AVReaderPlugin.hpp"
#include "AVReaderDefinitions.hpp"

#include <common/util.hpp>

#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Description.hpp>

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void AVReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"TuttleAVReader",
		"AVReader",
		"Audio Video reader" );
	desc.setPluginGrouping( "tuttle/image/io" );
	
	std::vector<std::string> supportedExtensions( avtranscoder::getInputExtensions() );
	
	// Hack: Add basic video container extensions
	// as some versions of LibAV doesn't declare properly all extensions...
	supportedExtensions.push_back("mov");
	supportedExtensions.push_back("avi");
	supportedExtensions.push_back("mpg");
	supportedExtensions.push_back("mkv");
	supportedExtensions.push_back("flv");
	supportedExtensions.push_back("m2ts");
	
	// sort / unique
	std::sort(supportedExtensions.begin(), supportedExtensions.end());
	supportedExtensions.erase(
		std::unique(supportedExtensions.begin(), supportedExtensions.end()),
		supportedExtensions.end() );

	desc.setDescription( "Video reader based on AvTranscoder library\n\n"
			"Supported extensions: \n" +
			boost::algorithm::join( supportedExtensions, ", " )
		);
	
	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	desc.setPluginEvaluation( 30 );
	
	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderInstanceSafe );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsMultipleClipPARs( true );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void AVReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                               OFX::EContext               context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	describeReaderParamsInContext( desc, context );
	
	// Groups
	OFX::GroupParamDescriptor* formatGroup = desc.defineGroupParam( kParamFormatGroup );
	OFX::GroupParamDescriptor* videoGroup  = desc.defineGroupParam( kParamVideoGroup );
	OFX::GroupParamDescriptor* metaGroup   = desc.defineGroupParam( kParamMetaGroup );
	
	formatGroup->setLabel( "Format" );
	videoGroup->setLabel( "Video" );
	metaGroup->setLabel( "Metadata" );
	
	formatGroup->setAsTab( );
	videoGroup->setAsTab( );
	metaGroup->setAsTab( );
	
	avtranscoder::OptionLoader optionLoader;

	/// FORMAT PARAMETERS
	avtranscoder::OptionLoader::OptionArray formatOptions = optionLoader.loadFormatContextOptions( AV_OPT_FLAG_DECODING_PARAM );
	common::addOptionsToGroup( desc, formatGroup, formatOptions, common::kPrefixFormat );
	
	/// VIDEO PARAMETERS
	avtranscoder::OptionLoader::OptionArray videoOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	common::addOptionsToGroup( desc, videoGroup, videoOptions, common::kPrefixVideo );
	
	/// METADATA PARAMETERS
	avtranscoder::OptionLoader::OptionArray metadataOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_METADATA );
	common::addOptionsToGroup( desc, metaGroup, metadataOptions, common::kPrefixMetaData );
	
	OFX::GroupParamDescriptor* wrapper = desc.defineGroupParam( kParamWrapper );
	wrapper->setLabel( "Wrapper" );
	wrapper->setOpen( false );
	wrapper->setParent( metaGroup );
	
	OFX::StringParamDescriptor* fileName = desc.defineStringParam( kParamFileName );
	fileName->setLabel( "File name" );
	fileName->setEnabled( false );
	fileName->setParent( wrapper );
	
	OFX::StringParamDescriptor* formatName = desc.defineStringParam( kParamFormatName );
	formatName->setLabel( "Format name" );
	formatName->setEnabled( false );
	formatName->setParent( wrapper );
	
	OFX::StringParamDescriptor* formatLongName = desc.defineStringParam( kParamFormatLongName );
	formatLongName->setLabel( "Format long name" );
	formatLongName->setEnabled( false );
	formatLongName->setParent( wrapper );
	
	OFX::DoubleParamDescriptor* startTime = desc.defineDoubleParam( kParamStartTime );
	startTime->setLabel( "Start time" );
	startTime->setEnabled( false );
	startTime->setParent( wrapper );
	
	OFX::DoubleParamDescriptor* duration = desc.defineDoubleParam( kParamDuration );
	duration->setLabel( "Duration" );
	duration->setEnabled( false );
	duration->setParent( wrapper );
	
	OFX::IntParamDescriptor* bitrate = desc.defineIntParam( kParamBitrate );
	bitrate->setLabel( "Bitrate" );
	bitrate->setEnabled( false );
	bitrate->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbStream = desc.defineIntParam( kParamNbStream );
	nbStream->setLabel( "Number of stream" );
	nbStream->setEnabled( false );
	nbStream->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbProgram = desc.defineIntParam( kParamNbProgram );
	nbProgram->setLabel( "Number of program" );
	nbProgram->setEnabled( false );
	nbProgram->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbVideoStream = desc.defineIntParam( kParamNbVideoStream );
	nbVideoStream->setLabel( "Number of video stream" );
	nbVideoStream->setEnabled( false );
	nbVideoStream->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbAudioStream = desc.defineIntParam( kParamNbAudioStream );
	nbAudioStream->setLabel( "Number of audio stream" );
	nbAudioStream->setEnabled( false );
	nbAudioStream->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbDataStream = desc.defineIntParam( kParamNbDataStream );
	nbDataStream->setLabel( "Number of data stream" );
	nbDataStream->setEnabled( false );
	nbDataStream->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbSubtitleStream = desc.defineIntParam( kParamNbSubtitleStream );
	nbSubtitleStream->setLabel( "Number of subtitle stream" );
	nbSubtitleStream->setEnabled( false );
	nbSubtitleStream->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbAttachementStream = desc.defineIntParam( kParamNbAttachementStream );
	nbAttachementStream->setLabel( "Number of attachement stream" );
	nbAttachementStream->setEnabled( false );
	nbAttachementStream->setParent( wrapper );
	
	OFX::IntParamDescriptor* nbUnknownStream = desc.defineIntParam( kParamNbUnknownStream );
	nbUnknownStream->setLabel( "Number of unknown stream" );
	nbUnknownStream->setEnabled( false );
	nbUnknownStream->setParent( wrapper );
	
	// OTHER
	OFX::BooleanParamDescriptor* useCustomSAR = desc.defineBooleanParam( kParamUseCustomSAR );
	useCustomSAR->setLabel( "Override SAR" );
	useCustomSAR->setDefault( false );
	useCustomSAR->setHint( "Override the file SAR (Storage Aspect Ratio) with a custom SAR value." );

	OFX::DoubleParamDescriptor* customSAR = desc.defineDoubleParam( kParamCustomSAR );
	customSAR->setLabel( "Custom SAR" );
	customSAR->setDefault( 1.0 );
	customSAR->setDisplayRange( 0., 3. );
	customSAR->setRange( 0., 10. );
	customSAR->setHint( "Choose a custom value to override the file SAR (Storage Aspect Ratio). Maximum value: 10." );
	
	OFX::IntParamDescriptor* streamIndex = desc.defineIntParam( kParamVideoStreamIndex );
	streamIndex->setLabel( kParamVideoStreamIndexLabel );
	streamIndex->setDefault( 0 );
	streamIndex->setDisplayRange( 0., 16. );
	streamIndex->setRange( 0., 100. );
	streamIndex->setHint( "Choose a custom value to decode the video stream you want. Maximum value: 100." );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* AVReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                         OFX::EContext        context )
{
	return new AVReaderPlugin( handle );
}

}
}
}
}
