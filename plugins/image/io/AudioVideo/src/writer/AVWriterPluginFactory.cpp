#include "AVWriterPluginFactory.hpp"
#include "AVWriterPlugin.hpp"
#include "AVWriterDefinitions.hpp"

#include <common/util.hpp>

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/Library.hpp>
#include <AvTranscoder/ProfileLoader.hpp>
#include <AvTranscoder/option/CodecContext.hpp>
#include <AvTranscoder/option/FormatContext.hpp>

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
	avtranscoder::ProfileLoader presetLoader( true );
	
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
	mainPreset->appendOption( "custom: Customized configuration" );
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
	formatPreset->appendOption( "custom: Customized configuration" );
	formatPreset->setParent( formatGroup );
	
	avtranscoder::ProfileLoader::Profiles formatPresets = presetLoader.getFormatProfiles();
	for( avtranscoder::ProfileLoader::Profiles::iterator it = formatPresets.begin(); it != formatPresets.end(); ++it )
	{
		formatPreset->appendOption( 
			(*it).find( avtranscoder::constants::avProfileIdentificator )->second +
			": " +
			(*it).find( avtranscoder::constants::avProfileIdentificatorHuman )->second
		);
	}
	
	// add group to manage option when custom preset
	OFX::GroupParamDescriptor* formatCustomGroupParam = desc.defineGroupParam( kParamFormatCustomGroup );
	formatCustomGroupParam->setLabel( "Format custom parameters" );
	formatCustomGroupParam->setHint( "Contains expert params, use to write audio streams when custom preset is specified." );
	formatCustomGroupParam->setOpen( false );
	formatCustomGroupParam->setParent( formatGroup );
	
	/// FORMAT PARAMETERS
	int default_format = 0;
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	std::vector<std::string> formatsShortNames( avtranscoder::getFormatsShortNames() );
	std::vector<std::string> formatsLongNames( avtranscoder::getFormatsLongNames() );
	for( std::vector<std::string>::const_iterator itShort = formatsShortNames.begin(),
		itLong  = formatsLongNames.begin(),
		itEnd = formatsShortNames.end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		format->appendOption( *itShort + ": " + *itLong );
		if( (*itShort) == "mp4" )
			default_format = format->getNOptions() - 1;
	}
	format->setLabel( kParamFormatLabel );
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( default_format );
	format->setParent( formatCustomGroupParam );

	avtranscoder::FormatContext formatContext( AV_OPT_FLAG_ENCODING_PARAM );
	avtranscoder::OptionArray formatOptions = formatContext.getOptions();
	common::addOptionsToGroup( desc, formatCustomGroupParam, formatOptions, common::kPrefixFormat );
	
	OFX::GroupParamDescriptor* formatDetailledGroup = desc.defineGroupParam( kParamFormatDetailledGroup );
	formatDetailledGroup->setLabel( "Detailled" );
	formatDetailledGroup->setAsTab( );
	formatDetailledGroup->setParent( formatGroup );
	
	avtranscoder::OptionArrayMap formatDetailledGroupOptions = avtranscoder::getOutputFormatOptions();
	common::addOptionsToGroup( desc, formatDetailledGroup, formatDetailledGroupOptions, common::kPrefixFormat );
	
	/// VIDEO PARAMETERS
	OFX::ChoiceParamDescriptor* videoPresetParam = desc.defineChoiceParam( kParamVideoPreset );
	videoPresetParam->setLabel( "Video Preset" );
	videoPresetParam->appendOption( "custom: Customized configuration" );
	videoPresetParam->setParent( videoGroup );
	
	avtranscoder::ProfileLoader::Profiles videoPresets = presetLoader.getVideoProfiles();
	for( avtranscoder::ProfileLoader::Profiles::iterator it = videoPresets.begin(); it != videoPresets.end(); ++it )
	{
		videoPresetParam->appendOption( 
			(*it).find( avtranscoder::constants::avProfileIdentificator )->second +
			": " + 
			(*it).find( avtranscoder::constants::avProfileIdentificatorHuman )->second
		);
	}
	
	OFX::GroupParamDescriptor* videoCustomGroupParam = desc.defineGroupParam( kParamVideoCustomGroup );
	videoCustomGroupParam->setLabel( "Video custom parameters" );
	videoCustomGroupParam->setHint( "Contains expert params, use to write video streams when custom preset is specified." );
	videoCustomGroupParam->setOpen( false );
	videoCustomGroupParam->setParent( videoGroup );

	OFX::BooleanParamDescriptor* useCustomFps = desc.defineBooleanParam( kParamUseCustomFps );
	useCustomFps->setLabel( "Override Fps" );
	useCustomFps->setDefault( false );
	useCustomFps->setHint( "Override the input Fps (Frames Per Second) with a custom Fps value." );
	useCustomFps->setParent( videoCustomGroupParam );

	OFX::DoubleParamDescriptor* customFps = desc.defineDoubleParam( kParamCustomFps );
	customFps->setLabel( "Custom Fps" );
	customFps->setRange( 0, INT_MAX );
	customFps->setDisplayRange( 0, 100 );
	customFps->setDefault( 25.0 );
	customFps->setHint( "Choose a custom value to override the Fps (Frames Per Second)." );
	customFps->setParent( videoCustomGroupParam );

	int default_codec = 0;
	std::string defaultVideoCodec( "mpeg4" );
	OFX::ChoiceParamDescriptor* videoCodec = desc.defineChoiceParam( kParamVideoCodec );
	std::vector<std::string> videoCodecsShortNames( avtranscoder::getVideoCodecsShortNames() );
	std::vector<std::string> videoCodecsLongNames( avtranscoder::getVideoCodecsLongNames() );
	for( std::vector<std::string>::const_iterator itShort = videoCodecsShortNames.begin(),
		itLong  = videoCodecsLongNames.begin(),
		itEnd = videoCodecsShortNames.end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		videoCodec->appendOption( *itShort + ": " + *itLong );
		if( (*itShort) == defaultVideoCodec )
			default_codec = videoCodec->getNOptions() - 1;
	}
	videoCodec->setLabel( kParamVideoCodecLabel );
	videoCodec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	videoCodec->setDefault( default_codec );
	videoCodec->setParent( videoCustomGroupParam );
	
	OFX::ChoiceParamDescriptor* videoCodecPixelFmt = desc.defineChoiceParam( kParamVideoCodecPixelFmt );
	videoCodecPixelFmt->setLabel( "Select the output video pixel type" );
	std::vector<std::string> pixelFormats = avtranscoder::getPixelFormats();
	for( size_t i = 0; i < pixelFormats.size(); ++i )
	{
		videoCodecPixelFmt->appendOption( pixelFormats.at( i ) );
	}
	videoCodecPixelFmt->setParent( videoCustomGroupParam );
	
	avtranscoder::CodecContext videoCodecContext( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	avtranscoder::OptionArray videoOptions = videoCodecContext.getOptions();
	common::addOptionsToGroup( desc, videoCustomGroupParam, videoOptions, common::kPrefixVideo );

	OFX::GroupParamDescriptor* videoDetailledGroup  = desc.defineGroupParam( kParamVideoDetailledGroup );
	videoDetailledGroup->setLabel( "Detailled" );
	videoDetailledGroup->setAsTab( );
	videoDetailledGroup->setParent( videoGroup );
	
	avtranscoder::OptionArrayMap videoDetailledGroupOptions = avtranscoder::getVideoCodecOptions(); 
	common::addOptionsToGroup( desc, videoDetailledGroup, videoDetailledGroupOptions, common::kPrefixVideo );
	
	/// AUDIO PARAMETERS
	OFX::ChoiceParamDescriptor* audioMainPresetParam = desc.defineChoiceParam( kParamAudioMainPreset );
	audioMainPresetParam->setLabel( "Main Preset" );
	audioMainPresetParam->appendOption( "custom: Customized configuration" );
	audioMainPresetParam->setParent( audioGroup );

	avtranscoder::ProfileLoader::Profiles audioPresets = presetLoader.getAudioProfiles();
	for( avtranscoder::ProfileLoader::Profiles::iterator it = audioPresets.begin(); it != audioPresets.end(); ++it )
	{
		audioMainPresetParam->appendOption( 
			(*it).find( avtranscoder::constants::avProfileIdentificator )->second +
			": " +
			(*it).find( avtranscoder::constants::avProfileIdentificatorHuman )->second
		);
	}
	
	OFX::GroupParamDescriptor* audioCustomGroupParam = desc.defineGroupParam( kParamAudioCustomGroup );
	audioCustomGroupParam->setLabel( "Audio custom parameters" );
	audioCustomGroupParam->setHint( "Contains expert params, use to write audio streams when custom preset is specified." );
	audioCustomGroupParam->setOpen( false );
	audioCustomGroupParam->setParent( audioGroup );
	
	int default_audio_codec = 0;
	std::string defaultAudioCodec( "pcm_s16le" );
	OFX::ChoiceParamDescriptor* audioCodecParam = desc.defineChoiceParam( kParamAudioCodec );
	std::vector<std::string> audioCodecsShortNames( avtranscoder::getAudioCodecsShortNames() );
	std::vector<std::string> audioCodecsLongNames( avtranscoder::getAudioCodecsLongNames() );
	for( std::vector<std::string>::const_iterator itShort = audioCodecsShortNames.begin(),
		itLong  = audioCodecsLongNames.begin(),
		itEnd = audioCodecsShortNames.end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		audioCodecParam->appendOption( *itShort + ": " + *itLong );
		if( (*itShort) == defaultAudioCodec )
			default_audio_codec = audioCodecParam->getNOptions() - 1;
	}
	audioCodecParam->setLabel( kParamAudioCodecLabel );
	audioCodecParam->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	audioCodecParam->setDefault( default_audio_codec );
	audioCodecParam->setParent( audioCustomGroupParam );
	
	OFX::ChoiceParamDescriptor* audioSampleFmtParam = desc.defineChoiceParam( kParamAudioCodecSampleFmt );
	audioSampleFmtParam->setLabel( "Select the output audio sample type" );
	std::vector<std::string> sampleFormats = avtranscoder::getSampleFormats();
	for( size_t i = 0; i < sampleFormats.size(); ++i )
	{
		audioSampleFmtParam->appendOption( sampleFormats.at( i ) );
	}
	audioSampleFmtParam->setParent( audioCustomGroupParam );
	
	avtranscoder::CodecContext audioCodecContext( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM );
	avtranscoder::OptionArray audioOptions = audioCodecContext.getOptions();
	common::addOptionsToGroup( desc, audioCustomGroupParam, audioOptions, common::kPrefixAudio );
	
	OFX::GroupParamDescriptor* audioDetailledGroup  = desc.defineGroupParam( kParamAudioDetailledGroup );
	audioDetailledGroup->setLabel( "Detailled" );
	audioDetailledGroup->setAsTab( );
	audioDetailledGroup->setParent( audioGroup );
	
	avtranscoder::OptionArrayMap audioDetailledGroupOptions = avtranscoder::getAudioCodecOptions();
	common::addOptionsToGroup( desc, audioDetailledGroup, audioDetailledGroupOptions, common::kPrefixAudio );
	
	OFX::IntParamDescriptor* audioNbInputs = desc.defineIntParam( kParamAudioNbInputs );
	audioNbInputs->setLabel( "Number Of Audio Inputs" );
	audioNbInputs->setHint( "An 'Input' allow you to create one of several audio streams in your output file. It could be by rewrap or transcode one or several audio streams of an input file, or by generate a dummy audio stream." );
	audioNbInputs->setRange( 0, maxNbAudioInput );
	audioNbInputs->setDisplayRange( 0, maxNbAudioInput );
	audioNbInputs->setDefault( 1 );
	audioNbInputs->setParent( audioGroup );
	
	// add a list of audioSubGroup (managed dynamically by the plugin)
	for( size_t indexAudioInput = 0; indexAudioInput < maxNbAudioInput; ++indexAudioInput )
	{
		std::ostringstream audioSubGroupName( kParamAudioSubGroup, std::ios_base::in | std::ios_base::ate );
		audioSubGroupName << "_" << indexAudioInput;
		OFX::GroupParamDescriptor* audioSubGroupParam = desc.defineGroupParam( audioSubGroupName.str() );
		std::ostringstream audioSubGroupLabel( "Input ", std::ios_base::in | std::ios_base::ate );
		audioSubGroupLabel << indexAudioInput;
		audioSubGroupParam->setLabel( audioSubGroupLabel.str() );
		audioSubGroupParam->setParent( audioGroup );
		
		// add flag to audio silent
		std::ostringstream audioSilentName( kParamAudioSilent, std::ios_base::in | std::ios_base::ate );
		audioSilentName << "_" << indexAudioInput;
		OFX::BooleanParamDescriptor* audioSilentParam = desc.defineBooleanParam( audioSilentName.str() );
		audioSilentParam->setLabel( "Silent stream" );
		audioSilentParam->setHint( "Write a silent audio stream" );
		audioSilentParam->setDefault( false );
		audioSilentParam->setParent( audioSubGroupParam );
		
		// add audio file path
		std::ostringstream audioFilePathName( kParamAudioFilePath, std::ios_base::in | std::ios_base::ate );
		audioFilePathName << "_" << indexAudioInput;
		OFX::StringParamDescriptor* audioFilePathParam = desc.defineStringParam( audioFilePathName.str() );
		audioFilePathParam->setLabel( "Input File Path" );
		audioFilePathParam->setStringType( OFX::eStringTypeFilePath );
		audioFilePathParam->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
		audioFilePathParam->setParent( audioSubGroupParam );

		// display number of audio streams
		std::ostringstream audioFileInfoName( kParamAudioFileInfo, std::ios_base::in | std::ios_base::ate );
		audioFileInfoName << "_" << indexAudioInput;
		OFX::StringParamDescriptor* audioFileInfoParam = desc.defineStringParam( audioFileInfoName.str() );
		audioFileInfoParam->setStringType( OFX::eStringTypeMultiLine );
		audioFileInfoParam->setLabel( "File info" );
		audioFileInfoParam->setHint( "Audio information about input file indicated." );
		audioFileInfoParam->setEnabled( false );
		audioFileInfoParam->setParent( audioSubGroupParam );

		// add flag to select a stream
		std::ostringstream audioSelectStreamName( kParamAudioSelectStream, std::ios_base::in | std::ios_base::ate );
		audioSelectStreamName << "_" << indexAudioInput;
		OFX::BooleanParamDescriptor* audioSelectStreamParam = desc.defineBooleanParam( audioSelectStreamName.str() );
		audioSelectStreamParam->setLabel( "Select One Stream" );
		audioSelectStreamParam->setHint( "By default select all streams of the input file." );
		audioSelectStreamParam->setDefault( false );
		audioSelectStreamParam->setParent( audioSubGroupParam );
		
		// add audio stream index
		std::ostringstream audioStreamIndexName( kParamAudioStreamIndex, std::ios_base::in | std::ios_base::ate );
		audioStreamIndexName << "_" << indexAudioInput;
		OFX::IntParamDescriptor* audioStreamIndexParam = desc.defineIntParam( audioStreamIndexName.str() );
		audioStreamIndexParam->setLabel( "Input Stream Index" );
		audioStreamIndexParam->setHint( "Select a specific stream of the input file." );
		audioStreamIndexParam->setRange( 0, INT_MAX );
		audioStreamIndexParam->setDisplayRange( 0, 16 );
		audioStreamIndexParam->setDefault( 0 );
		audioStreamIndexParam->setParent( audioSubGroupParam );
		
		// add audio codec preset
		std::ostringstream audioPresetName( kParamAudioPreset, std::ios_base::in | std::ios_base::ate );
		audioPresetName << "_" << indexAudioInput;
		OFX::ChoiceParamDescriptor* audioPresetParam = desc.defineChoiceParam( audioPresetName.str() );
		audioPresetParam->setLabel( "Output Encoding" );
		audioPresetParam->setHint( "Choose a preset to easily get a configuration for the audio 'Output'." );
		audioPresetParam->appendOption( "main preset: Refers to the list of Main Presets" );
		audioPresetParam->appendOption( "rewrap: Copy audio data (no transcode)" );
		audioPresetParam->setParent( audioSubGroupParam );

		for( avtranscoder::ProfileLoader::Profiles::iterator it = audioPresets.begin(); it != audioPresets.end(); ++it )
		{
			audioPresetParam->appendOption( 
				(*it).find( avtranscoder::constants::avProfileIdentificator )->second + 
				": " +
				(*it).find( avtranscoder::constants::avProfileIdentificatorHuman )->second
			);
		}
		
		// add audio channel index
		std::ostringstream audioOffsetName( kParamAudioOffset, std::ios_base::in | std::ios_base::ate );
		audioOffsetName << "_" << indexAudioInput;
		OFX::IntParamDescriptor* audioOffsetParam = desc.defineIntParam( audioOffsetName.str() );
		audioOffsetParam->setLabel( "Offset" );
		audioOffsetParam->setHint( "Add an offset (in milliseconds) at the beginning of the stream. By default 0." );
		audioOffsetParam->setRange( 0, INT_MAX );
		audioOffsetParam->setDisplayRange( 0, 10000 );
		audioOffsetParam->setDefault( 0 );
		audioOffsetParam->setParent( audioSubGroupParam );
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
	}

	/// ABOUT PARAMETERS
	avtranscoder::Libraries librairies( avtranscoder::getLibraries() );

	size_t libIndex = 0;
	for( avtranscoder::Libraries::iterator library = librairies.begin(); library != librairies.end(); ++library )
	{
		// add a group for the lib
		std::ostringstream libGroupName( kParamAboutLibName, std::ios_base::in | std::ios_base::ate );
		libGroupName << "_" << (*library).getName();
		OFX::GroupParamDescriptor* libGroupParam = desc.defineGroupParam( libGroupName.str() );
		libGroupParam->setLabel( (*library).getName() );
		libGroupParam->setParent( aboutGroup );
		
		// add license
		std::ostringstream licenseName( kParamAboutLicense, std::ios_base::in | std::ios_base::ate );
		licenseName << "_" << (*library).getName();
		OFX::StringParamDescriptor* licenseParam = desc.defineStringParam( licenseName.str() );
		std::stringstream completeLicense;
		completeLicense << (*library).getLicense();
		licenseParam->setLabel( kParamAboutLicenseLabel );
		licenseParam->setDefault( completeLicense.str() );
		licenseParam->setStringType( OFX::eStringTypeLabel );
		licenseParam->setParent( libGroupParam );

		// add complete version (major.minor.micro)
		std::ostringstream versionName( kParamAboutVersion, std::ios_base::in | std::ios_base::ate );
		versionName << "_" << (*library).getName();
		OFX::StringParamDescriptor* versionParam = desc.defineStringParam( versionName.str() );
		std::stringstream completeVersion;
		completeVersion << (*library).getStringVersion();
		versionParam->setLabel( kParamAboutVersionLabel );
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
