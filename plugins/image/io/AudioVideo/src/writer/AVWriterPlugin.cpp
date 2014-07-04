#include "AVWriterPlugin.hpp"
#include "AVWriterProcess.hpp"
#include "AVWriterDefinitions.hpp"

#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>
#include <AvTranscoder/ProgressListener.hpp>

#include <boost/foreach.hpp>

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
	, _paramVideoCustom()
	, _paramAudioCustom()
	, _paramMetadatas()
	, _outputFile( NULL )
	, _transcoder( NULL )
	, _rgbImage( NULL )
	, _imageToEncode( NULL )
	, _presets( true ) 
	, _lastOutputFilePath()
	, _initVideo( false )
	, _initWrap( false )
{
	// We want to render a sequence
	setSequentialRender( true );

	// format
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
		
		std::ostringstream audioStreamIdName( kParamAudioStreamId, std::ios_base::in | std::ios_base::ate );
		audioStreamIdName << "_" << idAudioStream;
		_paramAudioStreamIndex.push_back( fetchIntParam( audioStreamIdName.str() ) );
		_paramAudioStreamIndex.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioCopyStreamName( kParamAudioCopyStream, std::ios_base::in | std::ios_base::ate );
		audioCopyStreamName << "_" << idAudioStream;
		_paramAudioCopyStream.push_back( fetchBooleanParam( audioCopyStreamName.str() ) );
		_paramAudioCopyStream.back()->setIsSecretAndDisabled( false );
		
		std::ostringstream audioCodecPresetName( kParamAudioPreset, std::ios_base::in | std::ios_base::ate );
		audioCodecPresetName << "_" << idAudioStream;
		_paramAudioPreset.push_back( fetchChoiceParam( audioCodecPresetName.str() ) );
		_paramAudioPreset.back()->setIsSecretAndDisabled( false );
	}
	updateAudioParams();
	updateAudioSilent();
	updateAudioCopyStream();
	
	// our custom params
	_paramUseCustomFps = fetchBooleanParam( kParamUseCustomFps );
	_paramCustomFps = fetchDoubleParam( kParamCustomFps );
	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	
	// all custom params
	avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
	const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
	disableAVOptionsForCodecOrFormat( optionsFormatMap, formatName, kPrefixFormat );
	
	avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
	fetchCustomParams( optionsVideoCodecMap, kPrefixVideo );
	const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at(_paramVideoCodec->getValue() );
	disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName, kPrefixVideo );
	
	updatePixelFormat( videoCodecName );
	
	avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
	fetchCustomParams( optionsAudioCodecMap, kPrefixAudio );
	const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
	disableAVOptionsForCodecOrFormat( optionsAudioCodecMap, audioCodecName, kPrefixAudio );
	
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
	
	// av_get_pix_fmt( _paramVideoPixelFormat->getValue() )
	params._videoPixelFormat = static_cast<AVPixelFormat>( _paramVideoPixelFormat->getValue() );
	
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

