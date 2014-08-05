#include "AVWriterPlugin.hpp"
#include "AVWriterProcess.hpp"
#include "AVWriterDefinitions.hpp"

#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>
#include <AvTranscoder/ProgressListener.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <cctype>
#include <sstream>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

AVWriterPlugin::AVWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
	, _paramAudioSubGroup()
	, _paramAudioSilent()
	, _paramAudioFilePath()
	, _paramAudioStreamIndex()
	, _paramAudioCopyStream()
	, _paramAudioPreset()
	, _paramFormatCustom()
	, _paramVideoCustom()
	, _paramAudioCustom()
	, _paramFormatDetailCustom()
	, _paramVideoDetailCustom()
	, _paramAudioDetailCustom()
	, _paramMetadatas()
	, _outputFile( NULL )
	, _transcoder( NULL )
	, _presets( true ) 
	, _lastOutputFilePath()
	, _initVideo( false )
	, _initWrap( false )
{
	// We want to render a sequence
	setSequentialRender( true );

	// format
	_paramFormatCustomGroup = fetchGroupParam( kParamFormatCustomGroup );
	_paramFormat = fetchChoiceParam( kParamFormat );
	
	// video
	_paramVideoCustomGroup = fetchGroupParam( kParamVideoCustomGroup );
	_paramVideoCodec = fetchChoiceParam( kParamVideoCodec );
	
	// audio
	_paramAudioCustomGroup = fetchGroupParam( kParamAudioCustomGroup );
	_paramAudioCodec = fetchChoiceParam( kParamAudioCodec );
	
	_paramAudioNbStream = fetchIntParam( kParamAudioNbStream );
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		std::ostringstream audioSubGroupName( kParamAudioSubGroup, std::ios_base::in | std::ios_base::ate );
		audioSubGroupName << "_" << idAudioStream;
		_paramAudioSubGroup.push_back( fetchGroupParam( audioSubGroupName.str() ) );
		_paramAudioSubGroup.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioSilentName( kParamAudioSilent, std::ios_base::in | std::ios_base::ate );
		audioSilentName << "_" << idAudioStream;
		_paramAudioSilent.push_back( fetchBooleanParam( audioSilentName.str() ) );
		_paramAudioSilent.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioFilePathName( kParamAudioFilePath, std::ios_base::in | std::ios_base::ate );
		audioFilePathName << "_" << idAudioStream;
		_paramAudioFilePath.push_back( fetchStringParam( audioFilePathName.str() ) );
		_paramAudioFilePath.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioStreamIndexName( kParamAudioStreamIndex, std::ios_base::in | std::ios_base::ate );
		audioStreamIndexName << "_" << idAudioStream;
		_paramAudioStreamIndex.push_back( fetchIntParam( audioStreamIndexName.str() ) );
		_paramAudioStreamIndex.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioCopyStreamName( kParamAudioCopyStream, std::ios_base::in | std::ios_base::ate );
		audioCopyStreamName << "_" << idAudioStream;
		_paramAudioCopyStream.push_back( fetchBooleanParam( audioCopyStreamName.str() ) );
		_paramAudioCopyStream.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioCodecPresetName( kParamAudioPreset, std::ios_base::in | std::ios_base::ate );
		audioCodecPresetName << "_" << idAudioStream;
		_paramAudioPreset.push_back( fetchChoiceParam( audioCodecPresetName.str() ) );
		_paramAudioPreset.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioAllChannelsName( kParamAudioAllChannels, std::ios_base::in | std::ios_base::ate );
		audioAllChannelsName << "_" << idAudioStream;
		_paramAudioAllChannels.push_back( fetchBooleanParam( audioAllChannelsName.str() ) );
		_paramAudioAllChannels.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioChannelIndexName( kParamAudioChannelIndex, std::ios_base::in | std::ios_base::ate );
		audioChannelIndexName << "_" << idAudioStream;
		_paramAudioChannelIndex.push_back( fetchIntParam( audioChannelIndexName.str() ) );
		_paramAudioChannelIndex.back()->setIsSecretAndDisabled( false );
	}
	updateAudioParams();
	
	// our custom params
	_paramUseCustomFps = fetchBooleanParam( kParamUseCustomFps );
	_paramCustomFps = fetchDoubleParam( kParamCustomFps );
	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	_paramAudioSampleFormat = fetchChoiceParam( kParamAudioCodecSampleFmt );
	
	// all custom params
	avtranscoder::OptionLoader::OptionArray optionsFormatArray = _optionLoader.loadFormatContextOptions( AV_OPT_FLAG_ENCODING_PARAM );
	_paramFormatCustom.fetchCustomParams( *this, optionsFormatArray, common::kPrefixFormat );
	
	avtranscoder::OptionLoader::OptionArray optionsVideoArray = _optionLoader.loadCodecContextOptions( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	_paramVideoCustom.fetchCustomParams( *this, optionsVideoArray, common::kPrefixVideo );
	
	avtranscoder::OptionLoader::OptionArray optionsAudioArray = _optionLoader.loadCodecContextOptions( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM );
	_paramAudioCustom.fetchCustomParams( *this, optionsAudioArray, common::kPrefixAudio );
	
	avtranscoder::OptionLoader::OptionMap optionsFormatDetailMap = _optionLoader.loadOutputFormatOptions();
	_paramFormatDetailCustom.fetchCustomParams( *this, optionsFormatDetailMap, common::kPrefixFormat );
	const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
	common::disableOFXParamsForFormatOrCodec( *this, optionsFormatDetailMap, formatName, common::kPrefixFormat );
	
	avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
	_paramVideoDetailCustom.fetchCustomParams( *this, optionsVideoCodecMap, common::kPrefixVideo );
	const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at( _paramVideoCodec->getValue() );
	common::disableOFXParamsForFormatOrCodec( *this, optionsVideoCodecMap, videoCodecName, common::kPrefixVideo );
	
	avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
	_paramAudioDetailCustom.fetchCustomParams( *this, optionsAudioCodecMap, common::kPrefixAudio );
	const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at( _paramAudioCodec->getValue() );
	common::disableOFXParamsForFormatOrCodec( *this, optionsAudioCodecMap, audioCodecName, common::kPrefixAudio );
	
	updatePixelFormats( videoCodecName );
	updateSampleFormats( audioCodecName );
	
	// preset
	_paramMainPreset = fetchChoiceParam( kParamMainPreset );
	_paramFormatPreset = fetchChoiceParam( kParamFormatPreset );
	_paramMainVideoPreset = fetchChoiceParam( kParamMainVideoPreset );
	_paramMainAudioPreset = fetchChoiceParam( kParamMainAudioPreset );
	
	// metadata
	_paramMetadatas.push_back( fetchStringParam( kParamMetaAlbum           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaAlbumArtist     ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaArtist          ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaComment         ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaComposer        ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaCopyright       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaCreationTime    ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaDate            ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaDisc            ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaEncoder         ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaEncodedBy       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaFilename        ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaGenre           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaLanguage        ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaPerformer       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaPublisher       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaServiceName     ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaServiceProvider ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaTitle           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaTrack           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaVariantBitrate  ) );

	updateVisibleTools();
	
	_formatProfile[ avtranscoder::Profile::avProfileIdentificator ] = "customFormatPreset";
	_formatProfile[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Custom format preset";
	_formatProfile[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeFormat;
	
	_videoProfile[ avtranscoder::Profile::avProfileIdentificator ] = "customVideoPreset";
	_videoProfile[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Custom video preset";
	_videoProfile[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeVideo;
	
	_audioProfile[ avtranscoder::Profile::avProfileIdentificator ] = "customAudioPreset";
	_audioProfile[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Custom audio preset";
	_audioProfile[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeAudio;
}

void AVWriterPlugin::updateVisibleTools()
{
	OFX::InstanceChangedArgs args( this->timeLineGetTime() );
	changedParam( args, kParamUseCustomFps );
}

AVProcessParams AVWriterPlugin::getProcessParams()
{
	AVProcessParams params;

	params._outputFilePath = _paramFilepath->getValue();
	
	params._format = _paramFormat->getValue();
	params._formatName = _optionLoader.getFormatsShortNames().at( params._format );
	
	params._videoCodec = _paramVideoCodec->getValue();
	params._videoCodecName = _optionLoader.getVideoCodecsShortNames().at( params._videoCodec );
	
	params._audioCodec = _paramAudioCodec->getValue();
	params._audioCodecName = _optionLoader.getAudioCodecsShortNames().at( params._audioCodec );

	std::vector<std::string> supportedPixelFormats( avtranscoder::OptionLoader::getPixelFormats( params._videoCodecName ) );
	params._videoPixelFormatName = supportedPixelFormats.at( _paramVideoPixelFormat->getValue() );

	std::vector<std::string> supportedSampleFormats( avtranscoder::OptionLoader::getSampleFormats( params._audioCodecName ) );
	params._audioSampleFormatName = supportedSampleFormats.at( _paramAudioSampleFormat->getValue() );

	BOOST_FOREACH( OFX::StringParam* parameter, _paramMetadatas )
	{
		if( parameter->getValue().size() > 0 )
		{
			std::string ffmpegKey = parameter->getName();
			ffmpegKey.erase( 0, 5 );
			params._metadatas[ ffmpegKey ] = parameter->getValue();
		}
	}
	return params;
}

/**
 * @brief Update the list of pixel format supported depending on the video codec.
 * Warning: the function does not update the list correctly in Nuke.
 * @param videoCodecName
 */
void AVWriterPlugin::updatePixelFormats( const std::string& videoCodecName )
{
	_paramVideoPixelFormat->resetOptions();
	
	std::vector<std::string> pixelFormats( avtranscoder::OptionLoader::getPixelFormats( videoCodecName ) );
	// get all pixel formats if the list is empty with the video codec indicated
	if( pixelFormats.empty() )
	{
		pixelFormats = avtranscoder::OptionLoader::getPixelFormats();
	}
	
	for( std::vector<std::string>::iterator it = pixelFormats.begin(); it != pixelFormats.end(); ++it )
	{
		_paramVideoPixelFormat->appendOption( *it );
	}
}

/**
 * @brief Update the list of sample format supported depending on the audio codec.
 * Warning: the function does not update the list correctly in Nuke.
 * @param audioCodecName
 */
void AVWriterPlugin::updateSampleFormats( const std::string& audioCodecName )
{
	_paramAudioSampleFormat->resetOptions();
	
	std::vector<std::string> sampleFormats( _optionLoader.getSampleFormats( audioCodecName ) );
	// get all sample formats if the list is empty with the audio codec indicated
	if( sampleFormats.empty() )
	{
		sampleFormats = _optionLoader.getSampleFormats();
	}
	
	for( std::vector<std::string>::iterator it = sampleFormats.begin(); it != sampleFormats.end(); ++it )
	{
		_paramAudioSampleFormat->appendOption( *it );
	}
}

void AVWriterPlugin::updateAudioParams()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		bool flag = idAudioStream < (size_t)_paramAudioNbStream->getValue();
		_paramAudioSubGroup.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioSilent.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioFilePath.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioStreamIndex.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioCopyStream.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioAllChannels.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
		_paramAudioChannelIndex.at( idAudioStream )->setIsSecretAndDisabled( ! flag );
	}
	updateAudioSilent();
}

void AVWriterPlugin::updateAudioSilent()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		if( _paramAudioSubGroup.at( idAudioStream )->getIsEnable() &&
			! _paramAudioSubGroup.at( idAudioStream )->getIsSecret() )
		{
			// if silent stream
			bool flag = _paramAudioSilent.at( idAudioStream )->getValue();
			_paramAudioFilePath.at( idAudioStream )->setIsSecretAndDisabled( flag );
			_paramAudioStreamIndex.at( idAudioStream )->setIsSecretAndDisabled( flag );
			_paramAudioCopyStream.at( idAudioStream )->setIsSecretAndDisabled( flag );
			_paramAudioAllChannels.at( idAudioStream )->setIsSecretAndDisabled( flag );
			_paramAudioChannelIndex.at( idAudioStream )->setIsSecretAndDisabled( flag );
		}
	}
	updateAudioCopyStream();
}

void AVWriterPlugin::updateAudioCopyStream()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		if( _paramAudioSubGroup.at( idAudioStream )->getIsEnable() &&
			! _paramAudioSubGroup.at( idAudioStream )->getIsSecret() &&
			! _paramAudioSilent.at( idAudioStream )->getValue() )
		{
			// if copy stream
			bool flag = _paramAudioCopyStream.at( idAudioStream )->getValue();
			_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( flag );
			_paramAudioAllChannels.at( idAudioStream )->setIsSecretAndDisabled( flag );
			_paramAudioChannelIndex.at( idAudioStream )->setIsSecretAndDisabled( flag );
		}
	}
	updateAllChannels();
}

