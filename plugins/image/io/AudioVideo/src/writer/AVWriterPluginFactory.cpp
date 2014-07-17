#include "AVWriterPluginFactory.hpp"
#include "AVWriterPlugin.hpp"
#include "AVWriterDefinitions.hpp"

#include <common/util.hpp>

#include <AvTranscoder/Description.hpp>
#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Profile.hpp>

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <vector>
#include <string>
#include <utility> //pair
#include <sstream>

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
	
	desc.setDescription( "Video writer based on AvTranscoder library\n\n"
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
	
	avtranscoder::Profile presets( true );
	
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
	// @todo: add presets
	
	// Groups
	OFX::GroupParamDescriptor* formatGroup = desc.defineGroupParam( kParamFormatGroup );
	OFX::GroupParamDescriptor* videoGroup  = desc.defineGroupParam( kParamVideoGroup );
	OFX::GroupParamDescriptor* audioGroup  = desc.defineGroupParam( kParamAudioGroup );
	OFX::GroupParamDescriptor* metaGroup   = desc.defineGroupParam( kParamMetaGroup );
	OFX::GroupParamDescriptor* aboutGroup = desc.defineGroupParam( kParamAboutGroup );
	
	formatGroup->setLabel( "Format" );
	videoGroup->setLabel( "Video" );
	audioGroup->setLabel( "Audio" );
	metaGroup->setLabel( "Metadata" );
	aboutGroup->setLabel( "About" );
	
	formatGroup->setAsTab( );
	videoGroup->setAsTab( );
	audioGroup->setAsTab( );
	metaGroup->setAsTab( );
	aboutGroup->setAsTab( );
	
	/// FORMAT PARAMETERS
	/// format preset
	OFX::ChoiceParamDescriptor* formatPreset = desc.defineChoiceParam( kParamFormatPreset );
	formatPreset->setLabel( "Format Preset" );
	formatPreset->appendOption( "custom", "Customized configuration" );
	// @todo: add presets
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
	common::addOptionsToGroup( desc, formatGroup, formatGroupOptions, common::kPrefixFormat );
	
	/// format parameters
	OFX::GroupParamDescriptor* formatDetailledGroup = desc.defineGroupParam( kParamFormatDetailledGroup );
	formatDetailledGroup->setLabel( "Detailled" );
	formatDetailledGroup->setAsTab( );
	formatDetailledGroup->setParent( formatGroup );
	
	avtranscoder::OptionLoader::OptionMap formatDetailledGroupOptions = optionLoader.loadOutputFormatOptions();
	common::addOptionsToGroup( desc, formatDetailledGroup, formatDetailledGroupOptions, common::kPrefixFormat );
	
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
	/// video preset
	OFX::ChoiceParamDescriptor* videoMainPresetParam = desc.defineChoiceParam( kParamMainVideoPreset );
	videoMainPresetParam->setLabel( "Video Preset" );
	videoMainPresetParam->appendOption( "custom", "Customized configuration" );
	videoMainPresetParam->setParent( videoGroup );
	
	avtranscoder::Profile::ProfilesDesc videoPresets = presets.getVideoProfiles();
	for( avtranscoder::Profile::ProfilesDesc::iterator it = videoPresets.begin(); it != videoPresets.end(); ++it )
	{
		videoMainPresetParam->appendOption( 
			(*it).find( avtranscoder::Profile::avProfileIdentificator )->second, 
			(*it).find( avtranscoder::Profile::avProfileIdentificatorHuman )->second
		);
	}
	
	// add group to manage option when custom preset
	OFX::GroupParamDescriptor* videoCustomGroupParam = desc.defineGroupParam( kParamVideoCustomGroup );
	videoCustomGroupParam->setLabel( "Video custom parameters" );
	videoCustomGroupParam->setHint( "Contains expert params, use to write video streams when custom preset is specified." );
	videoCustomGroupParam->setParent( videoGroup );
	
	/// video codec list
	int default_codec = 0;
	std::string defaultVideoCodec( "mpeg4" );
	OFX::ChoiceParamDescriptor* videoCodec = desc.defineChoiceParam( kParamVideoCodec );
	for( std::vector<std::string>::const_iterator itShort = optionLoader.getVideoCodecsShortNames().begin(),
		itLong  = optionLoader.getVideoCodecsLongNames().begin(),
		itEnd = optionLoader.getVideoCodecsShortNames().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		videoCodec->appendOption( *itShort, *itLong );
		if( (*itShort) == defaultVideoCodec )
			default_codec = videoCodec->getNOptions() - 1;
	}
	videoCodec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	videoCodec->setDefault( default_codec );
	videoCodec->setParent( videoCustomGroupParam );
	
	/// video pixel formats list
	OFX::ChoiceParamDescriptor* videoCodecPixelFmt = desc.defineChoiceParam( kParamVideoCodecPixelFmt );
	videoCodecPixelFmt->setLabel( "Select the output video pixel type" );
	
	std::vector<std::string> pixelFormats = optionLoader.getPixelFormats();
	for( size_t i = 0; i < pixelFormats.size(); ++i )
	{
		videoCodecPixelFmt->appendOption( pixelFormats.at( i ) );
	}
	videoCodecPixelFmt->setParent( videoCustomGroupParam );
	
	// add video codec parameters
	avtranscoder::OptionLoader::OptionArray videoGroupOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	common::addOptionsToGroup( desc, videoCustomGroupParam, videoGroupOptions, common::kPrefixVideo );
	
	OFX::GroupParamDescriptor* videoDetailledGroup  = desc.defineGroupParam( kParamVideoDetailledGroup );
	videoDetailledGroup->setLabel( "Detailled" );
	videoDetailledGroup->setAsTab( );
	videoDetailledGroup->setParent( videoGroup );
	
	avtranscoder::OptionLoader::OptionMap videoDetailledGroupOptions = optionLoader.loadVideoCodecOptions(); 
	common::addOptionsToGroup( desc, videoDetailledGroup, videoDetailledGroupOptions, common::kPrefixVideo );
	
	/// AUDIO PARAMETERS
	// add main audio preset
	OFX::ChoiceParamDescriptor* audioMainPresetParam = desc.defineChoiceParam( kParamMainAudioPreset );
	audioMainPresetParam->setLabel( "Main Preset" );
	audioMainPresetParam->appendOption( "custom", "Customized configuration" );
	audioMainPresetParam->setParent( audioGroup );

	avtranscoder::Profile::ProfilesDesc audioPresets = presets.getAudioProfiles();
	for( avtranscoder::Profile::ProfilesDesc::iterator it = audioPresets.begin(); it != audioPresets.end(); ++it )
	{
		audioMainPresetParam->appendOption( 
			(*it).find( avtranscoder::Profile::avProfileIdentificator )->second, 
			(*it).find( avtranscoder::Profile::avProfileIdentificatorHuman )->second
		);
	}
	
	// add group to manage option when custom preset
	OFX::GroupParamDescriptor* audioCustomGroupParam = desc.defineGroupParam( kParamAudioCustomGroup );
	audioCustomGroupParam->setLabel( "Audio custom parameters" );
	audioCustomGroupParam->setHint( "Contains expert params, use to write audio streams when custom preset is specified." );
	audioCustomGroupParam->setParent( audioGroup );
	
	// add audio codec list
	int default_audio_codec = 0;
	std::string defaultAudioCodec( "pcm_s16le" );
	OFX::ChoiceParamDescriptor* audioCodecParam = desc.defineChoiceParam( kParamAudioCodec );
	for( std::vector<std::string>::const_iterator itShort = optionLoader.getAudioCodecsShortNames().begin(),
		itLong  = optionLoader.getAudioCodecsLongNames().begin(),
		itEnd = optionLoader.getAudioCodecsShortNames().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		audioCodecParam->appendOption( *itShort, *itLong );
		if( (*itShort) == defaultAudioCodec )
			default_audio_codec = audioCodecParam->getNOptions() - 1;
	}
	audioCodecParam->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	audioCodecParam->setDefault( default_audio_codec );
	audioCodecParam->setParent( audioCustomGroupParam );
	
	/// audio sample formats list
	int default_audio_sample_format = 0;
	std::string defaultAudioSampleFormat( "s16" );
	OFX::ChoiceParamDescriptor* audioSampleFmtParam = desc.defineChoiceParam( kParamAudioCodecSampleFmt );
	audioSampleFmtParam->setLabel( "Select the output audio sample type" );
	
	std::vector<std::string> sampleFormats = optionLoader.getSampleFormats();
	for( size_t i = 0; i < sampleFormats.size(); ++i )
	{
		audioSampleFmtParam->appendOption( sampleFormats.at( i ) );
		if( sampleFormats.at( i ) == defaultAudioSampleFormat )
			default_audio_sample_format = audioSampleFmtParam->getNOptions() - 1;
	}
	audioSampleFmtParam->setDefault( default_audio_sample_format );
	audioSampleFmtParam->setParent( audioCustomGroupParam );
	
	// add audio codec parameters
	avtranscoder::OptionLoader::OptionArray audioGroupOptions = optionLoader.loadCodecContextOptions( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM );
	common::addOptionsToGroup( desc, audioCustomGroupParam, audioGroupOptions, common::kPrefixAudio );
	
	// add audio details
	OFX::GroupParamDescriptor* audioDetailledGroup  = desc.defineGroupParam( kParamAudioDetailledGroup );
	audioDetailledGroup->setLabel( "Detailled" );
	audioDetailledGroup->setAsTab( );
	audioDetailledGroup->setParent( audioGroup );
	
	avtranscoder::OptionLoader::OptionMap audioDetailledGroupOptions = optionLoader.loadAudioCodecOptions();
	common::addOptionsToGroup( desc, audioDetailledGroup, audioDetailledGroupOptions, common::kPrefixAudio );
	
	// add number of audio stream
	OFX::IntParamDescriptor* audioNbStream = desc.defineIntParam( kParamAudioNbStream );
	audioNbStream->setLabel( "Number of audio stream" );
	audioNbStream->setRange( 0, maxNbAudioStream );
	audioNbStream->setDisplayRange( 0, maxNbAudioStream );
	audioNbStream->setDefault( 0 );
	audioNbStream->setParent( audioGroup );
	
	// add a list of audioSubGroup (managed dynamically by the plugin)
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		std::ostringstream audioSubGroupName( kParamAudioSubGroup, std::ios_base::in | std::ios_base::ate );
		audioSubGroupName << "_" << idAudioStream;
		OFX::GroupParamDescriptor* audioSubGroupParam = desc.defineGroupParam( audioSubGroupName.str() );
		std::ostringstream audioSubGroupLabel( "Stream ", std::ios_base::in | std::ios_base::ate );
		audioSubGroupLabel << idAudioStream;
		audioSubGroupParam->setLabel( audioSubGroupLabel.str() );
		audioSubGroupParam->setHint( "Indicate streams you want to add to the output file." );
		audioSubGroupParam->setParent( audioGroup );
		
		// add flag to audio silent
		std::ostringstream audioSilentName( kParamAudioSilent, std::ios_base::in | std::ios_base::ate );
		audioSilentName << "_" << idAudioStream;
		OFX::BooleanParamDescriptor* audioSilentParam = desc.defineBooleanParam( audioSilentName.str() );
		audioSilentParam->setLabel( "Silent stream" );
		audioSilentParam->setHint( "Write a silent audio stream" );
		audioSilentParam->setDefault( true );
		audioSilentParam->setParent( audioSubGroupParam );
		
		// add audio file path
		std::ostringstream audioFilePathName( kParamAudioFilePath, std::ios_base::in | std::ios_base::ate );
		audioFilePathName << "_" << idAudioStream;
		OFX::StringParamDescriptor* audioFilePathParam = desc.defineStringParam( audioFilePathName.str() );
		audioFilePathParam->setLabel( "Input file path" );
		audioFilePathParam->setHint( "Add a silent track if it is empty." );
		audioFilePathParam->setStringType( OFX::eStringTypeFilePath );
		audioFilePathParam->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
		audioFilePathParam->setParent( audioSubGroupParam );

		// add audio stream id
		std::ostringstream audioStreamIndexName( kParamAudioStreamIndex, std::ios_base::in | std::ios_base::ate );
		audioStreamIndexName << "_" << idAudioStream;
		OFX::IntParamDescriptor* audioStreamIndexParam = desc.defineIntParam( audioStreamIndexName.str() );
		audioStreamIndexParam->setLabel( "Input stream index" );
		audioStreamIndexParam->setRange( -1, INT_MAX );
		audioStreamIndexParam->setDisplayRange( -1, 16 );
		audioStreamIndexParam->setDefault( 0 );
		audioStreamIndexParam->setParent( audioSubGroupParam );

		// add flag to copy stream
		std::ostringstream audioCopyStreamName( kParamAudioCopyStream, std::ios_base::in | std::ios_base::ate );
		audioCopyStreamName << "_" << idAudioStream;
		OFX::BooleanParamDescriptor* audioCopyStreamParam = desc.defineBooleanParam( audioCopyStreamName.str() );
		audioCopyStreamParam->setLabel( "Only copy stream" );
		audioCopyStreamParam->setHint( "Only rewrap the audio stream whithout transcode it." );
		audioCopyStreamParam->setDefault( true );
		audioCopyStreamParam->setParent( audioSubGroupParam );
		
		// add audio codec preset
		std::ostringstream audioCodecPresetName( kParamAudioPreset, std::ios_base::in | std::ios_base::ate );
		audioCodecPresetName << "_" << idAudioStream;
		OFX::ChoiceParamDescriptor* audioCodecPresetParam = desc.defineChoiceParam( audioCodecPresetName.str() );
		audioCodecPresetParam->setLabel( "Output Preset" );
		audioCodecPresetParam->appendOption( "main preset", "Main preset" );
		audioCodecPresetParam->setParent( audioSubGroupParam );

		for( avtranscoder::Profile::ProfilesDesc::iterator it = audioPresets.begin(); it != audioPresets.end(); ++it )
		{
			audioCodecPresetParam->appendOption( 
				(*it).find( avtranscoder::Profile::avProfileIdentificator )->second, 
				(*it).find( avtranscoder::Profile::avProfileIdentificatorHuman )->second
			);
		}
	}
	
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
	
	/// ABOUT PARAMETERS
	avtranscoder::Libraries librairies( avtranscoder::getLibraries() );

	size_t libIndex = 0;
	for( avtranscoder::Libraries::iterator library = librairies.begin(); library != librairies.end(); ++library )
	{
		// add a group for the lib
		std::ostringstream libGroupName( kParamAboutLibName, std::ios_base::in | std::ios_base::ate );
		libGroupName << "_" << libIndex;
		OFX::GroupParamDescriptor* libGroupParam = desc.defineGroupParam( libGroupName.str() );
		libGroupParam->setLabel( (*library).getName() );
		libGroupParam->setParent( aboutGroup );
		
		// add licence
		std::ostringstream licenceName( kParamAboutLicence, std::ios_base::in | std::ios_base::ate );
		licenceName << "_" << libIndex;
		OFX::StringParamDescriptor* licenceParam = desc.defineStringParam( licenceName.str() );
		std::stringstream completeLicence;
		completeLicence << "Licence: " << (*library).getLicence();
		licenceParam->setDefault( completeLicence.str() );
		licenceParam->setStringType( OFX::eStringTypeLabel );
		licenceParam->setParent( libGroupParam );

		// add complete version (major.minor.micro)
		std::ostringstream versionName( kParamAboutVersion, std::ios_base::in | std::ios_base::ate );
		versionName << "_" << libIndex;
		OFX::StringParamDescriptor* versionParam = desc.defineStringParam( versionName.str() );
		std::stringstream completeVersion;
		completeVersion << "Version: " << (*library).getStringVersion();
		versionParam->setDefault( completeVersion.str() );
		versionParam->setStringType( OFX::eStringTypeLabel );
		versionParam->setParent( libGroupParam );
		
		++libIndex;
	}
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
