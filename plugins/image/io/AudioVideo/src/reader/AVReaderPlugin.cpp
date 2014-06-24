#include <AvTranscoder/DatasStructures/Pixel.hpp>

#include "AVReaderPlugin.hpp"
#include "AVReaderProcess.hpp"
#include "AVReaderDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

using namespace boost::gil;
namespace fs = boost::filesystem;

AVReaderPlugin::AVReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
	, _inputFile( NULL )
	, _inputStreamVideo( NULL )
	, _sourceImage( NULL )
	, _imageToDecode( NULL )
	, _lastInputFilePath( "" )
	, _idVideoStream( 0 )
	, _lastFrame( 0 )
{
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramVideoStreamIndex = fetchIntParam( kParamVideoStreamIndex );
	_paramUseCustomSAR = fetchBooleanParam( kParamUseCustomSAR );
	_paramCustomSAR = fetchDoubleParam( kParamCustomSAR );

	updateVisibleTools();
}

void AVReaderPlugin::ensureVideoIsOpen( const std::string& filepath )
{
	if( _lastInputFilePath == filepath && // already opened
		! _lastInputFilePath.empty() ) // not the first time...
		return;
	
	if( filepath == "" || ! boost::filesystem::exists( filepath ) )
	{
		_inputFile.reset();
		_lastInputFilePath = "";
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
		    << exception::filename( filepath ) );
	}
	
	try
	{
		// set and analyse inputFile
		_inputFile.reset( new avtranscoder::InputFile( filepath ) );
		_lastInputFilePath = filepath;
		_inputFile->analyse();
		
		// set range of the OFX param
		_paramVideoStreamIndex->setRange( 0, _inputFile->getProperties().videoStreams.size() );
		
		// get streamId of the video stream
		_idVideoStream = _inputFile->getProperties().videoStreams.at( _paramVideoStreamIndex->getValue() ).streamId;
		
		// buffered video stream at _indexVideoStream (to seek)
		_inputFile->readStream( _idVideoStream );
		
		// set video stream
		_inputStreamVideo.reset( new avtranscoder::InputStreamVideo( _inputFile->getStream( _idVideoStream ) ) );
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open input file : " + e.what()
		    << exception::filename( filepath ) );
	}
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
}

void AVReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ensureVideoIsOpen( _paramFilepath->getValue() );
	
	ReaderPlugin::getClipPreferences( clipPreferences );
	
	// frame varying
	clipPreferences.setOutputFrameVarying( true );
	
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
	double fps = properties.videoStreams.at( _idVideoStream ).fps;
	clipPreferences.setOutputFrameRate( fps );

	// sar
	if( _paramUseCustomSAR->getValue() )
	{
		clipPreferences.setPixelAspectRatio( *_clipDst, _paramCustomSAR->getValue() );
	}
	else
	{
		const avtranscoder::Properties& properties = _inputFile->getProperties();
		avtranscoder::Ratio sar = properties.videoStreams.at( _idVideoStream ).sar;
		const double videoRatio = sar.num / (double)sar.den;

		clipPreferences.setPixelAspectRatio( *_clipDst, videoRatio );
	}

	// interlaced
	bool isInterlaced = properties.videoStreams.at( _idVideoStream ).isInterlaced;
	if( isInterlaced )
	{
		bool topFieldFirst = properties.videoStreams.at( _idVideoStream ).topFieldFirst;
		clipPreferences.setOutputFielding( topFieldFirst ? OFX::eFieldUpper : OFX::eFieldLower );
	}
	else
	{
		clipPreferences.setOutputFielding( OFX::eFieldNone );
	}
}

bool AVReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	ensureVideoIsOpen( _paramFilepath->getValue() );

	double duration = _inputFile->getProperties().duration;
	double fps = _inputFile->getProperties().videoStreams.at( _idVideoStream ).fps;
	double nbFrames = fps * duration;

	range.min = 0.0;
	range.max = nbFrames - 1;

	return true;
}

bool AVReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ensureVideoIsOpen( _paramFilepath->getValue() );

	// get metadata of video stream
	const avtranscoder::Properties& properties = _inputFile->getProperties();
	size_t width = properties.videoStreams.at( _idVideoStream ).width;
	size_t height = properties.videoStreams.at( _idVideoStream ).height;
	avtranscoder::Ratio sar = properties.videoStreams.at( _idVideoStream ).sar;
	const double videoRatio = sar.num / (double)sar.den;

	const bool useCustomSAR = _paramUseCustomSAR->getValue();
	const double customSAR = _paramCustomSAR->getValue();

	rod.x1 = 0;
	rod.x2 = width * ( useCustomSAR ? customSAR : videoRatio );
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
	ensureVideoIsOpen( _paramFilepath->getValue() );
	
	// get source image
	avtranscoder::ImageDesc sourceImageDesc = _inputFile->getStream( _idVideoStream ).getVideoDesc().getImageDesc();
	_sourceImage.reset( new avtranscoder::Image( sourceImageDesc ) );
	
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
	avtranscoder::ImageDesc imageToDecodeDesc( sourceImageDesc );
	imageToDecodeDesc.setPixel( dstPixel.findPixel() );
	_imageToDecode.reset( new avtranscoder::Image( imageToDecodeDesc ) );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AVReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render(args);
	doGilRender<AVReaderProcess>( *this, args );
}

}
}
}
}
