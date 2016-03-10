#include "AVWriterPlugin.hpp"
#include "AVWriterProcess.hpp"
#include "AVWriterDefinitions.hpp"

#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <cctype>
#include <sstream>
#include <utility>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

AVWriterPlugin::AVWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
	, _paramAudioSubGroup()
	, _paramAudioSilent()
	, _paramAudioFilePath()
	, _paramAudioFileInfo()
	, _paramAudioStreamIndex()
	, _paramAudioPreset()
	, _paramFormatCustom( common::kPrefixFormat, AV_OPT_FLAG_ENCODING_PARAM, false )
	, _paramVideoCustom( common::kPrefixVideo, AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, false )
	, _paramAudioCustom( common::kPrefixAudio, AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM, false )
	, _paramFormatDetailCustom( common::kPrefixFormat, AV_OPT_FLAG_ENCODING_PARAM, true )
	, _paramVideoDetailCustom( common::kPrefixVideo, AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, true )
	, _paramAudioDetailCustom( common::kPrefixAudio, AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM, true )
	, _paramMetadatas()
	, _outputFile( NULL )
	, _transcoder( NULL )
	, _presetLoader( true ) 
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
	
	_paramAudioNbInput = fetchIntParam( kParamAudioNbInputs );
	for( size_t indexAudioInput = 0; indexAudioInput < maxNbAudioInput; ++indexAudioInput )
	{
		std::ostringstream audioSubGroupName( kParamAudioSubGroup, std::ios_base::in | std::ios_base::ate );
		audioSubGroupName << "_" << indexAudioInput;
		_paramAudioSubGroup.push_back( fetchGroupParam( audioSubGroupName.str() ) );
		_paramAudioSubGroup.back()->setIsSecretAndDisabled( true );
		
		std::ostringstream audioSilentName( kParamAudioSilent, std::ios_base::in | std::ios_base::ate );
		audioSilentName << "_" << indexAudioInput;
		_paramAudioSilent.push_back( fetchBooleanParam( audioSilentName.str() ) );
		_paramAudioSilent.back()->setIsSecretAndDisabled( true );
		
		std::ostringstream audioFilePathName( kParamAudioFilePath, std::ios_base::in | std::ios_base::ate );
		audioFilePathName << "_" << indexAudioInput;
		_paramAudioFilePath.push_back( fetchStringParam( audioFilePathName.str() ) );
		_paramAudioFilePath.back()->setIsSecretAndDisabled( true );
		
		std::ostringstream audioFileNbStreamName( kParamAudioFileInfo, std::ios_base::in | std::ios_base::ate );
		audioFileNbStreamName << "_" << indexAudioInput;
		_paramAudioFileInfo.push_back( fetchStringParam( audioFileNbStreamName.str() ) );
		_paramAudioFileInfo.back()->setIsSecret( true );

		std::ostringstream audioSelectStreamName( kParamAudioSelectStream, std::ios_base::in | std::ios_base::ate );
		audioSelectStreamName << "_" << indexAudioInput;
		_paramAudioSelectStream.push_back( fetchBooleanParam( audioSelectStreamName.str() ) );
		_paramAudioSelectStream.back()->setIsSecretAndDisabled( true );

		std::ostringstream audioStreamIndexName( kParamAudioStreamIndex, std::ios_base::in | std::ios_base::ate );
		audioStreamIndexName << "_" << indexAudioInput;
		_paramAudioStreamIndex.push_back( fetchIntParam( audioStreamIndexName.str() ) );
		_paramAudioStreamIndex.back()->setIsSecretAndDisabled( true );
		
		std::ostringstream audioCodecPresetName( kParamAudioPreset, std::ios_base::in | std::ios_base::ate );
		audioCodecPresetName << "_" << indexAudioInput;
		_paramAudioPreset.push_back( fetchChoiceParam( audioCodecPresetName.str() ) );
		_paramAudioPreset.back()->setIsSecretAndDisabled( true );

		std::ostringstream audioOffsetName( kParamAudioOffset, std::ios_base::in | std::ios_base::ate );
		audioOffsetName << "_" << indexAudioInput;
		_paramAudioOffset.push_back( fetchDoubleParam( audioOffsetName.str() ) );
		_paramAudioOffset.back()->setIsSecretAndDisabled( true );
	}

	// our custom params
	_paramUseCustomFps = fetchBooleanParam( kParamUseCustomFps );
	_paramCustomFps = fetchDoubleParam( kParamCustomFps );

	_paramUseCustomSize = fetchBooleanParam( kParamUseCustomSize );
	_paramCustomSize = fetchInt2DParam( kParamCustomSize );

	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	_paramAudioSampleFormat = fetchChoiceParam( kParamAudioCodecSampleFmt );

	// all custom params
	_paramFormatCustom.fetchLibAVParams( *this, common::kPrefixFormat );
	_paramVideoCustom.fetchLibAVParams( *this, common::kPrefixVideo );
	_paramAudioCustom.fetchLibAVParams( *this, common::kPrefixAudio );
	
	_paramFormatDetailCustom.fetchLibAVParams( *this, common::kPrefixFormat );
	_paramVideoDetailCustom.fetchLibAVParams( *this, common::kPrefixVideo );
	_paramAudioDetailCustom.fetchLibAVParams( *this, common::kPrefixAudio );

    const std::string formatName = getFormatName(_paramFormat->getValue());
    const std::string videoCodecName = getVideoCodecName(_paramVideoCodec->getValue());
    const std::string audioCodecName = getAudioCodecName(_paramAudioCodec->getValue());

	common::disableOFXParamsForFormatOrCodec( *this, _libavFeatures._optionsPerOutputFormat, formatName, common::kPrefixFormat );
	common::disableOFXParamsForFormatOrCodec( *this, _libavFeatures._optionsPerVideoCodec, videoCodecName, common::kPrefixVideo );
	common::disableOFXParamsForFormatOrCodec( *this, _libavFeatures._optionsPerAudioCodec, audioCodecName, common::kPrefixAudio );
	
	updatePixelFormats( videoCodecName );
	updateSampleFormats( audioCodecName );

	_paramVerbose = fetchBooleanParam( kParamVerbose );
	
	// preset
	_paramMainPreset = fetchChoiceParam( kParamMainPreset );
	_paramFormatPreset = fetchChoiceParam( kParamFormatPreset );
	_paramVideoPreset = fetchChoiceParam( kParamVideoPreset );
	_paramAudioMainPreset = fetchChoiceParam( kParamAudioMainPreset );
	
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

	updateAudioParams();
	updateVisibleTools();
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
	
	params._formatId = _paramFormat->getValue();
	params._formatName = getFormatName( params._formatId );
	
	params._videoCodecId = _paramVideoCodec->getValue();
	params._videoCodecName = getVideoCodecName( params._videoCodecId );
	
	params._audioCodecId = _paramAudioCodec->getValue();
	params._audioCodecName = getAudioCodecName( params._audioCodecId );

	params._videoPixelFormatName = getPixelFormatName( params._videoCodecName );
	params._audioSampleFormatName = getSampleFormatName( params._audioCodecName );

	BOOST_FOREACH( OFX::StringParam* ofxParam, _paramMetadatas )
	{
		if( ofxParam->getValue().size() > 0 )
		{
			std::string libavKey = ofxParam->getName();
			std::string libavValue = ofxParam->getValue();
			libavKey.erase( 0, common::prefixSize );
			params._metadatas.push_back( std::make_pair( libavKey, libavValue ) );
		}
	}
	return params;
}