void AVWriterPlugin::updateAllChannels()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		if( _paramAudioSubGroup.at( idAudioStream )->getIsEnable() &&
			! _paramAudioSubGroup.at( idAudioStream )->getIsSecret() &&
			! _paramAudioSilent.at( idAudioStream )->getValue() &&
			! _paramAudioCopyStream.at( idAudioStream )->getValue() )
		{
			// if transcode all channels of the stream
			bool flag = _paramAudioAllChannels.at( idAudioStream )->getValue();
			_paramAudioChannelIndex.at( idAudioStream )->setIsSecretAndDisabled( flag );
		}
	}
}

void AVWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
	
	if( paramName == kTuttlePluginFilename )
	{
		const std::string& extension = avtranscoder::getFormat( _paramFilepath->getValue() );
		std::vector<std::string>::iterator itFormat = std::find( _optionLoader.getFormatsShortNames().begin(), _optionLoader.getFormatsShortNames().end(), extension );
		if( itFormat != _optionLoader.getFormatsShortNames().end() )
		{
			size_t indexFormat = itFormat - _optionLoader.getFormatsShortNames().begin();
			_paramFormat->setValue( indexFormat );
		}
		cleanVideoAndAudio();
	}
	else if( paramName == kParamFormat )
	{
		avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
		const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
		common::disableOFXParamsForFormatOrCodec( *this, optionsFormatMap, formatName, common::kPrefixFormat );
	}
	else if( paramName == kParamVideoCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
		const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at( _paramVideoCodec->getValue() );
		common::disableOFXParamsForFormatOrCodec( *this, optionsVideoCodecMap, videoCodecName, common::kPrefixVideo );
		
		updatePixelFormats( videoCodecName );
	}
	else if( paramName == kParamAudioCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
		const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
		common::disableOFXParamsForFormatOrCodec( *this, optionsAudioCodecMap, audioCodecName, common::kPrefixAudio );
		
		updateSampleFormats( audioCodecName );
	}
	else if( paramName == kParamMainPreset )
	{
		if( _paramMainPreset->getValue() == 0 )
			return;
	}
	else if( paramName == kParamFormatPreset )
	{
		// if custom format preset
		if( _paramFormatPreset->getValue() == 0 )
		{
			int defaultFormatIndex;
			_paramFormat->getDefault( defaultFormatIndex );
			_paramFormat->setValue( defaultFormatIndex );
			
			_paramFormatCustomGroup->setIsSecretAndDisabled( false );
		}
		else
		{
			// hack to have nothing display in detailled group
			int formatWithNoOptionDetailsIndex = 0;
			_paramFormat->setValue( formatWithNoOptionDetailsIndex );
			
			_paramFormatCustomGroup->setIsSecretAndDisabled( true );
		}
	}
	else if( paramName == kParamMainVideoPreset )
	{
		// if custom video preset
		if( _paramMainVideoPreset->getValue() == 0 )
		{	
			int defaultVideoCodecIndex;
			_paramVideoCodec->getDefault( defaultVideoCodecIndex );
			_paramVideoCodec->setValue( defaultVideoCodecIndex );
			
			_paramVideoCustomGroup->setIsSecretAndDisabled( false );
		}
		else
		{
			// hack to have nothing display in detailled group
			int videoCodecWithNoOptionDetailsIndex = 0;
			_paramVideoCodec->setValue( videoCodecWithNoOptionDetailsIndex );
			
			_paramVideoCustomGroup->setIsSecretAndDisabled( true );
		}
	}
	else if( paramName == kParamMainAudioPreset )
	{
		// if custom audio preset
		if( _paramMainAudioPreset->getValue() == 0 )
		{
			_paramAudioCustomGroup->setIsSecretAndDisabled( false );
		}
		else
		{
			// hack to have nothing display in detailled group
			int defaultAudioCodecIndex;
			_paramAudioCodec->getDefault( defaultAudioCodecIndex );
			_paramAudioCodec->setValue( defaultAudioCodecIndex );
			
			_paramAudioCustomGroup->setIsSecretAndDisabled( true );
		}
	}
	else if( paramName == kParamUseCustomFps )
	{
		_paramCustomFps->setIsSecretAndDisabled( ! _paramUseCustomFps->getValue() );
	}
	else if( paramName == kParamCustomFps )
	{
		if( ! _paramUseCustomFps->getValue() )
		{
			_paramUseCustomFps->setValue(true);
		}
	}
	else if( paramName == kParamAudioNbStream )
	{
		updateAudioParams();
	}
	else if( paramName.find( kParamAudioCopyStream ) != std::string::npos )
	{
		updateAudioCopyStream();
	}
	else if( paramName.find( kParamAudioSilent ) != std::string::npos )
	{
		updateAudioSilent();
	}
	else if( paramName.find( kParamAudioAllChannels ) != std::string::npos )
	{
		updateAllChannels();
	}
	else if( paramName.find( kParamAudioFilePath ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioFilePath.size() + 1; // add "_"
		const size_t audioStreamIndex = boost::lexical_cast<size_t>( paramName.substr( indexPos, indexPos ) );
		_paramAudioSilent.at( audioStreamIndex )->setValue( false );
	}
	else if( paramName.find( kParamAudioPreset ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioPreset.size() + 1; // add "_"
		const size_t audioStreamIndex = boost::lexical_cast<size_t>( paramName.substr( indexPos, indexPos ) );
		_paramAudioCopyStream.at( audioStreamIndex )->setValue( false );
	}
	else if( paramName.find( kParamAudioChannelIndex ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioChannelIndex.size() + 1; // add "_"
		const size_t audioStreamIndex = boost::lexical_cast<size_t>( paramName.substr( indexPos, indexPos ) );
		_paramAudioAllChannels.at( audioStreamIndex )->setValue( false );
	}
}

void AVWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// Need to be computed at each frame
	clipPreferences.setOutputFrameVarying( true );
}

void AVWriterPlugin::initOutput()
{
	AVProcessParams params = getProcessParams();
	
	// create output file
	try
	{
		_outputFile.reset( new avtranscoder::OutputFile( params._outputFilePath ) );
		
		size_t mainPresetIndex = _paramFormatPreset->getValue();
		avtranscoder::Profile::ProfileDesc profile;

		// custom format preset
		if( mainPresetIndex == 0 )
		{
			updateFormatProfile();
			profile = _formatProfile;
		}
		// existing format preset
		else
		{
			// at( mainPresetIndex - 1 ): subtract the index of the custom preset
			profile = _presets.getFormatProfiles().at( mainPresetIndex - 1 );
		}
		
		_outputFile->setProfile( profile );
		
		_transcoder.reset( new avtranscoder::Transcoder( *_outputFile ) );
	}
	catch( std::exception& e )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open output file: " + e.what() );
	}
}