void AVWriterPlugin::disableAVOptionsForCodecOrFormat( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& codec, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		std::string subGroupName = subGroupOption.first;
		std::vector<avtranscoder::Option>& options = subGroupOption.second;
				
		// iterate on options
		BOOST_FOREACH( avtranscoder::Option& option, options )
		{
			std::string name = prefix;
			name += subGroupName;
			name += "_";
			name += option.getName();
			
			switch( option.getType() )
			{
				case avtranscoder::TypeBool:
				{
					OFX::BooleanParam* curOpt = fetchBooleanParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeInt:
				{
					OFX::IntParam* curOpt = fetchIntParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeDouble:
				{
					OFX::DoubleParam* curOpt = fetchDoubleParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeString:
				{
					OFX::StringParam* curOpt = fetchStringParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeRatio:
				{
					OFX::Int2DParam* curOpt = fetchInt2DParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeChoice:
				{
					OFX::ChoiceParam* curOpt = fetchChoiceParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeGroup:
				{
					std::string groupName = prefix;
					groupName += "g_";
					groupName += subGroupName;
					groupName += "_";
					groupName += option.getName();
					
					OFX::GroupParam* curOpt = fetchGroupParam( groupName );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string childName = prefix;
						childName += "flags_";
						childName += subGroupName;
						childName += "_";
						childName += child.getName();
						
						OFX::BooleanParam* curOpt = fetchBooleanParam( childName );
						curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

/**
 * @brief Update the list of pixel format supported depending on the video codec.
 * Warning: the function does not update the list correctly in Nuke.
 * @param videoCodecName
 */
void AVWriterPlugin::updatePixelFormat( const std::string& videoCodecName )
{
	_paramVideoPixelFormat->resetOptions();
	
	std::vector<std::string> pixelsFormat( _optionLoader.getPixelFormats( videoCodecName ) );
	// get all pixel formats if the list is empty with the video codec indicated
	if( pixelsFormat.empty() )
	{
		pixelsFormat = _optionLoader.getPixelFormats();
	}
	for( std::vector<std::string>::iterator it = pixelsFormat.begin(); it != pixelsFormat.end(); ++it )
	{
		_paramVideoPixelFormat->appendOption( *it );
	}
}

void AVWriterPlugin::updateAudioParams()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		if( idAudioStream < (size_t)_paramAudioNbStream->getValue() )
		{
			_paramAudioSubGroup.at( idAudioStream )->setIsSecretAndDisabled( false );
			_paramAudioSilent.at( idAudioStream )->setIsSecretAndDisabled( false );
			_paramAudioFilePath.at( idAudioStream )->setIsSecretAndDisabled( false );
			_paramAudioStreamIndex.at( idAudioStream )->setIsSecretAndDisabled( false );
			_paramAudioCopyStream.at( idAudioStream )->setIsSecretAndDisabled( false );
			_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( false );
		}
		else
		{
			_paramAudioSubGroup.at( idAudioStream )->setIsSecretAndDisabled( true );
			_paramAudioSilent.at( idAudioStream )->setIsSecretAndDisabled( true );
			_paramAudioFilePath.at( idAudioStream )->setIsSecretAndDisabled( true );
			_paramAudioStreamIndex.at( idAudioStream )->setIsSecretAndDisabled( true );
			_paramAudioCopyStream.at( idAudioStream )->setIsSecretAndDisabled( true );
			_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( true );
		}
	}
}

void AVWriterPlugin::updateAudioCopyStream()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		if( _paramAudioSubGroup.at( idAudioStream )->getIsEnable() &&
			! _paramAudioSubGroup.at( idAudioStream )->getIsSecret() )
		{
			// if copy stream
			if( _paramAudioCopyStream.at( idAudioStream )->getValue() )
			{
				_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( true );
			}
			else
			{
				_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( false );
			}
		}
	}
}

void AVWriterPlugin::updateAudioSilent()
{
	for( size_t idAudioStream = 0; idAudioStream < maxNbAudioStream; ++idAudioStream )
	{
		if( _paramAudioSubGroup.at( idAudioStream )->getIsEnable() &&
			! _paramAudioSubGroup.at( idAudioStream )->getIsSecret() )
		{
			// if silent stream
			if( _paramAudioSilent.at( idAudioStream )->getValue() )
			{
				_paramAudioFilePath.at( idAudioStream )->setIsSecretAndDisabled( true );
				_paramAudioStreamIndex.at( idAudioStream )->setIsSecretAndDisabled( true );
				_paramAudioCopyStream.at( idAudioStream )->setIsSecretAndDisabled( true );
				_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( true );
			}
			else
			{
				_paramAudioFilePath.at( idAudioStream )->setIsSecretAndDisabled( false );
				_paramAudioStreamIndex.at( idAudioStream )->setIsSecretAndDisabled( false );
				_paramAudioCopyStream.at( idAudioStream )->setIsSecretAndDisabled( false );
				if( ! _paramAudioCopyStream.at( idAudioStream )->getValue() )
					_paramAudioPreset.at( idAudioStream )->setIsSecretAndDisabled( false );
			}
		}
	}
}

void AVWriterPlugin::fetchCustomParams( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& prefix )
{
	common::CustomParams* customParams;
	if( prefix == kPrefixVideo )
		customParams = &_paramVideoCustom;
	if( prefix == kPrefixAudio )
		customParams = &_paramAudioCustom;
	else
		return;
	
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		std::string subGroupName = subGroupOption.first;
		std::vector<avtranscoder::Option>& options = subGroupOption.second;
				
		// iterate on options
		BOOST_FOREACH( avtranscoder::Option& option, options )
		{
			std::string name = prefix;
			name += subGroupName;
			name += "_";
			name += option.getName();
			
			switch( option.getType() )
			{
				case avtranscoder::TypeBool:
				{
					customParams->_paramBoolean.push_back( fetchBooleanParam( name ) );
					break;
				}
				case avtranscoder::TypeInt:
				{
					customParams->_paramInt.push_back( fetchIntParam( name ) );
					break;
				}
				case avtranscoder::TypeDouble:
				{
					customParams->_paramDouble.push_back( fetchDoubleParam( name ) );
					break;
				}
				case avtranscoder::TypeString:
				{
					customParams->_paramString.push_back( fetchStringParam( name ) );
					break;
				}
				case avtranscoder::TypeRatio:
				{
					customParams->_paramRatio.push_back( fetchInt2DParam( name ) );
					break;
				}
				case avtranscoder::TypeChoice:
				{
					customParams->_paramChoice.push_back( fetchChoiceParam( name ) );
					break;
				}
				case avtranscoder::TypeGroup:
				{
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string childName = prefix;
						childName += "flags_";
						childName += subGroupName;
						childName += "_";
						childName += child.getName();
						
						customParams->_paramBoolean.push_back( fetchBooleanParam( childName ) );
					}
					break;
				}
			default:
					break;
			}
		}
	}
}

void AVWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
	
	if( paramName == kParamFormat )
	{
		avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
		const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
		disableAVOptionsForCodecOrFormat( optionsFormatMap, formatName, kPrefixFormat );
	}
	else if( paramName == kParamVideoCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
		const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at( _paramVideoCodec->getValue() );
		disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName, kPrefixVideo );
		
		updatePixelFormat( videoCodecName );
	}
	else if( paramName == kParamAudioCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
		const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
		disableAVOptionsForCodecOrFormat( optionsAudioCodecMap, audioCodecName, kPrefixAudio );
	}
	else if( paramName == kParamMainPreset )
	{
		if( _paramMainPreset->getValue() == 0 )
			return;
	}
	else if( paramName == kParamFormatPreset )
	{
		if( _paramFormatPreset->getValue() == 0 )
			return;
	}
	else if( paramName == kParamMainVideoPreset )
	{
		// if custom preset
		if( _paramMainVideoPreset->getValue() == 0 )
		{	
			int defaultVideoCodecIndex;
			_paramVideoCodec->getDefault( defaultVideoCodecIndex );
			_paramVideoCodec->setValue( defaultVideoCodecIndex );
			
			_paramVideoCustomGroup->setIsSecretAndDisabled( false );
		}
		else
		{
			int videoCodecWithNoOptionDetailsIndex = 0;
			_paramVideoCodec->setValue( videoCodecWithNoOptionDetailsIndex );
			
			_paramVideoCustomGroup->setIsSecretAndDisabled( true );
		}
	}
	else if( paramName == kParamMainAudioPreset )
	{
		// if custom preset
		if( _paramMainAudioPreset->getValue() == 0 )
		{
			_paramAudioCustomGroup->setIsSecretAndDisabled( false );
		}
		else
		{
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
		updateAudioSilent();
		updateAudioCopyStream();
	}
	else if( paramName.find( kParamAudioCopyStream ) != std::string::npos )
	{
		updateAudioCopyStream();
	}
	else if( paramName.find( kParamAudioSilent ) != std::string::npos )
	{
		updateAudioSilent();
	}
}

void AVWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// Need to be computed at each frame
	clipPreferences.setOutputFrameVarying( true );
}

void AVWriterPlugin::initOutput( AVProcessParams& params )
{
	// create output file
	try
	{
		_outputFile.reset( new avtranscoder::OutputFile( params._outputFilePath ) );
		_outputFile->setup();
		
		_transcoder.reset( new avtranscoder::Transcoder( *_outputFile ) );
	}
	catch( std::exception& e )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open output file: " + e.what()
		    << exception::filename( params._outputFilePath ) );
	}
}