void AVWriterPlugin::updatePixelFormats( const std::string& videoCodecName )
{
	_paramVideoPixelFormat->resetOptions();
	
	std::vector<std::string> pixelFormats( avtranscoder::getSupportedPixelFormats( videoCodecName ) );
	// get all pixel formats if the list is empty with the video codec indicated
	if( pixelFormats.empty() )
	{
		pixelFormats = _libavFeatures._allSupportedPixelFormats;
	}
	
	for( std::vector<std::string>::iterator it = pixelFormats.begin(); it != pixelFormats.end(); ++it )
	{
		_paramVideoPixelFormat->appendOption( *it );
	}
}

void AVWriterPlugin::updateSampleFormats( const std::string& audioCodecName )
{
	_paramAudioSampleFormat->resetOptions();
	
	std::vector<std::string> sampleFormats( avtranscoder::getSupportedSampleFormats( audioCodecName ) );
	// get all sample formats if the list is empty with the audio codec indicated
	if( sampleFormats.empty() )
	{
		sampleFormats = _libavFeatures._allSupportedSampleFormats;
	}
	
	for( std::vector<std::string>::iterator it = sampleFormats.begin(); it != sampleFormats.end(); ++it )
	{
		_paramAudioSampleFormat->appendOption( *it );
	}
}

void AVWriterPlugin::updateAudioParams()
{
	for( size_t indexAudioOutput = 0; indexAudioOutput < maxNbAudioInput; ++indexAudioOutput )
	{
		bool isStreamConcerned = indexAudioOutput < (size_t)_paramAudioNbInput->getValue();
		_paramAudioSubGroup.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );
		_paramAudioSilent.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );
		_paramAudioFilePath.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );
		_paramAudioFileInfo.at( indexAudioOutput )->setIsSecret( ! isStreamConcerned );
		_paramAudioSelectStream.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );
		_paramAudioStreamIndex.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );
		_paramAudioPreset.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );
		_paramAudioOffset.at( indexAudioOutput )->setIsSecretAndDisabled( ! isStreamConcerned );

		updateAudioSilent( indexAudioOutput );
	}
}

void AVWriterPlugin::updateAudioSilent( size_t indexAudioOutput )
{
	if( _paramAudioSubGroup.at( indexAudioOutput )->getIsEnable() &&
		! _paramAudioSubGroup.at( indexAudioOutput )->getIsSecret() )
	{
		const bool isSilent = _paramAudioSilent.at( indexAudioOutput )->getValue();
		_paramAudioFilePath.at( indexAudioOutput )->setIsSecretAndDisabled( isSilent );
		_paramAudioFileInfo.at( indexAudioOutput )->setIsSecret( isSilent );
		_paramAudioSelectStream.at( indexAudioOutput )->setIsSecretAndDisabled( isSilent );
		_paramAudioStreamIndex.at( indexAudioOutput )->setIsSecretAndDisabled( isSilent );
		_paramAudioOffset.at( indexAudioOutput )->setIsSecretAndDisabled( isSilent );
		_paramAudioPreset.at( indexAudioOutput )->setIsSecretAndDisabled( false );
	}
	updateAudioSelectStream( indexAudioOutput );
}