void AVWriterPlugin::ensureVideoIsInit( const OFX::RenderArguments& args )
{
	AVProcessParams params = getProcessParams();
	
	// ouput file path already set
	if( _lastOutputFilePath != "" && _lastOutputFilePath == params._outputFilePath )
		return;
	
	if( params._outputFilePath == "" ) // no output file indicated
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "no output file indicated"
		    << exception::filename( params._outputFilePath ) );
	}
	
	if( ! isOutputInit() )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "the output is not init. Can't process." );
	}
	
	_lastOutputFilePath = params._outputFilePath;
	
	// create video stream
	try
	{
		size_t mainPresetIndex = _paramMainVideoPreset->getValue();
		avtranscoder::Profile::ProfileDesc profile;
		
		// custom video preset
		if( mainPresetIndex == 0 )
		{
			updateVideoProfile();
			profile = _videoProfile;
		}
		// existing video preset
		else
		{
			// at( mainPresetIndex - 1 ): subtract the index of the custom preset
			profile = _presets.getVideoProfiles().at( mainPresetIndex - 1 );
		}
		
		const OfxRectI bounds = _clipSrc->getPixelRod( args.time, args.renderScale );
		int width = bounds.x2 - bounds.x1;
		int height = bounds.y2 - bounds.y1;

		// describe the input of transcode
		avtranscoder::VideoFrameDesc imageDesc;
		imageDesc.setWidth( width );
		imageDesc.setHeight( height );
		imageDesc.setDar( width, height );

		avtranscoder::Pixel inputPixel;
		inputPixel.setComponents( 3 );
		inputPixel.setColorComponents( avtranscoder::eComponentRgb );
		inputPixel.setBitsPerPixel( 24 );
		inputPixel.setPlanar( false );

		imageDesc.setPixel( inputPixel );

		avtranscoder::VideoDesc inputVideoDesc = avtranscoder::VideoDesc( profile[ avtranscoder::Profile::avProfileCodec ] );
		inputVideoDesc.setImageParameters( imageDesc );

		_dummyVideo.setVideoDesc( inputVideoDesc );

		// the streamTranscoder is deleted by avTranscoder
		avtranscoder::StreamTranscoder* stream = new avtranscoder::StreamTranscoder( _dummyVideo, *_outputFile, profile );
		_transcoder->add( *stream );
	}	
	catch( std::exception& e )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to create video stream: " + e.what() );
	}
	_initVideo = true;
}

