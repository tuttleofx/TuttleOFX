#include "AVReaderPluginFactory.hpp"
#include "AVReaderPlugin.hpp"
#include "AVReaderDefinitions.hpp"

#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Option.hpp>

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}


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

	av_register_all();
	//av_log_set_level( AV_LOG_ERROR );

	std::vector<std::string> supportedExtensions;
	{
		AVInputFormat* iFormat = av_iformat_next( NULL );
		while( iFormat != NULL )
		{
			if( iFormat->extensions != NULL )
			{
				using namespace boost::algorithm;
				std::string extStr( iFormat->extensions );
				std::vector<std::string> exts;
				split( exts, extStr, is_any_of(",") );
				supportedExtensions.insert( supportedExtensions.end(), exts.begin(), exts.end() );

				// name's format defines (in general) extensions
				// require to fix extension in LibAV/FFMpeg to don't use it.
				extStr = iFormat->name;
				split( exts, extStr, is_any_of(",") );
				supportedExtensions.insert( supportedExtensions.end(), exts.begin(), exts.end() );
			}
			iFormat = av_iformat_next( iFormat );
		}
	}

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
	AVFormatContext* avFormatContext;
	avFormatContext = avformat_alloc_context();
	addOptionsToGroup( desc, formatGroup, (void*)avFormatContext, AV_OPT_FLAG_DECODING_PARAM, 0 );
	avformat_free_context( avFormatContext );
	
	/// VIDEO PARAMETERS
	AVCodecContext* avCodecContext;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 8, 0 )
	avCodecContext = avcodec_alloc_context();
	// deprecated in the same version
	//avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
#else
	AVCodec* avCodec = NULL;
	avCodecContext = avcodec_alloc_context3( avCodec );
#endif
	
	addOptionsToGroup( desc, videoGroup, (void*)avCodecContext, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, 0 );
	
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
 * @param av_class: ffmpeg / libav object which contains AVOptions
 * @param req_flags: AVOption flags
 * @param rej_flags: AVOption flags
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, void* av_class, int req_flags, int rej_flags )
{
	avtranscoder::OptionLoader optionLoader;
	optionLoader.loadOptions( av_class, req_flags, rej_flags );
	
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