void AVWriterPlugin::updateAudioSelectStream( size_t indexAudioOutput )
{
	if( _paramAudioSubGroup.at( indexAudioOutput )->getIsEnable() &&
		! _paramAudioSubGroup.at( indexAudioOutput )->getIsSecret() &&
		! _paramAudioSilent.at( indexAudioOutput )->getValue() )
	{
		const bool isSelectStream = _paramAudioSelectStream.at( indexAudioOutput )->getValue();
		_paramAudioStreamIndex.at( indexAudioOutput )->setIsSecretAndDisabled( ! isSelectStream );
		if( isSelectStream )
		{
			const std::string inputFilePath = _paramAudioFilePath.at( indexAudioOutput )->getValue();
			if( boost::filesystem::exists( inputFilePath ) )
			{
				avtranscoder::NoDisplayProgress progress;
				const avtranscoder::FileProperties fileProperties = avtranscoder::InputFile::analyseFile( inputFilePath, progress, avtranscoder::eAnalyseLevelHeader );
				const size_t nbAudioStream = fileProperties.getAudioProperties().size();
				_paramAudioStreamIndex.at( indexAudioOutput )->setRange( 0, nbAudioStream );
				_paramAudioStreamIndex.at( indexAudioOutput )->setDisplayRange( 0, nbAudioStream );
			}
		}
	}
	updateAudioOffset( indexAudioOutput );
}

void AVWriterPlugin::updateAudioOffset( size_t indexAudioOutput )
{
	if( _paramAudioSubGroup.at( indexAudioOutput )->getIsEnable() &&
		! _paramAudioSubGroup.at( indexAudioOutput )->getIsSecret() &&
		! _paramAudioSilent.at( indexAudioOutput )->getValue() )
	{
		bool isRewrap = false;
		switch( _paramAudioPreset.at( indexAudioOutput )->getValue() )
		{
			// check main audio preset
			case 0:
				if( _paramAudioMainPreset->getValue() == 1 )
					isRewrap = true;
				break;
			// check audio preset at index
			case 1:
				isRewrap = true;
				break;
			default:
				break;
		}
		_paramAudioOffset.at( indexAudioOutput )->setIsSecretAndDisabled( isRewrap );
	}
}


void AVWriterPlugin::updateAudioFileInfo( size_t indexAudioOutput )
{
	if( _paramAudioSubGroup.at( indexAudioOutput )->getIsEnable() &&
		! _paramAudioSubGroup.at( indexAudioOutput )->getIsSecret() &&
		! _paramAudioSilent.at( indexAudioOutput )->getValue() )
	{
		std::string inputFilePath = _paramAudioFilePath.at( indexAudioOutput )->getValue();
		if( boost::filesystem::exists( inputFilePath ) )
		{
			avtranscoder::NoDisplayProgress progress;
			const avtranscoder::FileProperties fileProperties = avtranscoder::InputFile::analyseFile( inputFilePath, progress, avtranscoder::eAnalyseLevelHeader );
			const size_t nbAudioStream = fileProperties.getAudioProperties().size();
			std::string audioInfo = "Audio streams: ";
			audioInfo += boost::lexical_cast<std::string>( nbAudioStream );
			audioInfo += "\n";
			for( size_t audioStreamIndex = 0; audioStreamIndex < nbAudioStream; ++audioStreamIndex )
			{
				const avtranscoder::AudioProperties& audioProperties = fileProperties.getAudioProperties().at( audioStreamIndex );

				// stream
				audioInfo += "Stream ";
				audioInfo += boost::lexical_cast<std::string>( audioStreamIndex );
				audioInfo += ": ";

				// channels
				audioInfo += boost::lexical_cast<std::string>( audioProperties.getNbChannels() );
				audioInfo += " channels";

				// channel layout
				audioInfo += " (layout ";
				audioInfo += audioProperties.getChannelLayout();
				audioInfo += ")";
				audioInfo += "\n";
			}
			_paramAudioFileInfo.at( indexAudioOutput )->setValue( audioInfo );
		}
		else
		{
			_paramAudioFileInfo.at( indexAudioOutput )->setValue( "" );
		}
	}
}


void AVWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );

	// filename
	if( paramName == kTuttlePluginFilename )
	{
		if( ! _paramFilepath->getValue().empty() )
		{
			const avtranscoder::OutputFile output( _paramFilepath->getValue() );
			std::istringstream extensions( output.getFormatName() );
			if( extensions.rdbuf()->in_avail() )
			{
				// use first format defined in the file to set the list of formats
				std::string extension;
				std::getline( extensions, extension, ',' );
				setFormatParam( extension );
			}
		}
	}
	// format
	else if( paramName == kParamFormat )
	{
		const std::string formatName = getFormatName( _paramFormat->getValue() );
		common::disableOFXParamsForFormatOrCodec( *this, _libavFeatures._optionsPerOutputFormat, formatName, common::kPrefixFormat );
	}
	// codecs
	else if( paramName == kParamVideoCodec )
	{
		const std::string videoCodecName = getVideoCodecName( _paramVideoCodec->getValue() );
		common::disableOFXParamsForFormatOrCodec( *this, _libavFeatures._optionsPerVideoCodec, videoCodecName, common::kPrefixVideo );
		
		updatePixelFormats( videoCodecName );
	}
	else if( paramName == kParamAudioCodec )
	{
		const std::string audioCodecName = getAudioCodecName(_paramAudioCodec->getValue() );
		common::disableOFXParamsForFormatOrCodec( *this, _libavFeatures._optionsPerAudioCodec, audioCodecName, common::kPrefixAudio );
		
		updateSampleFormats( audioCodecName );
	}
	// presets
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
		}
		else
		{
			updateFormatFromExistingProfile();
		}
	}
	else if( paramName == kParamVideoPreset )
	{
		// if custom video preset
		if( _paramVideoPreset->getValue() == 0 )
		{
			int defaultVideoCodecIndex;
			_paramVideoCodec->getDefault( defaultVideoCodecIndex );
			_paramVideoCodec->setValue( defaultVideoCodecIndex );
		}
		else
		{
			updateVideoFromExistingProfile();
		}
	}
	else if( paramName == kParamAudioMainPreset )
	{
		// if custom audio preset or rewrap
		if( _paramAudioMainPreset->getValue() == 0 || 
			_paramAudioMainPreset->getValue() == 1 )
		{
			int defaultAudioCodecIndex;
			_paramAudioCodec->getDefault( defaultAudioCodecIndex );
			_paramAudioCodec->setValue( defaultAudioCodecIndex );
		}
		else
		{
			updateAudioFromExistingProfile();
		}

		// update offset visibility of audio inputs
		for( size_t indexAudioInput = 0; indexAudioInput < _paramAudioNbInput->getValue(); ++indexAudioInput )
		{
			updateAudioOffset( indexAudioInput );
		}
	}
	// fps
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
	// size
	else if( paramName == kParamUseCustomSize )
	{
		_paramCustomSize->setIsSecretAndDisabled( ! _paramUseCustomSize->getValue() );
	}
	else if( paramName == kParamCustomSize )
	{
		if( ! _paramUseCustomSize->getValue() )
		{
			_paramUseCustomSize->setValue(true);
		}
	}
	// audio parameters
	else if( paramName == kParamAudioNbInputs )
	{
		updateAudioParams();
	}
	else if( paramName.find( kParamAudioSilent ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioSilent.size() + 1; // add "_"
		const size_t indexAudioOutput = boost::lexical_cast<size_t>( paramName.substr( indexPos ) );
		updateAudioSilent( indexAudioOutput );
	}
	else if( paramName.find( kParamAudioSelectStream ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioSelectStream.size() + 1; // add "_"
		const size_t indexAudioOutput = boost::lexical_cast<size_t>( paramName.substr( indexPos ) );
		updateAudioSelectStream( indexAudioOutput );
	}
	else if( paramName.find( kParamAudioPreset ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioPreset.size() + 1; // add "_"
		const size_t indexAudioOutput = boost::lexical_cast<size_t>( paramName.substr( indexPos ) );
		updateAudioOffset( indexAudioOutput );
	}
	else if( paramName.find( kParamAudioFilePath ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioFilePath.size() + 1; // add "_"
		const size_t indexAudioOutput = boost::lexical_cast<size_t>( paramName.substr( indexPos ) );
		_paramAudioSilent.at( indexAudioOutput )->setValue( false );

		// Update number of audio input
		if( ( indexAudioOutput + 1 ) > _paramAudioNbInput->getValue() )
			_paramAudioNbInput->setValue( indexAudioOutput + 1 );

		updateAudioFileInfo( indexAudioOutput );
	}
	else if( paramName.find( kParamAudioStreamIndex ) != std::string::npos )
	{
		const size_t indexPos = kParamAudioStreamIndex.size() + 1; // add "_"
		const size_t indexAudioOutput = boost::lexical_cast<size_t>( paramName.substr( indexPos ) );
		_paramAudioSelectStream.at( indexAudioOutput )->setValue( true );
	}
	// verbose
	else if( paramName == kParamVerbose )
	{
		if( _paramVerbose->getValue() )
			avtranscoder::Logger::setLogLevel( AV_LOG_DEBUG );
		else
			avtranscoder::Logger::setLogLevel( AV_LOG_QUIET );
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
	
	try
	{
		// create output file
		_outputFile.reset( new avtranscoder::OutputFile( params._outputFilePath ) );

		// add metadatas
		_outputFile->addMetadata( params._metadatas );

		// create transcoder
		_transcoder.reset( new avtranscoder::Transcoder( *_outputFile ) );
		_transcoder->setProcessMethod( avtranscoder::eProcessMethodBasedOnStream, 0 );
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open output file: " + e.what() );
	}
}

void AVWriterPlugin::initVideo( const OFX::RenderArguments& args )
{
	AVProcessParams params = getProcessParams();

	// no output file indicated
	if( params._outputFilePath == "" )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "no output file indicated"
		    << exception::filename( params._outputFilePath ) );
	}

	// output is not init
	if( ! isOutputInit() )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "the output is not init. Can't process." );
	}
	
	try
	{
		// Get custom video profile
		avtranscoder::ProfileLoader::Profile profile;
		profile[ avtranscoder::constants::avProfileIdentificator ] = "customVideoPreset";
		profile[ avtranscoder::constants::avProfileIdentificatorHuman ] = "Custom video preset";
		profile[ avtranscoder::constants::avProfileType ] = avtranscoder::constants::avProfileTypeVideo;
		profile[ avtranscoder::constants::avProfileCodec ] = params._videoCodecName;

		// pixel format
		profile[ avtranscoder::constants::avProfilePixelFormat ] = params._videoPixelFormatName;

		// fps
		if( _paramUseCustomFps->getValue() )
			profile[ avtranscoder::constants::avProfileFrameRate ] = boost::to_string( _paramCustomFps->getValue() );
		else
			profile[ avtranscoder::constants::avProfileFrameRate ] = boost::to_string( _clipSrc->getFrameRate() );

		// size
		if( _paramUseCustomSize->getValue() )
		{
			profile[ avtranscoder::constants::avProfileWidth ] = boost::to_string( _paramCustomSize->getValue().x );
			profile[ avtranscoder::constants::avProfileHeight ] = boost::to_string( _paramCustomSize->getValue().y );
		}

		// video options
		const avtranscoder::ProfileLoader::Profile videoProfile = _paramVideoCustom.getCorrespondingProfile();
		profile.insert( videoProfile.begin(), videoProfile.end() );

		// video detail options
		const avtranscoder::ProfileLoader::Profile videoDetailProfile = _paramVideoDetailCustom.getCorrespondingProfile( params._videoCodecName );
		profile.insert( videoDetailProfile.begin(), videoDetailProfile.end() );

		cleanProfile( profile, common::kPrefixVideo );

		// get rod
		const OfxRectI bounds = _clipSrc->getPixelRod( args.time, args.renderScale );
		const int width = bounds.x2 - bounds.x1;
		const int height = bounds.y2 - bounds.y1;

		// describe input frame and codec of transcode
		avtranscoder::VideoCodec videoCodec( avtranscoder::eCodecTypeEncoder, profile[ avtranscoder::constants::avProfileCodec ] );
		avtranscoder::VideoFrameDesc imageDesc( width, height, "rgb24" );
		videoCodec.setImageParameters( imageDesc );

		// add video stream
		_transcoder->add( "", 0, profile, videoCodec );
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
	if( ! _paramAudioNbInput->getValue() )
		return;

	if( ! isOutputInit() )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "the output is not init. Can't process." );
	}

	try
	{
		const size_t mainPresetIndex = _paramAudioMainPreset->getValue();
		avtranscoder::ProfileLoader::Profile profile;
		std::string presetName( "" );
		
		for( int i = 0; i < _paramAudioNbInput->getValue(); ++i )
		{
			const std::string inputFileName( _paramAudioFilePath.at( i )->getValue() );
			const size_t presetIndex = _paramAudioPreset.at( i )->getValue();
			const bool isSilent = _paramAudioSilent.at( i )->getValue();
			
			// No effect if file path is empty
			if( ! isSilent && inputFileName.empty() )
				continue;

			// Get custom audio profile
			if( presetIndex == 0 && mainPresetIndex == 0 )
			{
				AVProcessParams params = getProcessParams();

				profile[ avtranscoder::constants::avProfileIdentificator ] = "customAudioPreset";
				profile[ avtranscoder::constants::avProfileIdentificatorHuman ] = "Custom audio preset";
				profile[ avtranscoder::constants::avProfileType ] = avtranscoder::constants::avProfileTypeAudio;
				profile[ avtranscoder::constants::avProfileCodec ] = params._audioCodecName;

				// sample format
				if( ! params._audioSampleFormatName.empty() )
					profile[ avtranscoder::constants::avProfileSampleFormat ] = params._audioSampleFormatName;

				// audio options
				avtranscoder::ProfileLoader::Profile audioProfile = _paramAudioCustom.getCorrespondingProfile();
				profile.insert( audioProfile.begin(), audioProfile.end() );

				// audio detail options
				avtranscoder::ProfileLoader::Profile audioDetailProfile = _paramAudioDetailCustom.getCorrespondingProfile( params._audioCodecName );
				profile.insert( audioDetailProfile.begin(), audioDetailProfile.end() );

				cleanProfile( profile, common::kPrefixAudio );
			}
			// Rewrap
			else if( presetIndex == 1 || ( presetIndex == 0 && mainPresetIndex == 1 ) )
			{
				presetName = "";
			}
			// Audio profile
			else
			{
				// from main preset
				if( presetIndex == 0 )
				{
					// at( mainPresetIndex - 2 ): subtract the index of custom preset and rewrap
					profile = _presetLoader.getAudioProfiles().at( mainPresetIndex - 2 );
					presetName = profile.find( avtranscoder::constants::avProfileIdentificator )->second;
				}
				// from specific preset
				else
				{
					// at( presetIndex - 2 ): subtract the index of main preset and rewrap
					profile = _presetLoader.getAudioProfiles().at( presetIndex - 2 );
					presetName = profile.find( avtranscoder::constants::avProfileIdentificator )->second;
				}
			}
			
			// generator
			if( isSilent )
			{
				try
				{
					avtranscoder::AudioCodec audioCodec( avtranscoder::eCodecTypeEncoder, profile.at( avtranscoder::constants::avProfileCodec ) );
					const size_t sampleRate = boost::lexical_cast<size_t>( profile.at( avtranscoder::constants::avProfileSampleRate ) );
					const size_t channels = boost::lexical_cast<size_t>( profile.at( avtranscoder::constants::avProfileChannel ) );
					avtranscoder::AudioFrameDesc audioDesc( sampleRate, channels, profile.at( avtranscoder::constants::avProfileSampleFormat ) );
					audioCodec.setAudioParameters( audioDesc );

					// add silent stream
					_transcoder->add( "", 1, profile, audioCodec );
				}
				catch( std::out_of_range& e )
				{
					throw std::runtime_error( "unable to create an audio silent stream with no preset of encoding indicated" );
				}
			}
			else
			{
				bool selectOneStream = _paramAudioSelectStream.at( i )->getValue();
				int inputStreamIndex = selectOneStream ? _paramAudioStreamIndex.at( i )->getValue() : -1;				

				// Get index of audio streams
				std::vector< int > indexAudioStreams;
				if( inputStreamIndex == -1 )
				{
					avtranscoder::NoDisplayProgress progress;
					const avtranscoder::FileProperties fileProperties = avtranscoder::InputFile::analyseFile( inputFileName, progress, avtranscoder::eAnalyseLevelHeader );
					const std::vector< avtranscoder::AudioProperties > audioProperties = fileProperties.getAudioProperties();
					for( size_t propertiesIndex = 0; propertiesIndex < audioProperties.size(); ++propertiesIndex )
					{
						indexAudioStreams.push_back( audioProperties.at( propertiesIndex ).getStreamIndex() );
					}
				}
				
				// rewrap
				if( ! isSilent && presetIndex == 1 )
				{
					if( inputStreamIndex != -1 )
					{
						_transcoder->add( inputFileName, inputStreamIndex, presetName );
					}
					else
					{
						for( std::vector< int >::iterator itStreamIndex = indexAudioStreams.begin(); itStreamIndex != indexAudioStreams.end(); ++itStreamIndex )
						{
							_transcoder->add( inputFileName, *itStreamIndex, presetName );
						}
					}
				}
				// transcode
				else
				{
					// select all channels of the stream
					size_t subStream = -1;
					double offset = _paramAudioOffset.at( i )->getValue();

					// custom audio preset
					if( presetIndex == 0 && mainPresetIndex == 0 )
					{
						if( inputStreamIndex != -1 )
						{
							_transcoder->add( inputFileName, inputStreamIndex, subStream, profile, offset );
						}
						else
						{
							for( std::vector< int >::iterator itStreamIndex = indexAudioStreams.begin(); itStreamIndex != indexAudioStreams.end(); ++itStreamIndex )
							{
								_transcoder->add( inputFileName, *itStreamIndex, subStream, profile, offset );
							}
						}
					}
					// existing audio preset
					else
					{
						if( inputStreamIndex != -1 )
						{
							_transcoder->add( inputFileName, inputStreamIndex, subStream, presetName, offset );
						}
						else
						{
							for( std::vector< int >::iterator itStreamIndex = indexAudioStreams.begin(); itStreamIndex != indexAudioStreams.end(); ++itStreamIndex )
							{
								_transcoder->add( inputFileName, *itStreamIndex, subStream, presetName, offset );
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

void AVWriterPlugin::updateFormatFromExistingProfile()
{
	const size_t presetIndex = _paramFormatPreset->getValue();
	
	// existing format preset
	if( presetIndex != 0 )
	{
		// at( presetIndex - 1 ): subtract the index of custom preset
		const avtranscoder::ProfileLoader::Profile existingProfile = _presetLoader.getFormatProfiles().at( presetIndex - 1 );

		// format
		setFormatParam( existingProfile.at(avtranscoder::constants::avProfileFormat) );

		// other options
		BOOST_FOREACH( const avtranscoder::ProfileLoader::Profile::value_type& option, existingProfile )
		{
			if( option.first == avtranscoder::constants::avProfileIdentificator ||
				option.first == avtranscoder::constants::avProfileIdentificatorHuman ||
				option.first == avtranscoder::constants::avProfileType ||
				option.first == avtranscoder::constants::avProfileFormat )
				continue;

			if( ! _paramFormatCustom.setOption( option.first, option.second ) )
				_paramFormatDetailCustom.setOption( option.first, option.second, existingProfile.at(avtranscoder::constants::avProfileFormat) );
		}
	}
}

void AVWriterPlugin::updateVideoFromExistingProfile()
{
	const size_t presetIndex = _paramVideoPreset->getValue();
	
	// existing video preset
	if( presetIndex != 0 )
	{
		// at( presetIndex - 1 ): subtract the index of custom preset
		const avtranscoder::ProfileLoader::Profile existingProfile = _presetLoader.getVideoProfiles().at( presetIndex - 1 );

		// video codec
		setVideoCodecParam( existingProfile.at(avtranscoder::constants::avProfileCodec) );

		// pixel format
		if( existingProfile.find( avtranscoder::constants::avProfilePixelFormat ) != existingProfile.end() )
		{
			const std::vector<std::string> pixelFormats( avtranscoder::getSupportedPixelFormats( existingProfile.at(avtranscoder::constants::avProfileCodec) ) );
			std::vector<std::string>::const_iterator iterPixelFormat = std::find( pixelFormats.begin(), pixelFormats.end(), existingProfile.at(avtranscoder::constants::avProfilePixelFormat) );
			const size_t pixelFomatIndex = std::distance( pixelFormats.begin(), iterPixelFormat);
			if( pixelFomatIndex < pixelFormats.size() )
				_paramVideoPixelFormat->setValue( pixelFomatIndex );
			else
				updatePixelFormats( existingProfile.at(avtranscoder::constants::avProfileCodec) );
		}
		else
			updatePixelFormats( existingProfile.at(avtranscoder::constants::avProfileCodec) );

		// frame rate
		if( existingProfile.find( avtranscoder::constants::avProfileFrameRate ) != existingProfile.end() )
			_paramCustomFps->setValue( boost::lexical_cast<double>( existingProfile.at(avtranscoder::constants::avProfileFrameRate) ) );
		else
			_paramUseCustomFps->setValue( false );

		// size
		if( existingProfile.find( avtranscoder::constants::avProfileWidth ) != existingProfile.end() &&
			existingProfile.find( avtranscoder::constants::avProfileHeight ) != existingProfile.end() )
			_paramCustomSize->setValue(
				boost::lexical_cast<int>( existingProfile.at(avtranscoder::constants::avProfileWidth) ), 
				boost::lexical_cast<int>( existingProfile.at(avtranscoder::constants::avProfileHeight) )
			);
		else
			_paramUseCustomSize->setValue( false );

		// other options
		BOOST_FOREACH( const avtranscoder::ProfileLoader::Profile::value_type& option, existingProfile )
		{
			if( option.first == avtranscoder::constants::avProfileIdentificator ||
				option.first == avtranscoder::constants::avProfileIdentificatorHuman ||
				option.first == avtranscoder::constants::avProfileType ||
				option.first == avtranscoder::constants::avProfileCodec ||
				option.first == avtranscoder::constants::avProfilePixelFormat ||
				option.first == avtranscoder::constants::avProfileFrameRate )
				continue;

			if( ! _paramVideoCustom.setOption( option.first, option.second ) )
				_paramVideoDetailCustom.setOption( option.first, option.second, existingProfile.at(avtranscoder::constants::avProfileCodec) );
		}
	}
}

void AVWriterPlugin::updateAudioFromExistingProfile()
{
	const size_t presetIndex = _paramAudioMainPreset->getValue();
	
	// existing audio preset
	if( presetIndex > 1 )
	{
		// at( presetIndex - 2 ): subtract the index of custom preset and rewrap
		const avtranscoder::ProfileLoader::Profile existingProfile = _presetLoader.getAudioProfiles().at( presetIndex - 2 );

		// audio codec
		setAudioCodecParam( existingProfile.at(avtranscoder::constants::avProfileCodec) );

		// sample format
		if( existingProfile.find( avtranscoder::constants::avProfileSampleFormat ) != existingProfile.end() )
		{
			const std::vector<std::string> sampleFormats( avtranscoder::getSupportedSampleFormats( existingProfile.at(avtranscoder::constants::avProfileCodec) ) );
			std::vector<std::string>::const_iterator iterSampleFormat = std::find( sampleFormats.begin(), sampleFormats.end(), existingProfile.at(avtranscoder::constants::avProfileSampleFormat) );
			const size_t sampleFomatIndex = std::distance( sampleFormats.begin(), iterSampleFormat);
			if( sampleFomatIndex < sampleFormats.size() )
			{
				_paramAudioSampleFormat->setValue( sampleFomatIndex );
			}
			else
				updateSampleFormats( existingProfile.at(avtranscoder::constants::avProfileCodec) );
		}
		else
			updateSampleFormats( existingProfile.at(avtranscoder::constants::avProfileCodec) );

		// other options
		BOOST_FOREACH( const avtranscoder::ProfileLoader::Profile::value_type& option, existingProfile )
		{
			if( option.first == avtranscoder::constants::avProfileIdentificator ||
				option.first == avtranscoder::constants::avProfileIdentificatorHuman ||
				option.first == avtranscoder::constants::avProfileType ||
				option.first == avtranscoder::constants::avProfileCodec ||
				option.first == avtranscoder::constants::avProfileSampleFormat )
				continue;

			if( ! _paramAudioCustom.setOption( option.first, option.second ) )
				_paramAudioDetailCustom.setOption( option.first, option.second, existingProfile.at(avtranscoder::constants::avProfileCodec) );
		}
	}
}

void AVWriterPlugin::cleanVideoAndAudio()
{
	_outputFile.reset();
	_transcoder.reset();
	
	_initVideo = false;
	_initWrap = false;
}

void AVWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	WriterPlugin::beginSequenceRender( args );

	// Before new render
	cleanVideoAndAudio();
	initOutput();
}

void AVWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( ! _clipSrc->fetchImage( args.time ) || ! _clipDst->fetchImage( args.time ) )
		return;

	WriterPlugin::render( args );

	// @note: initVideo is called here because we need rendering parameters (ROD...)
	// of input in order to create our output video stream
	if( ! _initVideo )
		initVideo( args );

	if( ! _initWrap )
	{
		// add audio streams
		initAudio();

		// @note: beginWrap needs to be called once, and after have added all the streams (video/audio)
		_outputFile->beginWrap();
		_initWrap = true;

		// Get format profile
		const AVProcessParams params = getProcessParams();
		avtranscoder::ProfileLoader::Profile profile;
		profile[ avtranscoder::constants::avProfileIdentificator ] = "customFormatPreset";
		profile[ avtranscoder::constants::avProfileIdentificatorHuman ] = "Custom format preset";
		profile[ avtranscoder::constants::avProfileType ] = avtranscoder::constants::avProfileTypeFormat;
		profile[ avtranscoder::constants::avProfileFormat ] = params._formatName;

		const avtranscoder::ProfileLoader::Profile formatProfile = _paramFormatCustom.getCorrespondingProfile();
		profile.insert( formatProfile.begin(), formatProfile.end() );

		const avtranscoder::ProfileLoader::Profile formatDetailProfile = _paramFormatDetailCustom.getCorrespondingProfile( params._formatName );
		profile.insert( formatDetailProfile.begin(), formatDetailProfile.end() );

		cleanProfile( profile, common::kPrefixFormat );

		// set format profile (need to be done after beginWrap when muxing)
		_outputFile->setupWrapping( profile );

		// manage codec lantancy
		_transcoder->preProcessCodecLatency();
	}
	
	doGilRender<AVWriterProcess>( *this, args );
}

void AVWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{	
	if( ! _initWrap || ! _initVideo )
		return;
	
	WriterPlugin::endSequenceRender( args );

	// encode and wrap last frames
	std::vector< avtranscoder::StreamTranscoder* >& streams = _transcoder->getStreamTranscoders();
	for( size_t streamIndex = 0; streamIndex < streams.size(); ++streamIndex )
	{
		avtranscoder::IEncoder* encoder = streams.at( streamIndex )->getEncoder();
		avtranscoder::CodedData data;
		while( encoder->encodeFrame( data ) )
			streams.at( streamIndex )->getOutputStream().wrap( data );
	}

	// end wrapping
	_outputFile->endWrap();
}

std::string AVWriterPlugin::getFormatName( const size_t formatIndex ) const
{
	try
	{
		return _libavFeatures._availableFormats.at( formatIndex ).first;
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to get format " + formatIndex + " - " + e.what() );
	}
}

std::string AVWriterPlugin::getVideoCodecName( const size_t codecIndex ) const
{
	try
	{
		return _libavFeatures._availableVideoCodecs.at( codecIndex ).first;
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to get video codec " + codecIndex + " - " + e.what() );
	}
}

std::string AVWriterPlugin::getAudioCodecName( const size_t codecIndex ) const
{
	try
	{
		return _libavFeatures._availableAudioCodecs.at( codecIndex ).first;
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to get audio codec " + codecIndex + " - " + e.what() );
	}
}

std::string AVWriterPlugin::getPixelFormatName( const std::string& videoCodecName ) const
{
	std::vector<std::string> supportedPixelFormats( avtranscoder::getSupportedPixelFormats( videoCodecName ) );

	try
	{
		// no pixel format declared by the codec: return pixel format selected by the user
		if( supportedPixelFormats.empty() )
		{
			supportedPixelFormats = _libavFeatures._allSupportedPixelFormats;
			return supportedPixelFormats.at( _paramVideoPixelFormat->getValue() );
		}

		// Works if Host can updatePixelFormats()
		return supportedPixelFormats.at( _paramVideoPixelFormat->getValue() );
	}
	catch( const std::exception& e )
	{
		std::string supportedPixelFormatList( "" );
		for( std::vector<std::string>::iterator it = supportedPixelFormats.begin(); it != supportedPixelFormats.end(); ++it )
		{
			supportedPixelFormatList.append( *it + " " );
		}
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to get supported pixel format choosen for video codec " + videoCodecName + 
				" - Supported pixel format are: " + supportedPixelFormatList + "- " + e.what() );
	}
}

std::string AVWriterPlugin::getSampleFormatName( const std::string& audioCodecName ) const
{
	std::vector<std::string> supportedSampleFormats( avtranscoder::getSupportedSampleFormats( audioCodecName ) );

	try
	{
		// no sample format declared by the codec: return sample format selected by the user
		if( supportedSampleFormats.empty() )
		{
			supportedSampleFormats = _libavFeatures._allSupportedSampleFormats;
			return supportedSampleFormats.at(  _paramAudioSampleFormat->getValue() );
		}

		// Works if Host can updateSampleFormats()
		return supportedSampleFormats.at( _paramAudioSampleFormat->getValue() );
	}
	catch( const std::exception& e )
	{
		std::string supportedSampleFormatList( "" );
		for( std::vector<std::string>::iterator it = supportedSampleFormats.begin(); it != supportedSampleFormats.end(); ++it )
		{
			supportedSampleFormatList.append( *it + " " );
		}
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to get supported sample format choosen for audio codec " + audioCodecName + 
				" - Supported sample format are: " + supportedSampleFormatList + "- " + e.what() );
	}
}

void AVWriterPlugin::setFormatParam( const std::string& formatShortName )
{
	const common::LibAVFeaturesAvailable::NamesArray& formats( _libavFeatures._availableFormats );
	for( common::LibAVFeaturesAvailable::NamesArray::const_iterator itFormat = formats.begin();
		itFormat != formats.end();
		++itFormat )
	{
		if( itFormat->first == formatShortName )
		{
			const size_t indexFormat = std::distance( formats.begin(), itFormat );
			if( indexFormat < formats.size() )
				_paramFormat->setValue( indexFormat );
			break;
		}
	}
}

void AVWriterPlugin::setVideoCodecParam( const std::string& videoCodecShortName )
{
	const common::LibAVFeaturesAvailable::NamesArray& codecs( _libavFeatures._availableVideoCodecs );
	for( common::LibAVFeaturesAvailable::NamesArray::const_iterator itCodec = codecs.begin();
		itCodec != codecs.end();
		++itCodec )
	{
		if( itCodec->first == videoCodecShortName )
		{
			const size_t indexCodec = std::distance( codecs.begin(), itCodec );
			if( indexCodec < codecs.size() )
				_paramVideoCodec->setValue( indexCodec );
			break;
		}
	}
}

void AVWriterPlugin::setAudioCodecParam( const std::string& audioCodecShortName )
{
	const common::LibAVFeaturesAvailable::NamesArray codecs( _libavFeatures._availableAudioCodecs );
	for( common::LibAVFeaturesAvailable::NamesArray::const_iterator itCodec = codecs.begin();
		itCodec != codecs.end();
		++itCodec )
	{
		if( itCodec->first == audioCodecShortName )
		{
			const size_t indexCodec = std::distance( codecs.begin(), itCodec );
			if( indexCodec < codecs.size() )
				_paramAudioCodec->setValue( indexCodec );
			break;
		}
	}
}

void AVWriterPlugin::cleanProfile( avtranscoder::ProfileLoader::Profile& profileToClean, const std::string& prefix )
{
	// 1 - remove libav options which can make the wrapper/encoder failed if bad value
	avtranscoder::ProfileLoader::Profile::iterator itProfile = profileToClean.begin();
	while( itProfile != profileToClean.end() )
	{
		if( itProfile->second == "unknown" )
			profileToClean.erase( itProfile++ );
		else
			++itProfile;
	}

	// 2 - add libav options which can't correspond to an OFX Choice parameter's value
	size_t presetIndex = 0;
	avtranscoder::ProfileLoader::Profile originalProfile;
	if( prefix == common::kPrefixFormat )
	{
		presetIndex = _paramFormatPreset->getValue();
		if( presetIndex != 0 )
			originalProfile = _presetLoader.getFormatProfiles().at( presetIndex - 1 );
	}
	else if( prefix == common::kPrefixVideo )
	{
		presetIndex = _paramVideoPreset->getValue();
		if( presetIndex != 0 )
			originalProfile = _presetLoader.getVideoProfiles().at( presetIndex - 1 );
	}

	BOOST_FOREACH( avtranscoder::ProfileLoader::Profile::value_type& option, originalProfile )
	{
		if( profileToClean.find( option.first ) == profileToClean.end() )
		{
			profileToClean[ option.first ] = option.second;
		}
	}
}

}
}
}
}
