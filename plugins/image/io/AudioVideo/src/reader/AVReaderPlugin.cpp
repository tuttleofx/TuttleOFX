#include "AVReaderPlugin.hpp"
#include "AVReaderProcess.hpp"
#include "AVReaderDefinitions.hpp"

#include <AvTranscoder/EssenceStructures/Pixel.hpp>
#include <AvTranscoder/ProgressListener.hpp>
#include <AvTranscoder/Metadatas/Print.hpp>

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
	, _videoStreamId( 0 )
	, _lastFrame( -1 )
	, _initVideo( false )
{
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramVideoStreamIndex = fetchIntParam( kParamVideoStreamIndex );
	_paramUseCustomSAR = fetchBooleanParam( kParamUseCustomSAR );
	_paramCustomSAR = fetchDoubleParam( kParamCustomSAR );
	
	avtranscoder::OptionLoader::OptionArray formatsOptions = _optionLoader.loadFormatContextOptions( AV_OPT_FLAG_DECODING_PARAM );
	_paramFormatCustom.fetchCustomParams( *this, formatsOptions, common::kPrefixFormat );
	
	avtranscoder::OptionLoader::OptionArray videoOptions = _optionLoader.loadCodecContextOptions( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM );
	_paramVideoCustom.fetchCustomParams( *this, videoOptions, common::kPrefixVideo );
	
	avtranscoder::OptionLoader::OptionArray metadataOptions = _optionLoader.loadCodecContextOptions( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_METADATA );
	_paramMetaDataCustom.fetchCustomParams( *this, metadataOptions, common::kPrefixMetaData );
	
	avtranscoder::OptionLoader::OptionMap optionsFormatDetailMap = _optionLoader.loadOutputFormatOptions();
	_paramFormatDetailCustom.fetchCustomParams( *this, optionsFormatDetailMap, common::kPrefixFormat );
	common::disableOFXParamsForFormatOrCodec( *this, optionsFormatDetailMap, "", common::kPrefixFormat );
	
	avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
	_paramVideoDetailCustom.fetchCustomParams( *this, optionsVideoCodecMap, common::kPrefixVideo );
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
		avtranscoder::ProgressListener progress;
		// using fast analyse ( do not extract gop structure )
		_inputFile->analyse( progress, avtranscoder::InputFile::eAnalyseLevelFast );
		
		// get streamId of the video stream
		if( videoStreamIndex >= _inputFile->getProperties().videoStreams.size() )
		{
			throw std::runtime_error( "the stream index doesn't exist in the input file" );
		}
		_videoStreamId = _inputFile->getProperties().videoStreams.at( videoStreamIndex ).streamId;
		_lastVideoStreamIndex = videoStreamIndex;
		
		// buffered video stream at _indexVideoStream (to seek)
		_inputFile->readStream( _videoStreamId );
		
		// set video stream
		_inputStreamVideo.reset( new avtranscoder::InputVideo( _inputFile->getStream( _videoStreamId ) ) );
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

			const avtranscoder::Properties& inputProperties = _inputFile->getProperties();

			// set range of the OFX param
			_paramVideoStreamIndex->setRange( 0, inputProperties.videoStreams.size() );
			_paramVideoStreamIndex->setDisplayRange( 0, inputProperties.videoStreams.size() );

			// update wrapper of Metadata tab
			std::string wrapperValue( "" );
			BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, inputProperties.getDataMap() )
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
			BOOST_FOREACH( const avtranscoder::VideoProperties& videoStream, inputProperties.videoStreams )
			{
				videoValue += "::::: VIDEO STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, videoStream.getDataMap() )
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
			BOOST_FOREACH( const avtranscoder::AudioProperties& audioStream, inputProperties.audioStreams )
			{
				audioValue += "::::: AUDIO STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, audioStream.getDataMap() )
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
			BOOST_FOREACH( const avtranscoder::DataProperties& dataStream, inputProperties.dataStreams )
			{
				dataValue += "::::: DATA STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, dataStream.getDataMap() )
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
			BOOST_FOREACH( const avtranscoder::SubtitleProperties& subtitleStream, inputProperties.subtitleStreams )
			{
				subtitleValue += "::::: SUBTITLE STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, subtitleStream.getDataMap() )
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
			BOOST_FOREACH( const avtranscoder::AttachementProperties& attachementStream, inputProperties.attachementStreams )
			{
				attachementValue += "::::: ATTACHEMENT STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, attachementStream.getDataMap() )
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
			BOOST_FOREACH( const avtranscoder::UnknownProperties& unknownStream, inputProperties.unknownStreams )
			{
				unknownValue += "::::: UNKNOWN STREAM ::::: \n";
				BOOST_FOREACH( const avtranscoder::MetadatasMap::value_type& pair, unknownStream.getDataMap() )
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
			avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
			common::disableOFXParamsForFormatOrCodec( *this, optionsFormatMap, inputProperties.formatName, common::kPrefixFormat );
			
			// update video details parameters
			avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
			const std::string videoCodecName = inputProperties.videoStreams.at( _paramVideoStreamIndex->getValue() ).codecName;
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

			avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
			common::disableOFXParamsForFormatOrCodec( *this, optionsFormatMap, "", common::kPrefixFormat );
			
			avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
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
	
	if( _paramUseCustomSAR->getValue() )
		return _paramCustomSAR->getValue();
	
	const avtranscoder::Properties& properties = _inputFile->getProperties();
	avtranscoder::Ratio sar = properties.videoStreams.at( _videoStreamId ).sar;
	const double videoRatio = sar.num / (double)sar.den;

	return videoRatio;
}

void AVReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ensureVideoIsOpen();
	
	ReaderPlugin::getClipPreferences( clipPreferences );
	
	// conversion of bitdepth
	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
	{
		// @TODO get file info => if bitdetph = 10 or 16 => UShort
		OFX::EBitDepth fileBitDepth = OFX::eBitDepthUByte;
		
		if( OFX::getImageEffectHostDescription()->supportsBitDepth( fileBitDepth ) )
			clipPreferences.setClipBitDepth( *_clipDst, fileBitDepth );
		else
			clipPreferences.setClipBitDepth( *_clipDst, OFX::getImageEffectHostDescription()->getDefaultPixelDepth() );
	}
	
	// conversion of channel
	if( getExplicitChannelConversion() == eParamReaderChannelAuto )
	{
		// @TODO get file info => if components = rgba set RGBA
		OFX::EPixelComponent filePixelComponent = OFX::ePixelComponentRGB;
		
		if( OFX::getImageEffectHostDescription()->supportsPixelComponent( filePixelComponent ) )
			clipPreferences.setClipComponents( *_clipDst, filePixelComponent );
		else
			clipPreferences.setClipComponents( *_clipDst, OFX::getImageEffectHostDescription()->getDefaultPixelComponent() );
	}
	
	const avtranscoder::Properties& properties = _inputFile->getProperties();
	
	// output frame rate
	double fps = properties.videoStreams.at( _videoStreamId ).fps;
	clipPreferences.setOutputFrameRate( fps );

	clipPreferences.setPixelAspectRatio( *_clipDst, retrievePAR() );

	// interlaced
	bool isInterlaced = properties.videoStreams.at( _videoStreamId ).isInterlaced;
	if( isInterlaced )
	{
		bool topFieldFirst = properties.videoStreams.at( _videoStreamId ).topFieldFirst;
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

	double duration = _inputFile->getProperties().duration;
	double fps = _inputFile->getProperties().videoStreams.at( _videoStreamId ).fps;
	double nbFrames = boost::math::trunc( fps * duration );

	range.min = 0.0;
	range.max = nbFrames - 1.0;

	return true;
}

bool AVReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ensureVideoIsOpen();

	// get metadata of video stream
	const avtranscoder::Properties& properties = _inputFile->getProperties();
	size_t width = properties.videoStreams.at( _videoStreamId ).width;
	size_t height = properties.videoStreams.at( _videoStreamId ).height;
	
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

	_inputFile->setProfile( _paramFormatCustom.getCorrespondingProfileDesc() );
	_inputStreamVideo->setProfile( _paramVideoCustom.getCorrespondingProfileDesc() );
	
	// get source image
	avtranscoder::VideoFrameDesc sourceImageDesc = _inputFile->getStream( _videoStreamId ).getVideoDesc().getVideoFrameDesc();
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
