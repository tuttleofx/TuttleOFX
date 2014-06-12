#include "AVReaderPluginFactory.hpp"
#include "AVReaderPlugin.hpp"
#include "AVReaderDefinitions.hpp"

#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Option.hpp>
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

	desc.setDescription( "Video reader based on LibAV library\n\n"
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
	OFX::GroupParamDescriptor* audioGroup  = desc.defineGroupParam( kParamAudioGroup );
	OFX::GroupParamDescriptor* metaGroup   = desc.defineGroupParam( kParamMetaGroup );
	
	formatGroup->setLabel( "Format" );
	videoGroup->setLabel( "Video" );
	audioGroup->setLabel( "Audio" );
	metaGroup->setLabel( "Metadata" );
	
	formatGroup->setAsTab( );
	videoGroup->setAsTab( );
	audioGroup->setAsTab( );
	metaGroup->setAsTab( );
	
	/// FORMAT PARAMETERS
	addOptionsToGroup( desc, formatGroup, AV_OPT_FLAG_DECODING_PARAM );
	
	/// VIDEO PARAMETERS
	addOptionsToGroup( desc, videoGroup, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	
	/// AUDIO PARAMETERS
	addOptionsToGroup( desc, audioGroup, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM );
	
	/// METADATA PARAMETERS
	addOptionsToGroup( desc, metaGroup, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_METADATA );
	
	OFX::BooleanParamDescriptor* useCustomSAR = desc.defineBooleanParam( kParamUseCustomSAR );
	useCustomSAR->setLabel( "Override SAR" );
	useCustomSAR->setDefault( false );
	useCustomSAR->setHint( "Override the file SAR (Storage Aspect Ratio) with a custom SAR value." );

	OFX::DoubleParamDescriptor* customSAR = desc.defineDoubleParam( kParamCustomSAR );
	customSAR->setLabel( "Custom SAR" );
	customSAR->setDefault( 1.0 );
	customSAR->setHint( "Choose a custom value to override the file SAR (Storage Aspect Ratio)." );
	
	OFX::IntParamDescriptor* streamIndex = desc.defineIntParam( kParamVideoStreamIndex );
	streamIndex->setLabel( kParamVideoStreamIndexLabel );
	streamIndex->setDefault( 0 );
	streamIndex->setHint( "Choose a custom value to decode the video stream you want." );
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

/**
 * @brief Create OFX parameters depending on the list of Options found.
 * Get AVOption of ffmpeg / libav from an OptionLoader in avTranscoder lib.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param req_flags: AVOption flags
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, int req_flags )
{
	avtranscoder::OptionLoader optionLoader;
	optionLoader.loadOptions( req_flags );
	
	// ValueDescriptor ?
	OFX::ParamDescriptor* param = NULL;
	BOOST_FOREACH( avtranscoder::Option& option, optionLoader.getOptions() )
	{
		switch( option.getType() )
		{
			case avtranscoder::TypeBool:
			{
				OFX::BooleanParamDescriptor* boolParam = desc.defineBooleanParam( option.getName() );
				boolParam->setDefault( option.getDefaultValueBool() );
				param = boolParam;
				break;
			}
			case avtranscoder::TypeInt:
			{
				OFX::IntParamDescriptor* intParam = desc.defineIntParam( option.getName() );
				intParam->setDefault( option.getDefaultValueInt() );
				param = intParam;
				break;
			}
			case avtranscoder::TypeDouble:
			{
				OFX::DoubleParamDescriptor* doubleParam = desc.defineDoubleParam( option.getName() );
				doubleParam->setDefault( option.getDefaultValueDouble() );
				param = doubleParam;
				break;
			}
			case avtranscoder::TypeString:
			{
				OFX::StringParamDescriptor* strParam = desc.defineStringParam( option.getName() );
				strParam->setDefault( option.getDefaultValueString() );
				param = strParam;
				break;
			}
			case avtranscoder::TypeRatio:
			{
				OFX::Int2DParamDescriptor* ratioParam = desc.defineInt2DParam( option.getName() );
				ratioParam->setDefault( option.getDefaultValueRatio().first, option.getDefaultValueRatio().second );
				param = ratioParam;
				break;
			}
			case avtranscoder::TypeChoice:
			{
				OFX::ChoiceParamDescriptor* choiceParam = desc.defineChoiceParam( option.getName() );
				choiceParam->setDefault( option.getDefaultChildIndex() );
				for( size_t i = 0; i < option.getNbChilds(); ++i )
					choiceParam->appendOption( option.getChild( i ).getName(), option.getChild( i ).getHelp() );
				param = choiceParam;
				break;
			}
			case avtranscoder::TypeGroup:
			{
				std::string groupName = "g_";
				groupName += option.getName();
				OFX::GroupParamDescriptor* groupParam = desc.defineGroupParam( groupName );
				for( size_t i = 0; i < option.getNbChilds(); ++i )
				{
					OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( option.getChild( i ).getName() );
					param->setDefault( option.getChild( i ).getOffset() );
					param->setHint( option.getChild( i ).getHelp() );
					param->setParent( groupParam );
				}
				param = groupParam;
				break;
			}
			default:
				break;
		}
		if( param )
		{
			param->setLabel( option.getName() );
			param->setHint( option.getHelp() );
			param->setParent( group );
		}
	}
}

}
}
}
}