void AVWriterPlugin::initAudio()
{
	// no audio stream specified
	if( ! _paramAudioNbStream->getValue() )
		return;

	if( ! isOutputInit() )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "the output is not init. Can't process." );
	}
	
	// create audio streams
	try
	{
		size_t mainPresetIndex = _paramMainAudioPreset->getValue();
		avtranscoder::Profile::ProfileDesc profile;
		std::string presetName( "" );
		
		for( int i = 0; i < _paramAudioNbStream->getValue(); ++i )
		{
			std::string inputFileName( _paramAudioFilePath.at( i )->getValue() );
			size_t presetIndex = _paramAudioPreset.at( i )->getValue();
			
			// custom audio preset
			if( presetIndex == 0 && mainPresetIndex == 0 )
			{
				updateAudioProfile();
				profile = _audioProfile;
				presetName = "";
			}
			// main audio preset
			else if( presetIndex == 0 && mainPresetIndex != 0 )
			{
				// at( mainPresetIndex - 1 ): subtract the index of the custom preset
				profile = _presets.getAudioProfiles().at( mainPresetIndex - 1 );
				presetName = profile.find( avtranscoder::Profile::avProfileIdentificator )->second;
				
			}
			// specific audio preset
			else
			{
				// at( presetIndex - 1 ): subtract the index of the index of the main preset
				profile = _presets.getAudioProfiles().at( presetIndex - 1 );
				presetName = profile.find( avtranscoder::Profile::avProfileIdentificator )->second;
			}
			
			// dummy
			if( _paramAudioSilent.at( i )->getValue() )
			{	
				avtranscoder::AudioDesc audioDesc( profile[ avtranscoder::Profile::avProfileCodec ] );
				const size_t sampleRate = boost::lexical_cast<size_t>( profile[ avtranscoder::Profile::avProfileSampleRate ] );
				const size_t channels = boost::lexical_cast<size_t>( profile[ avtranscoder::Profile::avProfileChannel ] );
				audioDesc.setAudioParameters( 
					sampleRate, 
					channels, 
					avtranscoder::OptionLoader::getAVSampleFormat( profile[ avtranscoder::Profile::avProfileSampleFormat ] ) );
				
				_transcoder->add( "", 1, profile, audioDesc );
			}
			else
			{
				int inputStreamIndex = _paramAudioStreamIndex.at( i )->getValue();
				
				size_t nbStream = 1;
				if( inputStreamIndex == -1 )
				{
					avtranscoder::ProgressListener progress;
					avtranscoder::Properties properties = avtranscoder::InputFile::analyseFile( inputFileName, progress, avtranscoder::InputFile::eAnalyseLevelFast );
					nbStream = properties.streamsCount;
				}
				
				// rewrap
				if( ! _paramAudioSilent.at( i )->getValue() &&  _paramAudioCopyStream.at( i )->getValue() )
				{
					if( inputStreamIndex != -1 )
					{
						_transcoder->add( inputFileName, inputStreamIndex, "" );
					}
					else
					{
						for( size_t streamIndex = 0; streamIndex < nbStream; ++streamIndex )
						{
							_transcoder->add( inputFileName, streamIndex, "" );
						}
					}
				}
				// transcode
				else
				{
					size_t subStream = -1;
					if( ! _paramAudioAllChannels.at( i )->getValue() )
					{
						subStream = _paramAudioChannelIndex.at( i )->getValue();
					}
					
					// custom audio preset
					if( presetIndex == 0 && mainPresetIndex == 0 )
					{
						if( inputStreamIndex != -1 )
						{
							_transcoder->add( inputFileName, inputStreamIndex, subStream, profile );
						}
						else
						{
							for( size_t streamIndex = 0; streamIndex < nbStream; ++streamIndex )
							{
								_transcoder->add( inputFileName, streamIndex, subStream, profile );
							}
						}
					}
					// existing audio preset
					else
					{
						if( inputStreamIndex != -1 )
						{
							_transcoder->add( inputFileName, inputStreamIndex, subStream, presetName );
						}
						else
						{
							for( size_t streamIndex = 0; streamIndex < nbStream; ++streamIndex )
							{
								_transcoder->add( inputFileName, streamIndex, subStream, presetName );
							}
						}
					}
				}
			}
		}
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to create audio stream: " + e.what() );
	}
}

