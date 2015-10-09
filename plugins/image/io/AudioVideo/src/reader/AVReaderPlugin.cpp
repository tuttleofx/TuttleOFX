#include "AVReaderPlugin.hpp"
#include "AVReaderProcess.hpp"
#include "AVReaderDefinitions.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/file/FormatContext.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/mediaProperty/FileProperties.hpp>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <stdexcept>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

using namespace boost::gil;
namespace fs = boost::filesystem;

AVReaderPlugin::AVReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
	, _paramFormatCustom( common::kPrefixFormat, AV_OPT_FLAG_DECODING_PARAM, false )
	, _paramVideoCustom( common::kPrefixVideo, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, false )
	, _paramMetaDataCustom( common::kPrefixMetaData, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_METADATA, false )
	, _paramFormatDetailCustom( common::kPrefixFormat, AV_OPT_FLAG_DECODING_PARAM, true )
	, _paramVideoDetailCustom( common::kPrefixVideo, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, true )
	, _inputFile( NULL )
	, _inputStream( NULL )
	, _inputDecoder( NULL )
	, _sourceImage( NULL )
	, _imageToDecode( NULL )
	, _lastInputFilePath( "" )
	, _lastVideoStreamIndex( 0 )
	, _lastFrame( -1 )
	, _initVideo( false )
{
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramVideoStreamIndex = fetchIntParam( kParamVideoStreamIndex );
	_paramUseCustomSAR = fetchBooleanParam( kParamUseCustomSAR );
	_paramCustomSAR = fetchDoubleParam( kParamCustomSAR );

	_paramFormatCustom.fetchLibAVParams( *this, common::kPrefixFormat );
	_paramVideoCustom.fetchLibAVParams( *this, common::kPrefixVideo );
	_paramMetaDataCustom.fetchLibAVParams( *this, common::kPrefixMetaData );
	
	_paramFormatDetailCustom.fetchLibAVParams( *this, common::kPrefixFormat );
	_paramVideoDetailCustom.fetchLibAVParams( *this, common::kPrefixVideo );
	
	_paramMetaDataWrapper = fetchStringParam( kParamMetaDataWrapper );
	_paramMetaDataWrapper->setIsSecret( true );
	_paramMetaDataVideo = fetchStringParam( kParamMetaDataVideo );
	_paramMetaDataVideo->setIsSecret( true );
	_paramMetaDataAudio = fetchStringParam( kParamMetaDataAudio );
	_paramMetaDataAudio->setIsSecret( true );
	_paramMetaDataData = fetchStringParam( kParamMetaDataData );
	_paramMetaDataData->setIsSecret( true );
	_paramMetaDataSubtitle = fetchStringParam( kParamMetaDataSubtitle );
	_paramMetaDataSubtitle->setIsSecret( true );
	_paramMetaDataAttachement = fetchStringParam( kParamMetaDataAttachement );
	_paramMetaDataAttachement->setIsSecret( true );
	_paramMetaDataUnknown = fetchStringParam( kParamMetaDataUnknown );
	_paramMetaDataUnknown->setIsSecret( true );

	_paramVerbose = fetchBooleanParam( kParamVerbose );

	const avtranscoder::OptionArrayMap optionsFormatDetailMap = avtranscoder::getOutputFormatOptions();
	common::disableOFXParamsForFormatOrCodec( *this, optionsFormatDetailMap, "", common::kPrefixFormat );

	const avtranscoder::OptionArrayMap optionsVideoCodecMap = avtranscoder::getVideoCodecOptions();
	common::disableOFXParamsForFormatOrCodec( *this, optionsVideoCodecMap, "", common::kPrefixVideo );

	updateVisibleTools();
}

