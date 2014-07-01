#include "AVWriterPlugin.hpp"
#include "AVWriterProcess.hpp"
#include "AVWriterDefinitions.hpp"

#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/VideoDesc.hpp>

#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>
#include <AvTranscoder/ProgressListener.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixdesc.h>
}

#include <boost/gil/gil_all.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <cctype>
#include <sstream>
#include <utility> // pair

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

void AVWriterPlugin::disableAVOptionsForCodecOrFormat( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& codec )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		std::string subGroupName = subGroupOption.first;
		std::vector<avtranscoder::Option>& options = subGroupOption.second;
				
		// iterate on options
		BOOST_FOREACH( avtranscoder::Option& option, options )
		{
			std::string name = subGroupName;
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
					std::string groupName = "g_";
					groupName += subGroupName;
					groupName += "_";
					groupName += option.getName();
					
					OFX::GroupParam* curOpt = fetchGroupParam( groupName );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string ChildName = "flags_";
						ChildName += subGroupName;
						ChildName += "_";
						ChildName += child.getName();
						
						OFX::BooleanParam* curOpt = fetchBooleanParam( ChildName );
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

AVWriterPlugin::AVWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
	, _paramAudioSubGroup()
	, _paramAudioSilent()
	, _paramAudioFilePath()
	, _paramAudioStreamIndex()
	, _paramAudioCopyStream()
	, _paramAudioPreset()
	, _paramMetadatas()
	, _outputFile( NULL )
	, _transcoder( NULL )
	, _rgbImage( NULL )
	, _imageToEncode( NULL )
	, _presets( true ) 
	, _lastOutputFilePath()
	, _initVideo( false )
	, _initAudio( false )
	, _initOutpuFile( false )
{
	// We want to render a sequence
	setSequentialRender( true );

	_paramFormat     = fetchChoiceParam( kParamFormat );
	_paramVideoCodec = fetchChoiceParam( kParamVideoCodec );
	_paramAudioCodec = fetchChoiceParam( kParamAudioCodec );
	
	_paramMainPreset       = fetchChoiceParam( kParamMainPreset );
	_paramFormatPreset     = fetchChoiceParam( kParamFormatPreset );
	_paramMainVideoPreset = fetchChoiceParam( kParamMainVideoPreset );
	_paramMainAudioPreset = fetchChoiceParam( kParamMainAudioPreset );
	
	_paramUseCustomFps     = fetchBooleanParam( kParamUseCustomFps );
	_paramCustomFps        = fetchDoubleParam( kParamCustomFps );
	
	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	
	_paramAudioCustomGroup = fetchGroupParam( kParamAudioCustomGroup );
	
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
	
	avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
	const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
	disableAVOptionsForCodecOrFormat( optionsFormatMap, formatName );
	
	avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
	const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at(_paramVideoCodec->getValue() );
	disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName );
	
	updatePixelFormat( videoCodecName );
	
	avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
	const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
	disableAVOptionsForCodecOrFormat( optionsAudioCodecMap, audioCodecName );
	
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

void AVWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
	
	if( paramName == kParamFormat )
	{
		avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
		const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
		disableAVOptionsForCodecOrFormat( optionsFormatMap, formatName );
	}
	else if( paramName == kParamVideoCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
		const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at( _paramVideoCodec->getValue() );
		disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName );
		
		updatePixelFormat( videoCodecName );
	}
	else if( paramName == kParamAudioCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
		const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
		disableAVOptionsForCodecOrFormat( optionsAudioCodecMap, audioCodecName );
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
		if( _paramMainVideoPreset->getValue() == 0 )
			return;
	}
	else if( paramName == kParamMainAudioPreset )
	{
		// if custom preset
		if( _paramMainAudioPreset->getValue() == 0 )
		{
			_paramAudioCustomGroup->setIsSecretAndDisabled( false );
			return;
		}
		_paramAudioCustomGroup->setIsSecretAndDisabled( true );
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

void AVWriterPlugin::ensureVideoIsInit( const OFX::RenderArguments& args, AVProcessParams& params )
{
	// ouput file path already set
	if( _lastOutputFilePath != "" &&
		_lastOutputFilePath == params._outputFilePath )
		return;
	
	if( params._outputFilePath == "" ) // no output file indicated
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "no output file indicated"
		    << exception::filename( params._outputFilePath ) );
	}
	
	// create output file
	try
	{
		_outputFile.reset( new avtranscoder::OutputFile( params._outputFilePath ) );
		_outputFile->setup();
		
		_transcoder.reset( new avtranscoder::Transcoder( *_outputFile ) );
		
		_lastOutputFilePath = params._outputFilePath;
	}
	catch( std::exception& e )
	{
		_initVideo = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open output file: " + e.what()
		    << exception::filename( params._outputFilePath ) );
	}
	
	// create video stream
	try
	{
		size_t mainPresetIndex = _paramMainVideoPreset->getValue();
		
		// custom video preset
		if( mainPresetIndex == 0 )
		{			
			avtranscoder::VideoDesc& videoOutputDesc = _outputStreamVideo.getVideoDesc();
			
			videoOutputDesc.setVideoCodec( params._videoCodecName );

			const OfxRectI bounds = _clipSrc->getPixelRod( args.time, args.renderScale );
			int width = bounds.x2 - bounds.x1;
			int height = bounds.y2 - bounds.y1;

			avtranscoder::ImageDesc imageDesc;
			avtranscoder::Pixel pixel( params._videoPixelFormat );
			imageDesc.setPixel( pixel );
			imageDesc.setWidth( width );
			imageDesc.setHeight( height );
			imageDesc.setDar( _clipSrc->getPixelAspectRatio(), 1 );
			videoOutputDesc.setImageParameters( imageDesc );
			
			if( _paramUseCustomFps->getValue() )
			{
				videoOutputDesc.setTimeBase( 1, _paramCustomFps->getValue() );
			}
			else
			{
				videoOutputDesc.setTimeBase( 1, _clipSrc->getFrameRate() );
			}
			
			if( ! _outputStreamVideo.setup( ) )
			{
				throw std::runtime_error( "error during initialising video output stream" );
			}
		}
		// existing video preset
		else
		{
			// at( mainPresetIndex - 1 ): subtract the index of the custom preset
			std::string presetName( _presets.getVideoProfiles().at( mainPresetIndex - 1 ).find( avtranscoder::Profile::avProfileIdentificator )->second );
			_outputStreamVideo.setProfile( _presets.getProfile( presetName ) );
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

void AVWriterPlugin::ensureAudioIsInit( AVProcessParams& params )
{
	if( ! _paramAudioNbStream->getValue() ) // no audio specified
	{
		_initAudio = false;
		return;
	}
	
	// create audio streams
	try
	{
		for( int i = 0; i < _paramAudioNbStream->getValue(); ++i )
		{
			std::string inputFileName( _paramAudioFilePath.at( i )->getValue() );
			int inputStreamIndex = _paramAudioStreamIndex.at( i )->getValue();
			
			// dummy
			if( _paramAudioSilent.at( i )->getValue() )
			{
				// @todo: avTranscoder ; can set a profile to a dummy (and so can have a dummy without an existing audio stream)
				_transcoder->add( "", 0, "" );
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
				std::string presetName( "" );
				
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
					size_t mainPresetIndex = _paramMainAudioPreset->getValue();
					size_t presetIndex = _paramAudioPreset.at( i )->getValue();

					// custom audio preset
					if( presetIndex == 0 ||
						( presetIndex == 1 && mainPresetIndex == 0 ) )
					{
						avtranscoder::Profile::ProfileDesc customPreset;
						customPreset[ avtranscoder::Profile::avProfileIdentificator ] = "customPreset";
						customPreset[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Custom preset";
						customPreset[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeAudio;
						// @todo: get it from OFX params
						customPreset[ "codec" ] = params._audioCodecName;
						customPreset[ "sample_fmt" ] = "s16";

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
		_initAudio = false;
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to create audio stream: " + e.what() );
	}
	
	_initAudio = true;
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
	_initAudio = false;
	_initOutpuFile = false;
}

/**
 * @brief The overridden begin render function
 * @param[in]   args     Begin Rendering parameters
 */
void AVWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	// Before new render
	cleanVideoAndAudio();
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
	ensureAudioIsInit( params );

	if( ! _initOutpuFile )
	{
		_outputFile->beginWrap();
		_initOutpuFile = true;
	}
	
	doGilRender<AVWriterProcess>( *this, args );
}

void AVWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{	
	if( ! _initOutpuFile || ! _initVideo )
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
