#include "AVReaderPlugin.hpp"
#include "AVReaderProcess.hpp"
#include "AVReaderDefinitions.hpp"

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/option/Context.hpp>
#include <AvTranscoder/option/FormatContext.hpp>
#include <AvTranscoder/option/CodecContext.hpp>
#include <AvTranscoder/progress/NoDisplayProgress.hpp>
#include <AvTranscoder/frame/Pixel.hpp>
#include <AvTranscoder/mediaProperty/FileProperties.hpp>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/trunc.hpp>

#include <stdexcept>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

using namespace boost::gil;
namespace fs = boost::filesystem;

AVReaderPlugin::AVReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
	, _paramFormatCustom()
	, _paramVideoCustom()
	, _paramMetaDataCustom()
	, _paramFormatDetailCustom()
	, _paramVideoDetailCustom()
	, _inputFile( NULL )
	, _inputStreamVideo( NULL )
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

	avtranscoder::FormatContext formatContext( AV_OPT_FLAG_DECODING_PARAM );
	avtranscoder::OptionArray formatOptions = formatContext.getOptions();
	_paramFormatCustom.fetchLibAVParams( *this, formatOptions, common::kPrefixFormat );

	avtranscoder::CodecContext videoCodecContext( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	avtranscoder::OptionArray videoOptions = videoCodecContext.getOptions();
	_paramVideoCustom.fetchLibAVParams( *this, videoOptions, common::kPrefixVideo );
	
	avtranscoder::CodecContext metadataCodecContext( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_METADATA );
	avtranscoder::OptionArray metadataOptions = metadataCodecContext.getOptions();
	_paramMetaDataCustom.fetchLibAVParams( *this, metadataOptions, common::kPrefixMetaData );
	
	avtranscoder::OptionArrayMap optionsFormatDetailMap = avtranscoder::getOutputFormatOptions();
	_paramFormatDetailCustom.fetchLibAVParams( *this, optionsFormatDetailMap, common::kPrefixFormat );
	common::disableOFXParamsForFormatOrCodec( *this, optionsFormatDetailMap, "", common::kPrefixFormat );
	
	avtranscoder::OptionArrayMap optionsVideoCodecMap = avtranscoder::getVideoCodecOptions();
	_paramVideoDetailCustom.fetchLibAVParams( *this, optionsVideoCodecMap, common::kPrefixVideo );
	common::disableOFXParamsForFormatOrCodec( *this, optionsVideoCodecMap, "", common::kPrefixVideo );
	
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

	updateVisibleTools();
}