void AVReaderPlugin::ensureVideoIsOpen()
{
	const std::string& filepath = _paramFilepath->getValue();
	const size_t videoStreamIndex = _paramVideoStreamIndex->getValue();

	// if the given file is already opened
	if( _lastInputFilePath == filepath &&
	    ! _lastInputFilePath.empty() &&
	    _lastVideoStreamIndex == videoStreamIndex )
		return;

	// if the given file does not exist
	if( filepath == "" || ! boost::filesystem::exists( filepath ) )
	{
		cleanInputFile();
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
		    << exception::filename( filepath ) );
	}
	
	try
	{
		// set and analyse inputFile
		_inputFile.reset( new avtranscoder::InputFile( filepath ) );
		_lastInputFilePath = filepath;

		// launch deep analyse to get info of first gop
		avtranscoder::NoDisplayProgress progress;
		_inputFile->analyse( progress, avtranscoder::eAnalyseLevelFirstGop );
		
		// get index of the video stream
		if( videoStreamIndex >= _inputFile->getProperties().getVideoProperties().size() )
		{
			throw std::runtime_error( "the stream index doesn't exist in the input file" );
		}
		_lastVideoStreamIndex = videoStreamIndex;
		
		// buffered the selected video stream
		_inputStream = &_inputFile->getStream( _inputFile->getProperties().getVideoProperties().at(videoStreamIndex).getStreamIndex() );
		_inputStream->activate();
		
		// set video decoder
		_inputDecoder.reset( new avtranscoder::VideoDecoder( *_inputStream ) );
	}
	catch( std::exception& e )
	{
		cleanInputFile();
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open input file : " + e.what()
		    << exception::filename( filepath ) );
	}
	_initVideo = true;
}

void AVReaderPlugin::cleanInputFile()
{
	_inputFile.reset();
	_inputDecoder.reset();
	_sourceImage.reset();
	_imageToDecode.reset();
	_lastInputFilePath = "";
	_lastVideoStreamIndex = 0;
	_initVideo = false;
}

void AVReaderPlugin::updateVisibleTools()
{
	OFX::InstanceChangedArgs args( this->timeLineGetTime() );
	changedParam( args, kParamUseCustomSAR );
}

AVReaderParams AVReaderPlugin::getProcessParams() const
{
	AVReaderParams params;

	params._filepath = _paramFilepath->getValue();
	params._inputProperties = &_inputFile->getProperties();

	std::istringstream formats( params._inputProperties->getFormatName() );
	if( formats.rdbuf()->in_avail() )
	{
		// use first format defined in the file to set the list of formats
		std::getline( formats, params._inputFormatName, ',' );
	}

	params._inputVideoProperties = &params._inputProperties->getVideoProperties().at( _paramVideoStreamIndex->getValue() );

	return params;
}

void AVReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	ReaderPlugin::changedParam( args, paramName );
	
	if( paramName == kParamUseCustomSAR )
	{
		const bool useCustomSAR = _paramUseCustomSAR->getValue();
		_paramCustomSAR->setIsSecretAndDisabled( !useCustomSAR );
	}
	else if( paramName == kParamCustomSAR )
	{
		const bool useCustomSAR = _paramUseCustomSAR->getValue();
		if( ! useCustomSAR )
		{
			// If we set a customSAR and the useCustomSAR param is not enabled,
			// use it directly!
			// This could not happen in a UI application, but only from command line or from API.
			_paramUseCustomSAR->setValue( true );
		}
	}
	else if( paramName == kTuttlePluginFilename )
	{
		typedef std::pair<std::string, std::string> PropertyPair;
		try
		{
			ensureVideoIsOpen();

			AVReaderParams params = getProcessParams();

			// set range of the OFX param
			_paramVideoStreamIndex->setRange( 0, params._inputProperties->getVideoProperties().size() );
			_paramVideoStreamIndex->setDisplayRange( 0, params._inputProperties->getVideoProperties().size() );

			// update wrapper of Metadata tab
			std::string wrapperValue( "" );
			BOOST_FOREACH( const PropertyPair& pair, params._inputProperties->getPropertiesAsVector() )
			{
				wrapperValue += pair.first + ": " + pair.second + "\n";
			}
			if( ! wrapperValue.empty() )
			{
				_paramMetaDataWrapper->setIsSecret( false );
				_paramMetaDataWrapper->setValue( wrapperValue );
			}

			// update video of Metadata tab
			std::string videoValue( "" );
			BOOST_FOREACH( const avtranscoder::VideoProperties& videoStream, params._inputProperties->getVideoProperties() )
			{
				videoValue += "::::: VIDEO STREAM ::::: \n";
				BOOST_FOREACH( const PropertyPair& pair, videoStream.getPropertiesAsVector() )
				{
					videoValue += pair.first + ": " + pair.second + "\n";
				}
				videoValue += "\n\n";
			}
			if( ! videoValue.empty() )
			{
				_paramMetaDataVideo->setIsSecret( false );
				_paramMetaDataVideo->setValue( videoValue );
			}

			// update audio of Metadata tab
			std::string audioValue( "" );
			BOOST_FOREACH( const avtranscoder::AudioProperties& audioStream, params._inputProperties->getAudioProperties() )
			{
				audioValue += "::::: AUDIO STREAM ::::: \n";
				BOOST_FOREACH( const PropertyPair& pair, audioStream.getPropertiesAsVector() )
				{
					audioValue += pair.first + ": " + pair.second + "\n";
				}
				audioValue += "\n\n";
			}
			if( ! audioValue.empty() )
			{
				_paramMetaDataAudio->setIsSecret( false );
				_paramMetaDataAudio->setValue( audioValue );
			}
			
			// update data of Metadata tab
			std::string dataValue( "" );
			BOOST_FOREACH( const avtranscoder::DataProperties& dataStream, params._inputProperties->getDataProperties() )
			{
				dataValue += "::::: DATA STREAM ::::: \n";
				BOOST_FOREACH( const PropertyPair& pair, dataStream.getPropertiesAsVector() )
				{
					dataValue += pair.first + ": " + pair.second + "\n";
				}
				dataValue += "\n\n";
			}
			if( ! dataValue.empty() )
			{
				_paramMetaDataData->setIsSecret( false );
				_paramMetaDataData->setValue( dataValue );
			}
			
			// update subtitle of Metadata tab
			std::string subtitleValue( "" );
			BOOST_FOREACH( const avtranscoder::SubtitleProperties& subtitleStream, params._inputProperties->getSubtitleProperties() )
			{
				subtitleValue += "::::: SUBTITLE STREAM ::::: \n";
				BOOST_FOREACH( const PropertyPair& pair, subtitleStream.getPropertiesAsVector() )
				{
					subtitleValue += pair.first + ": " + pair.second + "\n";
				}
				subtitleValue += "\n\n";
			}
			if( ! subtitleValue.empty() )
			{
				_paramMetaDataSubtitle->setIsSecret( false );
				_paramMetaDataSubtitle->setValue( subtitleValue );
			}
			
			// update attachement of Metadata tab
			std::string attachementValue( "" );
			BOOST_FOREACH( const avtranscoder::AttachementProperties& attachementStream, params._inputProperties->getAttachementProperties() )
			{
				attachementValue += "::::: ATTACHEMENT STREAM ::::: \n";
				BOOST_FOREACH( const PropertyPair& pair, attachementStream.getPropertiesAsVector() )
				{
					attachementValue += pair.first + ": " + pair.second + "\n";
				}
				attachementValue += "\n\n";
			}
			if( ! attachementValue.empty() )
			{
				_paramMetaDataAttachement->setIsSecret( false );
				_paramMetaDataAttachement->setValue( attachementValue );
			}
			
			// update unknown of Metadata tab
			std::string unknownValue( "" );
			BOOST_FOREACH( const avtranscoder::UnknownProperties& unknownStream, params._inputProperties->getUnknownProperties() )
			{
				unknownValue += "::::: UNKNOWN STREAM ::::: \n";
				BOOST_FOREACH( const PropertyPair& pair, unknownStream.getPropertiesAsVector() )
				{
					unknownValue += pair.first + ": " + pair.second + "\n";
				}
				unknownValue += "\n\n";
			}
			if( ! unknownValue.empty() )
			{
				_paramMetaDataUnknown->setIsSecret( false );
				_paramMetaDataUnknown->setValue( unknownValue );
			}

			// update format details parameters
			avtranscoder::OptionArrayMap optionsFormatMap = avtranscoder::getOutputFormatOptions();
			common::disableOFXParamsForFormatOrCodec( *this, optionsFormatMap, params._inputFormatName, common::kPrefixFormat );
			
			// update video details parameters
			avtranscoder::OptionArrayMap optionsVideoCodecMap = avtranscoder::getVideoCodecOptions();
			const std::string videoCodecName = params._inputVideoProperties->getCodecName();
			common::disableOFXParamsForFormatOrCodec( *this, optionsVideoCodecMap, videoCodecName, common::kPrefixVideo );
		}
		catch( std::exception& e )
		{
			_paramVideoStreamIndex->setRange( 0, 100. );
			_paramVideoStreamIndex->setDisplayRange( 0, 16 );

			_paramMetaDataWrapper->setIsSecret( true );
			_paramMetaDataVideo->setIsSecret( true );
			_paramMetaDataAudio->setIsSecret( true );
			_paramMetaDataData->setIsSecret( true );
			_paramMetaDataSubtitle->setIsSecret( true );
			_paramMetaDataAttachement->setIsSecret( true );
			_paramMetaDataUnknown->setIsSecret( true );

			avtranscoder::OptionArrayMap optionsFormatMap = avtranscoder::getOutputFormatOptions();
			common::disableOFXParamsForFormatOrCodec( *this, optionsFormatMap, "", common::kPrefixFormat );
			
			avtranscoder::OptionArrayMap optionsVideoCodecMap = avtranscoder::getVideoCodecOptions();
			common::disableOFXParamsForFormatOrCodec( *this, optionsVideoCodecMap, "", common::kPrefixVideo );
		}
	}
	else if( paramName == kParamVerbose )
	{
		if( _paramVerbose->getValue() )
			avtranscoder::Logger::setLogLevel( AV_LOG_DEBUG );
		else
			avtranscoder::Logger::setLogLevel( AV_LOG_QUIET );
	}
}

