#include "AVWriterPluginFactory.hpp"
#include "AVWriterPlugin.hpp"
#include "AVWriterDefinitions.hpp"

#include <common/util.hpp>

#include <libav/LibAVPreset.hpp>
#include <libav/LibAVFormatPreset.hpp>
#include <libav/LibAVVideoPreset.hpp>
#include <libav/LibAVAudioPreset.hpp>

#include <AvTranscoder/Description.hpp>
#include <AvTranscoder/OptionLoader.hpp>

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
	#include <libavutil/pixdesc.h>
}

#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>

#include <vector>
#include <string>
#include <utility> //pair

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void AVWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"TuttleAVWriter",
		"AVWriter",
		"Audio Video writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	std::vector<std::string> supportedExtensions( avtranscoder::getOutputExtensions() );
	
	desc.setDescription( "Video writer based on LibAV library\n\n"
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
void AVWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, 
                                               OFX::EContext               context )
{
	avtranscoder::OptionLoader optionLoader;
	
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
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	/// @todo: 16 bits
	bitDepth->setDefault( eTuttlePluginBitDepth8 );
	bitDepth->setEnabled( false );

	OFX::BooleanParamDescriptor* premult = static_cast<OFX::BooleanParamDescriptor*>( desc.getParamDescriptor( kParamPremultiplied ) );
	premult->setDefault( true );
	
	/// MAIN PRESET SELECTOR
	OFX::ChoiceParamDescriptor* mainPreset = desc.defineChoiceParam( kParamMainPreset );
	mainPreset->setLabel( "Main Preset" );
	mainPreset->appendOption( "custom", "Customized configuration" );
	
	std::vector<std::string> idList;
	std::vector<std::string> idLabelList;
	LibAVPreset::getPresetList( idList, idLabelList );
	for( unsigned int it = 0; it < idList.size(); ++it )
	{
		mainPreset->appendOption( idList.at( it ), idLabelList.at( it ) );
	}
	
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
	/// format preset
	OFX::ChoiceParamDescriptor* formatPreset = desc.defineChoiceParam( kParamFormatPreset );
	formatPreset->setLabel( "Format Preset" );
	formatPreset->appendOption( "custom", "Customized configuration" );
	std::vector<std::string> idFormatList;
	std::vector<std::string> idFormatLabelList;
	LibAVFormatPreset::getPresetList( idFormatList, idFormatLabelList );
	for( unsigned int it = 0; it < idFormatList.size(); ++it )
	{
		formatPreset->appendOption( idFormatList.at( it ), idFormatLabelList.at( it ) );
	}
	formatPreset->setParent( formatGroup );
	
	/// format list
	int default_format = 0;
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	for( std::vector<std::string>::const_iterator itShort = optionLoader.getFormatsShortNames().begin(),
		itLong = optionLoader.getFormatsLongNames().begin(),
		itEnd = optionLoader.getFormatsShortNames().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		format->appendOption( *itShort, *itLong );
		if( (*itShort) == "mp4" )
			default_format = format->getNOptions() - 1;
	}
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( default_format );
	format->setParent( formatGroup );
	
	avtranscoder::OptionLoader::OptionArray formatGroupOptions = optionLoader.loadFormatContextOptions( AV_OPT_FLAG_ENCODING_PARAM );
	common::addOptionsToGroup( desc, formatGroup, formatGroupOptions );
	
	/// format parameters
	OFX::GroupParamDescriptor* formatDetailledGroup = desc.defineGroupParam( kParamFormatDetailledGroup );
	formatDetailledGroup->setLabel( "Detailled" );
	formatDetailledGroup->setAsTab( );
	formatDetailledGroup->setParent( formatGroup );
	
	avtranscoder::OptionLoader::OptionMap formatDetailledGroupOptions = optionLoader.loadOutputFormatOptions();
	common::addOptionsToGroup( desc, formatDetailledGroup, formatDetailledGroupOptions );
	
	// fps parameters
	OFX::BooleanParamDescriptor* useCustomFps = desc.defineBooleanParam( kParamUseCustomFps );
	useCustomFps->setLabel( "Override Fps" );
	useCustomFps->setDefault( false );
	useCustomFps->setHint( "Override the input Fps (Frames Per Second) with a custom Fps value." );

	OFX::DoubleParamDescriptor* customFps = desc.defineDoubleParam( kParamCustomFps );
	customFps->setLabel( "Custom Fps" );
	customFps->setDefault( 1.0 );
	customFps->setHint( "Choose a custom value to override the Fps (Frames Per Second)." );
	
	/// VIDEO PARAMETERS
	/// video codec preset
	OFX::ChoiceParamDescriptor* videoPreset = desc.defineChoiceParam( kParamVideoPreset );
	videoPreset->setLabel( "Video Preset" );
	videoPreset->appendOption( "custom", "Customized configuration" );
	
	std::vector<std::string> idVideoList;
	std::vector<std::string> idVideoLabelList;
	LibAVVideoPreset::getPresetList( idVideoList, idVideoLabelList );
	for( unsigned int it = 0; it < idVideoList.size(); ++it )
	{
		videoPreset->appendOption( idVideoList.at( it ), idVideoLabelList.at( it ) );
	}
	videoPreset->setParent( videoGroup );
	
	/// video codec list
	int default_codec = 0;
	OFX::ChoiceParamDescriptor* videoCodec = desc.defineChoiceParam( kParamVideoCodec );
	for( std::vector<std::string>::const_iterator itShort = optionLoader.getVideoCodecsShortNames().begin(),
		itLong  = optionLoader.getVideoCodecsLongNames().begin(),
		itEnd = optionLoader.getVideoCodecsShortNames().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		videoCodec->appendOption( *itShort, *itLong );
		if( (*itShort) == "mpeg4" )
			default_codec = videoCodec->getNOptions() - 1;
	}
	videoCodec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	videoCodec->setDefault( default_codec );
	videoCodec->setParent( videoGroup );
	
	/// video codec parameters
	OFX::ChoiceParamDescriptor* videoCodecPixelFmt = desc.defineChoiceParam( kParamVideoCodecPixelFmt );
	videoCodecPixelFmt->setLabel( kParamVideoCodecPixelFmt );
	videoCodecPixelFmt->setLabel( "Select the output video pixel type." );
	
	for( int pix_fmt = 0; pix_fmt < PIX_FMT_NB; pix_fmt++ )
	{
		const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[pix_fmt];
		if(!pix_desc->name)
			continue;
		videoCodecPixelFmt->appendOption( pix_desc->name );
	}
	videoCodecPixelFmt->setParent( videoGroup );
	
	avtranscoder::OptionLoader::OptionArray videoGroupOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	common::addOptionsToGroup( desc, videoGroup, videoGroupOptions );
	
	OFX::GroupParamDescriptor* videoDetailledGroup  = desc.defineGroupParam( kParamVideoDetailledGroup );
	videoDetailledGroup->setLabel( "Detailled" );
	videoDetailledGroup->setAsTab( );
	videoDetailledGroup->setParent( videoGroup );
	
	avtranscoder::OptionLoader::OptionMap videoDetailledGroupOptions = optionLoader.loadVideoCodecOptions(); 
	common::addOptionsToGroup( desc, videoDetailledGroup, videoDetailledGroupOptions );
	
	/// AUDIO PARAMETERS
	/// audio codec preset
	OFX::ChoiceParamDescriptor* audioPreset = desc.defineChoiceParam( kParamAudioPreset );
	audioPreset->setLabel( "Audio Preset" );
	audioPreset->appendOption( "custom", "Customized configuration" );
	
	std::vector<std::string> idAudioList;
	std::vector<std::string> idAudioLabelList;
	LibAVAudioPreset::getPresetList( idAudioList, idAudioLabelList );
	for( unsigned int it = 0; it < idAudioList.size(); ++it )
	{
		audioPreset->appendOption( idAudioList.at( it ), idAudioLabelList.at( it ) );
	}
	audioPreset->setParent( audioGroup );
	
	/// audio codec list
	int default_audio_codec = 0;
	OFX::ChoiceParamDescriptor* audioCodec = desc.defineChoiceParam( kParamAudioCodec );
	for( std::vector<std::string>::const_iterator itShort = optionLoader.getAudioCodecsShortNames().begin(),
		itLong  = optionLoader.getAudioCodecsLongNames().begin(),
		itEnd = optionLoader.getAudioCodecsShortNames().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		audioCodec->appendOption( *itShort, *itLong );
		if( (*itShort) == "pcm_s24be" )
			default_audio_codec = audioCodec->getNOptions() - 1;
	}
	audioCodec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	audioCodec->setDefault( default_audio_codec );
	audioCodec->setParent( audioGroup );
	
	/// audio codec parameters
	avtranscoder::OptionLoader::OptionArray audioGroupOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM );
	common::addOptionsToGroup( desc, audioGroup, audioGroupOptions );
	
	OFX::GroupParamDescriptor* audioDetailledGroup  = desc.defineGroupParam( kParamAudioDetailledGroup );
	audioDetailledGroup->setLabel( "Detailled" );
	audioDetailledGroup->setAsTab( );
	audioDetailledGroup->setParent( audioGroup );
	
	avtranscoder::OptionLoader::OptionMap audioDetailledGroupOptions = optionLoader.loadAudioCodecOptions();
	common::addOptionsToGroup( desc, audioDetailledGroup, audioDetailledGroupOptions );
	
	/// METADATA PARAMETERS
	std::map<std::string, std::string> keys;
	std::map<std::string, std::string> hints;

	keys[ kParamMetaAlbum           ] = "Album";
	keys[ kParamMetaAlbumArtist     ] = "Album Artist";
	keys[ kParamMetaArtist          ] = "Artist";
	keys[ kParamMetaComment         ] = "Comment";
	keys[ kParamMetaComposer        ] = "Composer";
	keys[ kParamMetaCopyright       ] = "Copyright";
	keys[ kParamMetaCreationTime    ] = "Creation Time";
	keys[ kParamMetaDate            ] = "Date";
	keys[ kParamMetaDisc            ] = "Disc";
	keys[ kParamMetaEncoder         ] = "Encoder";
	keys[ kParamMetaEncodedBy       ] = "Encoded by";
	keys[ kParamMetaFilename        ] = "Filename";
	keys[ kParamMetaGenre           ] = "Genre";
	keys[ kParamMetaLanguage        ] = "Language";
	keys[ kParamMetaPerformer       ] = "Performer";
	keys[ kParamMetaPublisher       ] = "Publisher";
	keys[ kParamMetaServiceName     ] = "Service Pame";
	keys[ kParamMetaServiceProvider ] = "Service Provider";
	keys[ kParamMetaTitle           ] = "Title";
	keys[ kParamMetaTrack           ] = "Track";
	keys[ kParamMetaVariantBitrate  ] = "Variant Bitrate";

	hints[ kParamMetaAlbum           ] = "name of the set this work belongs to";
	hints[ kParamMetaAlbumArtist     ] = "main creator of the set/album, if different from artist. (e.g. 'Various Artists' for compilation albums.)";
	hints[ kParamMetaArtist          ] = "main creator of the work";
	hints[ kParamMetaComment         ] = "any additional description of the file.";
	hints[ kParamMetaComposer        ] = "who composed the work, if different from artist.";
	hints[ kParamMetaCopyright       ] = "name of copyright holder.";
	hints[ kParamMetaCreationTime    ] = "time when the file was created, preferably in ISO 8601.";
	hints[ kParamMetaDate            ] = "date when the file was created, preferably in ISO 8601.";
	hints[ kParamMetaDisc            ] = "number of a subset, e.g. disc in a multi-disc collection.";
	hints[ kParamMetaEncoder         ] = "name/settings of the software/hardware that produced the file.";
	hints[ kParamMetaEncodedBy       ] = "person/group who created the file.";
	hints[ kParamMetaFilename        ] = "original name of the file.";
	hints[ kParamMetaGenre           ] = "self-evident>.";
	hints[ kParamMetaLanguage        ] = "main language in which the work is performed, preferably in ISO 639-2 format. Multiple languages can be specified by separating them with commas.";
	hints[ kParamMetaPerformer       ] = "artist who performed the work, if different from artist. ( E.g for 'Also sprach Zarathustra', artist would be 'Richard Strauss' and performer 'London Philharmonic Orchestra'.";
	hints[ kParamMetaPublisher       ] = "name of the label/publisher.";
	hints[ kParamMetaServiceName     ] = "name of the service in broadcasting (channel name).";
	hints[ kParamMetaServiceProvider ] = "name of the service provider in broadcasting.";
	hints[ kParamMetaTitle           ] = "name of the work.";
	hints[ kParamMetaTrack           ] = "number of this work in the set, can be in form current/total.";
	hints[ kParamMetaVariantBitrate  ] = "the total bitrate of the bitrate variant that the current stream is part of";

	std::map<std::string, std::string>::iterator ritKeys;
	std::map<std::string, std::string>::iterator ritHint;

	for( ritKeys = keys.begin(), ritHint = hints.begin();
		 ritKeys != keys.end();
		 ++ritKeys, ++ritHint )
	{
		OFX::StringParamDescriptor* metaParam = desc.defineStringParam( ritKeys->first );
		metaParam->setLabel( ritKeys->second );
		metaParam->setHint( ritHint->second );
		metaParam->setParent( metaGroup );

		if( ritKeys->first == kParamMetaEncoder )
		{
			std::ostringstream os;
			os << "TuttleOFX AudioVideo ";
			os << getMajorVersion();
			os << ".";
			os << getMinorVersion();

			metaParam->setDefault( os.str() );
			metaParam->setEnabled( false );
		}
	}

	OFX::GroupParamDescriptor* metaDetailledGroup   = desc.defineGroupParam( kParamMetaDetailledGroup );
	metaDetailledGroup->setLabel( "Detailled" );
	metaDetailledGroup->setAsTab( );
	metaDetailledGroup->setParent( metaGroup );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* AVWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
							     OFX::EContext        context )
{
	return new AVWriterPlugin( handle );
}

}
}
}
}