bool AVWriterPlugin::isOutputInit()
{
	if( _outputFile.get() == NULL ||
		_transcoder.get() == NULL )
		return false;
	return true;
}

void AVWriterPlugin::updateFormatProfile()
{
	AVProcessParams params = getProcessParams();
	
	_formatProfile[ avtranscoder::Profile::avProfileFormat ] = params._formatName;
	
	// format options from avTranscoder
	common::CustomParams::OptionsForPreset formatOptionsForPreset = _paramFormatCustom.getOptionsNameAndValue();
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, formatOptionsForPreset )
	{
		_formatProfile[ nameAndValue.first ] = nameAndValue.second;
	}
	
	// format options related to a codec from avTranscoder
	common::CustomParams::OptionsForPreset formatCodecOptionsForPreset = _paramFormatDetailCustom.getOptionsNameAndValue( params._formatName );
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, formatCodecOptionsForPreset )
	{
		_formatProfile[ nameAndValue.first ] = nameAndValue.second;
	}
}

void AVWriterPlugin::updateVideoProfile()
{
	AVProcessParams params = getProcessParams();
	
	_videoProfile[ avtranscoder::Profile::avProfileCodec ] = params._videoCodecName;
	_videoProfile[ avtranscoder::Profile::avProfilePixelFormat ] = params._videoPixelFormatName;

	if( _paramUseCustomFps->getValue() )
	{
		_videoProfile[ avtranscoder::Profile::avProfileFrameRate ] = boost::to_string( _paramCustomFps->getValue() );
	}
	else
	{
		_videoProfile[ avtranscoder::Profile::avProfileFrameRate ] = boost::to_string( _clipSrc->getFrameRate() );
	}

	_videoProfile[ "aspect" ] = boost::to_string( _clipSrc->getPixelAspectRatio() );

	// video options from avTranscoder
	common::CustomParams::OptionsForPreset videoOptionsForPreset = _paramVideoCustom.getOptionsNameAndValue();
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, videoOptionsForPreset )
	{
		_videoProfile[ nameAndValue.first ] = nameAndValue.second;
	}
	
	// video options related to a codec from avTranscoder
	common::CustomParams::OptionsForPreset videoCodecOptionsForPreset = _paramVideoDetailCustom.getOptionsNameAndValue( params._videoCodecName );
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, videoCodecOptionsForPreset )
	{
		_videoProfile[ nameAndValue.first ] = nameAndValue.second;
	}
}