double AVReaderPlugin::retrievePAR()
{
	if( ! OFX::getImageEffectHostDescription()->supportsMultipleClipPARs )
		return 1.0;
	
	double par = 1.0;
	if( _paramUseCustomSAR->getValue() )
	{
		par = _paramCustomSAR->getValue();
	}
	else
	{
		AVReaderParams params = getProcessParams();
		const avtranscoder::Rational sar = params._inputVideoProperties->getSar();
		par = sar.num / (double)sar.den;
	}

	if( par == 0.0 )
		return 1.0;

	return par;
}

void AVReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ensureVideoIsOpen();
	
	ReaderPlugin::getClipPreferences( clipPreferences );

	AVReaderParams params = getProcessParams();

	// conversion of bitdepth
	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
	{
		size_t bitDepth = params._inputVideoProperties->getPixelProperties().getBitsPerPixel();
		OFX::EBitDepth fileBitDepth;
		if( bitDepth == 0 )
			fileBitDepth = OFX::eBitDepthNone;
		if( bitDepth == 8)
			fileBitDepth = OFX::eBitDepthUByte;
		else if( bitDepth == 16)
			fileBitDepth = OFX::eBitDepthUShort;
		else if( bitDepth == 24)
			fileBitDepth = OFX::eBitDepthFloat;
		else
			fileBitDepth = OFX::eBitDepthCustom;

		if( OFX::getImageEffectHostDescription()->supportsBitDepth( fileBitDepth ) )
			clipPreferences.setClipBitDepth( *_clipDst, fileBitDepth );
		else
			clipPreferences.setClipBitDepth( *_clipDst, OFX::getImageEffectHostDescription()->getDefaultPixelDepth() );
	}
	
	// conversion of channel
	if( getExplicitChannelConversion() == eParamReaderChannelAuto )
	{
		size_t nbComponents = params._inputVideoProperties->getPixelProperties().getNbComponents();
		OFX::EPixelComponent filePixelComponent;
		if( nbComponents == 0 )
			filePixelComponent = OFX::ePixelComponentNone;
		else if( nbComponents == 1 )
			filePixelComponent = OFX::ePixelComponentAlpha;
		else if( nbComponents == 3 )
			filePixelComponent = OFX::ePixelComponentRGB;
		else if( nbComponents == 4 )
			filePixelComponent = OFX::ePixelComponentRGBA;
		else
			filePixelComponent = OFX::ePixelComponentCustom;

		if( OFX::getImageEffectHostDescription()->supportsPixelComponent( filePixelComponent ) )
			clipPreferences.setClipComponents( *_clipDst, filePixelComponent );
		else
			clipPreferences.setClipComponents( *_clipDst, OFX::getImageEffectHostDescription()->getDefaultPixelComponent() );
	}

	// output frame rate
	const double fps = params._inputVideoProperties->getFps();
	clipPreferences.setOutputFrameRate( fps );

	clipPreferences.setPixelAspectRatio( *_clipDst, retrievePAR() );

	// interlaced
	const bool isInterlaced = params._inputVideoProperties->isInterlaced();
	if( isInterlaced )
	{
		const bool topFieldFirst = params._inputVideoProperties->isTopFieldFirst();
		clipPreferences.setOutputFielding( topFieldFirst ? OFX::eFieldUpper : OFX::eFieldLower );
	}
	else
	{
		clipPreferences.setOutputFielding( OFX::eFieldNone );
	}
}

