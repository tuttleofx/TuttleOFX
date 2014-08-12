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
	
	OFX::GroupParamDescriptor* formatDetailledGroup = desc.defineGroupParam( kParamFormatDetailledGroup );
	formatDetailledGroup->setLabel( "Detailled" );
	formatDetailledGroup->setAsTab( );
	formatDetailledGroup->setParent( formatGroup );
	
	avtranscoder::OptionLoader::OptionMap formatDetailledGroupOptions = optionLoader.loadOutputFormatOptions();
	common::addOptionsToGroup( desc, formatDetailledGroup, formatDetailledGroupOptions, common::kPrefixFormat );
	
	/// VIDEO PARAMETERS
	avtranscoder::OptionLoader::OptionArray videoOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	common::addOptionsToGroup( desc, videoGroup, videoOptions, common::kPrefixVideo );
	
	OFX::BooleanParamDescriptor* useCustomSAR = desc.defineBooleanParam( kParamUseCustomSAR );
	useCustomSAR->setLabel( "Override SAR" );
	useCustomSAR->setDefault( false );
	useCustomSAR->setHint( "Override the file SAR (Storage Aspect Ratio) with a custom SAR value." );
	useCustomSAR->setParent( videoGroup );

	OFX::DoubleParamDescriptor* customSAR = desc.defineDoubleParam( kParamCustomSAR );
	customSAR->setLabel( "Custom SAR" );
	customSAR->setDefault( 1.0 );
	customSAR->setDisplayRange( 0., 3. );
	customSAR->setRange( 0., 10. );
	customSAR->setHint( "Choose a custom value to override the file SAR (Storage Aspect Ratio). Maximum value: 10." );
	customSAR->setParent( videoGroup );

	OFX::IntParamDescriptor* streamIndex = desc.defineIntParam( kParamVideoStreamIndex );
	streamIndex->setLabel( kParamVideoStreamIndexLabel );
	streamIndex->setDefault( 0 );
	streamIndex->setDisplayRange( 0., 16. );
	streamIndex->setRange( 0., 100. );
	streamIndex->setHint( "Choose a custom value to decode the video stream you want. Maximum value: 100." );
	streamIndex->setParent( videoGroup );

	OFX::GroupParamDescriptor* videoDetailledGroup  = desc.defineGroupParam( kParamVideoDetailledGroup );
	videoDetailledGroup->setLabel( "Detailled" );
	videoDetailledGroup->setAsTab( );
	videoDetailledGroup->setParent( videoGroup );
	
	avtranscoder::OptionLoader::OptionMap videoDetailledGroupOptions = optionLoader.loadVideoCodecOptions(); 
	common::addOptionsToGroup( desc, videoDetailledGroup, videoDetailledGroupOptions, common::kPrefixVideo );
	
	/// METADATA PARAMETERS
	avtranscoder::OptionLoader::OptionArray metadataOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_METADATA );
	common::addOptionsToGroup( desc, metaGroup, metadataOptions, common::kPrefixMetaData );
	
	OFX::StringParamDescriptor* metaDataWrapper = desc.defineStringParam( kParamMetaDataWrapper );
	metaDataWrapper->setLabel( "Wrapper" );
	metaDataWrapper->setEnabled( false );
	metaDataWrapper->setStringType( OFX::eStringTypeMultiLine );
	metaDataWrapper->setParent( metaGroup );

	OFX::StringParamDescriptor* metaDataVideo = desc.defineStringParam( kParamMetaDataVideo );
	metaDataVideo->setLabel( "Video" );
	metaDataVideo->setEnabled( false );
	metaDataVideo->setStringType( OFX::eStringTypeMultiLine );
	metaDataVideo->setParent( metaGroup );

	OFX::StringParamDescriptor* metaDataAudio = desc.defineStringParam( kParamMetaDataAudio );
	metaDataAudio->setLabel( "Audio" );
	metaDataAudio->setEnabled( false );
	metaDataAudio->setStringType( OFX::eStringTypeMultiLine );
	metaDataAudio->setParent( metaGroup );

	OFX::StringParamDescriptor* metaDataData = desc.defineStringParam( kParamMetaDataData );
	metaDataData->setLabel( "Data" );
	metaDataData->setEnabled( false );
	metaDataData->setStringType( OFX::eStringTypeMultiLine );
	metaDataData->setParent( metaGroup );

	OFX::StringParamDescriptor* metaDataSubtitle = desc.defineStringParam( kParamMetaDataSubtitle );
	metaDataSubtitle->setLabel( "Subtitle" );
	metaDataSubtitle->setEnabled( false );
	metaDataSubtitle->setStringType( OFX::eStringTypeMultiLine );
	metaDataSubtitle->setParent( metaGroup );

	OFX::StringParamDescriptor* metaDataAttachement = desc.defineStringParam( kParamMetaDataAttachement );
	metaDataAttachement->setLabel( "Attachement" );
	metaDataAttachement->setEnabled( false );
	metaDataAttachement->setStringType( OFX::eStringTypeMultiLine );
	metaDataAttachement->setParent( metaGroup );

	OFX::StringParamDescriptor* metaDataUnknown = desc.defineStringParam( kParamMetaDataUnknown );
	metaDataUnknown->setLabel( "Unknown" );
	metaDataUnknown->setEnabled( false );
	metaDataUnknown->setStringType( OFX::eStringTypeMultiLine );
	metaDataUnknown->setParent( metaGroup );
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