void AVWriterPlugin::ensureVideoIsInit( const OFX::RenderArguments& args, AVProcessParams& params )
{
	// ouput file path already set
	if( _outputFile.get() == NULL ||
		( _lastOutputFilePath != "" && _lastOutputFilePath == params._outputFilePath ) )
		return;
	
	if( params._outputFilePath == "" ) // no output file indicated
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "no output file indicated"
		    << exception::filename( params._outputFilePath ) );
	}
	
	_lastOutputFilePath = params._outputFilePath;
	
	// create video stream
	try
	{
		const OfxRectI bounds = _clipSrc->getPixelRod( args.time, args.renderScale );
		int width = bounds.x2 - bounds.x1;
		int height = bounds.y2 - bounds.y1;

		avtranscoder::ImageDesc imageDesc;
		avtranscoder::Pixel pixel( params._videoPixelFormat );
		// @todo: avTranscoder, set par of pixel
		imageDesc.setPixel( pixel );
		imageDesc.setWidth( width );
		imageDesc.setHeight( height );
		imageDesc.setDar( width, height );

		size_t mainPresetIndex = _paramMainVideoPreset->getValue();
		
		// custom video preset
		if( mainPresetIndex == 0 )
		{
			avtranscoder::Profile::ProfileDesc customPreset;
			customPreset[ avtranscoder::Profile::avProfileIdentificator ] = "customVideoPreset";
			customPreset[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Custom video preset";
			customPreset[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeVideo;
			
			customPreset[ avtranscoder::Profile::avProfileCodec ] = params._videoCodecName;
			customPreset[ avtranscoder::Profile::avProfilePixelFormat ] = boost::to_string( params._videoPixelFormat );
			
			if( _paramUseCustomFps->getValue() )
			{
				customPreset[ avtranscoder::Profile::avProfileFrameRate ] = boost::to_string( _paramCustomFps->getValue() );
			}
			else
			{
				customPreset[ avtranscoder::Profile::avProfileFrameRate ] = boost::to_string( _clipSrc->getFrameRate() );
			}
			
			customPreset[ "aspect" ] = boost::to_string( _clipSrc->getPixelAspectRatio() );
			
			common::CustomParams::OptionsForPreset optionsForPreset = _paramVideoCustom.getOptionsNameAndValue( params._videoCodecName );
			BOOST_FOREACH( common::CustomParams::OptionForPreset nameAndValue, optionsForPreset )
			{
				customPreset[ nameAndValue.first ] = nameAndValue.second;
			}
			
			_outputStreamVideo.setProfile( customPreset, imageDesc );
		}
		// existing video preset
		else
		{
			// at( mainPresetIndex - 1 ): subtract the index of the custom preset
			std::string presetName( _presets.getVideoProfiles().at( mainPresetIndex - 1 ).find( avtranscoder::Profile::avProfileIdentificator )->second );
			_outputStreamVideo.setProfile( _presets.getProfile( presetName ), imageDesc );
		}
		
		_outputFile->addVideoStream( _outputStreamVideo.getVideoDesc() );
	}	
	catch( std::exception& e )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to create video stream: " + e.what() );
	}
	_initVideo = true;
}