bool AVReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	ensureVideoIsOpen();

	AVReaderParams params = getProcessParams();
	size_t nbFrames = params._inputVideoProperties->getNbFrames();
	// if nbFrames is unknown
	if( nbFrames == 0 )
		nbFrames = 1;

	range.min = 0.0;
	range.max = nbFrames - 1.0;

	return true;
}

bool AVReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ensureVideoIsOpen();

	// get metadata of video stream
	AVReaderParams params = getProcessParams();
	const size_t width = params._inputVideoProperties->getWidth();
	const size_t height = params._inputVideoProperties->getHeight();
	
	const double pixelAspectRatio = retrievePAR();

	rod.x1 = 0;
	rod.x2 = width * pixelAspectRatio;
	rod.y1 = 0;
	rod.y2 = height;

	return true;
}

void AVReaderPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	ReaderPlugin::beginSequenceRender( args );

	ensureVideoIsOpen();

	AVReaderParams params = getProcessParams();

	// set format
	avtranscoder::ProfileLoader::Profile formatProfile;
	formatProfile[ avtranscoder::constants::avProfileIdentificator ] = "customFormatPreset";
	formatProfile[ avtranscoder::constants::avProfileIdentificatorHuman ] = "Custom format preset";
	formatProfile[ avtranscoder::constants::avProfileType ] = avtranscoder::constants::avProfileTypeFormat;
	formatProfile[ avtranscoder::constants::avProfileFormat ] = params._inputFormatName;
	// format options
	const avtranscoder::ProfileLoader::Profile formatCommonProfile = _paramFormatCustom.getCorrespondingProfile();
	formatProfile.insert( formatCommonProfile.begin(), formatCommonProfile.end() );
	// format detail options
	const avtranscoder::ProfileLoader::Profile formatDetailProfile = _paramFormatDetailCustom.getCorrespondingProfile( params._inputFormatName );
	formatProfile.insert( formatDetailProfile.begin(), formatDetailProfile.end() );
	_inputFile->setupUnwrapping( formatProfile );

	// set video decoder
	avtranscoder::ProfileLoader::Profile videoProfile;
	videoProfile[ avtranscoder::constants::avProfileIdentificator ] = "customVideoPreset";
	videoProfile[ avtranscoder::constants::avProfileIdentificatorHuman ] = "Custom video preset";
	videoProfile[ avtranscoder::constants::avProfileType ] = avtranscoder::constants::avProfileTypeVideo;
	videoProfile[ avtranscoder::constants::avProfileCodec ] = params._inputVideoProperties->getCodecName();
	// video options
	const avtranscoder::ProfileLoader::Profile videoCommonProfile = _paramVideoCustom.getCorrespondingProfile();
	videoProfile.insert( videoCommonProfile.begin(), videoCommonProfile.end() );
	// video detail options
	const avtranscoder::ProfileLoader::Profile videoDetailProfile = _paramVideoDetailCustom.getCorrespondingProfile( params._inputVideoProperties->getCodecName() );
	videoProfile.insert( videoDetailProfile.begin(), videoDetailProfile.end() );
	_inputDecoder->setupDecoder( videoProfile );

	// get source image
	const avtranscoder::VideoFrameDesc sourceImageDesc( _inputStream->getVideoCodec().getVideoFrameDesc() );
	_sourceImage.reset( new avtranscoder::VideoFrame( sourceImageDesc ) );

	// get image to decode
	const avtranscoder::VideoFrameDesc imageToDecodeDesc( sourceImageDesc.getWidth(), sourceImageDesc.getHeight(), "rgb24" );
	_imageToDecode.reset( new avtranscoder::VideoFrame( imageToDecodeDesc ) );
}

void AVReaderPlugin::render( const OFX::RenderArguments& args )
{
	if( ! _initVideo )
		return;
	
	ReaderPlugin::render(args);
	doGilRender<AVReaderProcess>( *this, args );
}

}
}
}
}