void AVWriterPlugin::updateAudioProfile()
{
	AVProcessParams params = getProcessParams();
	
	_audioProfile[ avtranscoder::Profile::avProfileCodec ] = params._audioCodecName;
	_audioProfile[ avtranscoder::Profile::avProfileSampleFormat ] = params._audioSampleFormatName;

	// audio options from avTranscoder
	common::CustomParams::OptionsForPreset audioOptionsForPreset = _paramAudioCustom.getOptionsNameAndValue();
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, audioOptionsForPreset )
	{
		_audioProfile[ nameAndValue.first ] = nameAndValue.second;
	}
	
	// audio options related to a codec from avTranscoder
	common::CustomParams::OptionsForPreset audioCodecOptionsForPreset = _paramAudioDetailCustom.getOptionsNameAndValue( params._audioCodecName );
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, audioCodecOptionsForPreset )
	{
		_audioProfile[ nameAndValue.first ] = nameAndValue.second;
	}
}


void AVWriterPlugin::cleanVideoAndAudio()
{
	_outputFile.reset();
	_transcoder.reset();
	
	_initVideo = false;
	_initWrap = false;
	
	_lastOutputFilePath = "";
}

/**
 * @brief The overridden begin render function
 * @param[in]   args     Begin Rendering parameters
 */
void AVWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	// Before new render
	cleanVideoAndAudio();
	
	initOutput();
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AVWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	ensureVideoIsInit( args );

	if( ! _initWrap )
	{
		initAudio();
		_outputFile->beginWrap();
		_initWrap = true;
	}
	
	doGilRender<AVWriterProcess>( *this, args );
}

void AVWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{	
	if( ! _initWrap || ! _initVideo )
		return;
	
	_outputFile->endWrap();
}

}
}
}
}