void AVWriterPlugin::initAudio( AVProcessParams& params )
{
	if( ! _paramAudioNbStream->getValue() ) // no audio specified
		return;
	
	// create audio streams
	try
	{
		for( int i = 0; i < _paramAudioNbStream->getValue(); ++i )
		{
			std::string inputFileName( _paramAudioFilePath.at( i )->getValue() );
			int inputStreamIndex = _paramAudioStreamIndex.at( i )->getValue();
			std::string presetName( "" );
			
			size_t mainPresetIndex = _paramMainAudioPreset->getValue();
			avtranscoder::Profile::ProfileDesc customPreset;
			// custom audio preset
			if( mainPresetIndex == 0 )
			{
				customPreset[ avtranscoder::Profile::avProfileIdentificator ] = "customAudioPreset";
				customPreset[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Custom audio preset";
				customPreset[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeAudio;
				customPreset[ avtranscoder::Profile::avProfileCodec ] = params._audioCodecName;
				// @todo: get this from OFX params
				customPreset[ avtranscoder::Profile::avProfileSampleFormat ] = "s16";
				customPreset[ avtranscoder::Profile::avProfileSampleRate ] = "48000";
				customPreset[ avtranscoder::Profile::avProfileChannel ] = "1";
				
				common::CustomParams::OptionsForPreset optionsForPreset = _paramAudioCustom.getOptionsNameAndValue( params._audioCodecName );
				BOOST_FOREACH( common::CustomParams::OptionForPreset nameAndValue, optionsForPreset )
				{
					customPreset[ nameAndValue.first ] = nameAndValue.second;
				}
			}
			
			// dummy
			if( _paramAudioSilent.at( i )->getValue() )
			{
				// custom audio preset
				if( mainPresetIndex == 0 )
				{
					_transcoder->add( "", 0, customPreset );
				}
				else
				{
					// at( mainPresetIndex - 1 ): subtract the index of the custom preset
					_transcoder->add( "", 0,  _presets.getAudioProfiles().at( mainPresetIndex - 1 ) );
				}
			}
			else
			{
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
						_transcoder->add( inputFileName, inputStreamIndex, presetName );
					}
					else
					{
						for( size_t streamIndex = 0; streamIndex < nbStream; ++streamIndex )
						{
							_transcoder->add( inputFileName, streamIndex, presetName );
						}
					}
				}
				// transcode
				else
				{
					size_t presetIndex = _paramAudioPreset.at( i )->getValue();

					// custom audio preset
					if( presetIndex == 0 ||
						( presetIndex == 1 && mainPresetIndex == 0 ) )
					{
						if( inputStreamIndex != -1 )
						{
							_transcoder->add( inputFileName, inputStreamIndex, customPreset );
						}
						else
						{
							for( size_t streamIndex = 0; streamIndex < nbStream; ++streamIndex )
							{
								_transcoder->add( inputFileName, inputStreamIndex, customPreset );
							}
						}
					}
					// existing audio preset
					else
					{
						// main audio preset
						if( presetIndex == 1 )
						{
							// at( mainPresetIndex - 1 ): subtract the index of the custom preset
							presetName = _presets.getAudioProfiles().at( mainPresetIndex - 1 ).find( avtranscoder::Profile::avProfileIdentificator )->second;
						}
						// specific audio preset
						else
						{
							// at( presetIndex - 2 ): subtract the index of the custom preset + the index of the main preset
							presetName = _presets.getAudioProfiles().at( presetIndex - 2 ).find( avtranscoder::Profile::avProfileIdentificator )->second;
						}
						if( inputStreamIndex != -1 )
						{
							_transcoder->add( inputFileName, inputStreamIndex, presetName );
						}
						else
						{
							for( size_t streamIndex = 0; streamIndex < nbStream; ++streamIndex )
							{
								_transcoder->add( inputFileName, inputStreamIndex, presetName );
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

void AVWriterPlugin::cleanVideoAndAudio()
{
	_outputFile.reset();

	// clean video
	_rgbImage.reset();
	_imageToEncode.reset();
	
	// clean audio
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
	
	AVProcessParams params = getProcessParams();
	initOutput( params );
	initAudio( params );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AVWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	AVProcessParams params = getProcessParams();
	ensureVideoIsInit( args, params );

	if( ! _initWrap )
	{
		_outputFile->beginWrap();
		_initWrap = true;
	}
	
	doGilRender<AVWriterProcess>( *this, args );
}

void AVWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{	
	if( ! _initWrap || ! _initVideo )
		return;
	
	// if video latency
	avtranscoder::DataStream codedImage;
	while( _outputStreamVideo.encodeFrame( codedImage ) )
	{
		_outputFile->wrap( codedImage, 0 );
	}
	
	_outputFile->endWrap();
}

}
}
}
}