void AVReaderPlugin::ensureVideoIsOpen()
{
	const std::string& filepath = _paramFilepath->getValue();
	const size_t videoStreamIndex = _paramVideoStreamIndex->getValue();
	 
	if( _lastInputFilePath == filepath && // already opened
		! _lastInputFilePath.empty() && // not the first time...
		_lastVideoStreamIndex == videoStreamIndex )
		return;
	
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
		avtranscoder::NoDisplayProgress progress;
		// using fast analyse ( do not extract gop structure )
		_inputFile->analyse( progress, avtranscoder::eAnalyseLevelHeader );
		
		// get streamId of the video stream
		if( videoStreamIndex >= _inputFile->getProperties().getVideoProperties().size() )
		{
			throw std::runtime_error( "the stream index doesn't exist in the input file" );
		}
		_lastVideoStreamIndex = videoStreamIndex;
		
		// buffered video stream at _indexVideoStream (to seek)
		_inputFile->activateStream( _paramVideoStreamIndex->getValue() );
		
		// set video stream
		_inputStreamVideo.reset( new avtranscoder::AvInputVideo( _inputFile->getStream( _paramVideoStreamIndex->getValue() ) ) );
		_inputStreamVideo->setup();
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
	_inputStreamVideo.reset();
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

	_paramFilepath->getValue( params._filepath );
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
		try
		{
			ensureVideoIsOpen();

			const avtranscoder::FileProperties& inputProperties = _inputFile->getProperties();

			// set range of the OFX param
			_paramVideoStreamIndex->setRange( 0, inputProperties.getVideoProperties().size() );
			_paramVideoStreamIndex->setDisplayRange( 0, inputProperties.getVideoProperties().size() );

			// update wrapper of Metadata tab
			std::string wrapperValue( "" );
			BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, inputProperties.getPropertiesAsMap() )
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
			BOOST_FOREACH( const avtranscoder::VideoProperties& videoStream, inputProperties.getVideoProperties() )
			{
				videoValue += "::::: VIDEO STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, videoStream.getPropertiesAsMap() )
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
			BOOST_FOREACH( const avtranscoder::AudioProperties& audioStream, inputProperties.getAudioProperties() )
			{
				audioValue += "::::: AUDIO STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, audioStream.getPropertiesAsMap() )
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
			BOOST_FOREACH( const avtranscoder::DataProperties& dataStream, inputProperties.getDataProperties() )
			{
				dataValue += "::::: DATA STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, dataStream.getPropertiesAsMap() )
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
			BOOST_FOREACH( const avtranscoder::SubtitleProperties& subtitleStream, inputProperties.getSubtitleProperties() )
			{
				subtitleValue += "::::: SUBTITLE STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, subtitleStream.getPropertiesAsMap() )
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
			BOOST_FOREACH( const avtranscoder::AttachementProperties& attachementStream, inputProperties.getAttachementProperties() )
			{
				attachementValue += "::::: ATTACHEMENT STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, attachementStream.getPropertiesAsMap() )
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
			BOOST_FOREACH( const avtranscoder::UnknownProperties& unknownStream, inputProperties.getUnknownPropertiesProperties() )
			{
				unknownValue += "::::: UNKNOWN STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::PropertiesMap::value_type& pair, unknownStream.getPropertiesAsMap() )
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
			common::disableOFXParamsForFormatOrCodec( *this, optionsFormatMap, inputProperties.getFormatName(), common::kPrefixFormat );
			
			// update video details parameters
			avtranscoder::OptionArrayMap optionsVideoCodecMap = avtranscoder::getVideoCodecOptions();
			const std::string videoCodecName = inputProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getCodecName();
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
}

/**
 * @brief Retrieve Pixel Aspect Ratio from parameters (if custom) or from video file.
 * @warning video have to be open (see ensureVideoIsOpen)
 */
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
		const avtranscoder::FileProperties& fileProperties = _inputFile->getProperties();
		avtranscoder::Rational sar = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getSar();
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

	const avtranscoder::FileProperties fileProperties = _inputFile->getProperties();

	// conversion of bitdepth
	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
	{
		size_t bitDepth = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getBitDepth();
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
		size_t nbComponents = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getComponentsCount();
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
	double fps = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getFps();
	clipPreferences.setOutputFrameRate( fps );

	clipPreferences.setPixelAspectRatio( *_clipDst, retrievePAR() );

	// interlaced
	bool isInterlaced = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).isInterlaced();
	if( isInterlaced )
	{
		bool topFieldFirst = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).isTopFieldFirst();
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

	double duration = _inputFile->getProperties().getDuration();
	double fps = _inputFile->getProperties().getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getFps();
	double nbFrames = boost::math::trunc( fps * duration );

	range.min = 0.0;
	range.max = nbFrames - 1.0;

	return true;
}

bool AVReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ensureVideoIsOpen();

	// get metadata of video stream
	const avtranscoder::FileProperties& fileProperties = _inputFile->getProperties();
	size_t width = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getWidth();
	size_t height = fileProperties.getVideoProperties().at( _paramVideoStreamIndex->getValue() ).getHeight();
	
	const double pixelAspectRatio = retrievePAR();

	rod.x1 = 0;
	rod.x2 = width * pixelAspectRatio;
	rod.y1 = 0;
	rod.y2 = height;

	return true;
}
/**
 * @brief The overridden begin render function
 * @param[in]   args     Begin Rendering parameters
 */
void AVReaderPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	ensureVideoIsOpen();

	_inputFile->setProfile( _paramFormatCustom.getCorrespondingProfile() );
	_inputStreamVideo->setProfile( _paramVideoCustom.getCorrespondingProfile() );
	
	// get source image
	avtranscoder::VideoFrameDesc sourceImageDesc = _inputFile->getStream( _paramVideoStreamIndex->getValue() ).getVideoCodec().getVideoFrameDesc();
	_sourceImage.reset( new avtranscoder::VideoFrame( sourceImageDesc ) );
	
	// get pixel data of image to decode
	avtranscoder::Pixel dstPixel;
	dstPixel.setColorComponents( avtranscoder::eComponentRgb );
	dstPixel.setPlanar( false );
	
	// get image to decode
	avtranscoder::VideoFrameDesc imageToDecodeDesc( sourceImageDesc );
	imageToDecodeDesc.setPixel( dstPixel.findPixel() );
	_imageToDecode.reset( new avtranscoder::VideoFrame( imageToDecodeDesc ) );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
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
