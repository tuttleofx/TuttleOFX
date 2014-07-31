#include "AVReaderPlugin.hpp"
#include "AVReaderProcess.hpp"
#include "AVReaderDefinitions.hpp"

#include <AvTranscoder/EssenceStructures/Pixel.hpp>
#include <AvTranscoder/ProgressListener.hpp>
#include <AvTranscoder/Metadatas/Print.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

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
	disableAVOptionsForCodecOrFormat( optionsFormatDetailMap, "", common::kPrefixFormat );
	
	avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
	_paramVideoDetailCustom.fetchCustomParams( *this, optionsVideoCodecMap, common::kPrefixVideo );
	disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, "", common::kPrefixVideo );
	
	_paramMetaDataInputFile = fetchStringParam( kParamMetaDataInputFile );

	updateVisibleTools();
	
	_videoProfile[ avtranscoder::Profile::avProfileIdentificator ] = "readerVideoPreset";
	_videoProfile[ avtranscoder::Profile::avProfileIdentificatorHuman ] = "Reader video preset";
	_videoProfile[ avtranscoder::Profile::avProfileType ] = avtranscoder::Profile::avProfileTypeVideo;
}

void AVReaderPlugin::disableAVOptionsForCodecOrFormat( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& codec, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		const std::string subGroupName = subGroupOption.first;
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
					groupName += common::kPrefixGroup;
					groupName += subGroupName;
					groupName += "_";
					groupName += option.getName();
					
					OFX::GroupParam* curOpt = fetchGroupParam( groupName );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string childName = prefix;
						if( ! subGroupName.empty() )
						{
							childName += subGroupName;
							childName += "_";
						}
						childName += child.getUnit();
						childName += common::kPrefixFlag;
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

void AVReaderPlugin::updateProfileFromCustomParams( const common::CustomParams& customParams, avtranscoder::Profile::ProfileDesc& profile )
{
	common::CustomParams::OptionsForPreset optionsForPreset = customParams.getOptionsNameAndValue();
	BOOST_FOREACH( common::CustomParams::OptionsForPreset::value_type& nameAndValue, optionsForPreset )
	{
		profile[ nameAndValue.first ] = nameAndValue.second;
	}
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

			// update MetaData tab
			_paramMetaDataInputFile->setValue( avtranscoder::getMetadatasToDisplay( *_inputFile ) );
			
			// update format details parameters
			avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
			disableAVOptionsForCodecOrFormat( optionsFormatMap, inputProperties.formatName, common::kPrefixFormat );
			
			// update video details parameters
			avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
			const std::string videoCodecName = inputProperties.videoStreams.at( _paramVideoStreamIndex->getValue() ).codecName;
			disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName, common::kPrefixVideo );
		}
		catch( std::exception& e )
		{
			_paramVideoStreamIndex->setRange( 0, 100. );
			_paramVideoStreamIndex->setDisplayRange( 0, 16 );

			_paramMetaDataInputFile->setValue( _paramMetaDataInputFile->getDefault() );
			
			avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
			disableAVOptionsForCodecOrFormat( optionsFormatMap, "", common::kPrefixFormat );
			
			avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
			disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, "", common::kPrefixVideo );
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
	double nbFrames = fps * duration;

	range.min = 0.0;
	range.max = nbFrames - 1;

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

	updateProfileFromCustomParams( _paramFormatCustom, _formatProfile );
	_inputFile->setProfile( _formatProfile );
	
	updateProfileFromCustomParams( _paramVideoCustom, _videoProfile );
	_inputStreamVideo->setProfile( _videoProfile );
	
	// get source image
	avtranscoder::VideoFrameDesc sourceImageDesc = _inputFile->getStream( _videoStreamId ).getVideoDesc().getVideoFrameDesc();
	_sourceImage.reset( new avtranscoder::VideoFrame( sourceImageDesc ) );
	
	// get pixel data of image to decode
	avtranscoder::Pixel dstPixel;
	size_t pixelComponents = sourceImageDesc.getPixelDesc().getComponents(); // get this from gil view
	size_t pixelDepth = 8; // @todo: waiting for getMaxBitPerChannel() in avTranscoder
	dstPixel.setBitsPerPixel( pixelDepth * pixelComponents );
	dstPixel.setComponents( pixelComponents );
	dstPixel.setColorComponents( avtranscoder::eComponentRgb );
	dstPixel.setSubsampling( avtranscoder::eSubsamplingNone );
	dstPixel.setAlpha( false );
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
