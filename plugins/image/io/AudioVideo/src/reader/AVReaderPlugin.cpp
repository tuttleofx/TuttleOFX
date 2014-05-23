#include <AvTranscoder/common.hpp>
#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>

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
	, _indexVideoStream( 0 )
	, _lastInputFilePath( "" )
{	
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kTuttlePluginFilename );
	_paramBitDepth = fetchChoiceParam( kTuttlePluginBitDepth );
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
		_lastInputFilePath = filepath;
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
		    << exception::filename( filepath ) );
	}
	
	try
	{
		// set and analyse inputFile
		_inputFile.reset( new avtranscoder::InputFile( filepath ) );
		_lastInputFilePath = filepath;
		_inputFile->analyse();

		// get index of first video stream
		_indexVideoStream = _inputFile->getProperties().videoStreams[0].streamId;
		
		// set video stream
		_inputStreamVideo.reset( new avtranscoder::InputStreamVideo( _inputFile->getStream( _indexVideoStream ) ) );
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open file : " + e.what()
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
	
	// conversion of bitdepth
	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
	{
		clipPreferences.setClipBitDepth( *_clipDst, OFX::eBitDepthUByte ); /// @todo tuttle: some video format may need other bit depth (how we can detect this ?)
	}
	else
	{
		clipPreferences.setClipBitDepth( *_clipDst, OFX::eBitDepthUShort );
	}
	
	// conversion of channel
	if( getExplicitChannelConversion() == eParamReaderChannelAuto )
	{
		clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGB );
	}
	else
	{
		clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
	}
	
	// frame varying
	clipPreferences.setOutputFrameVarying( true );
	
	// sar
	const avtranscoder::Properties& properties = _inputFile->getProperties();
	avtranscoder::Ratio sar = properties.videoStreams.at( _indexVideoStream ).sar;
	const double videoRatio = sar.num / (double)sar.den;
	clipPreferences.setPixelAspectRatio( *this->_clipDst, videoRatio );
	
	// output frame rate
	double fps = properties.videoStreams.at( _indexVideoStream ).fps;
	clipPreferences.setOutputFrameRate( fps );
	
	// sar
	const bool useCustomSAR = _paramUseCustomSAR->getValue();
	const double customSAR = _paramCustomSAR->getValue();
	clipPreferences.setPixelAspectRatio( *_clipDst, useCustomSAR ? customSAR : videoRatio );
	
	// interlaced
	bool isInterlaced = properties.videoStreams.at( _indexVideoStream ).isInterlaced;
	bool topFieldFirst = properties.videoStreams.at( _indexVideoStream ).topFieldFirst;
	if( isInterlaced )
	{
		clipPreferences.setOutputFielding( OFX::eFieldNone );
	}
	else
	{
		if( topFieldFirst )
		{
			clipPreferences.setOutputFielding( OFX::eFieldUpper );
		}
		else
		{
			clipPreferences.setOutputFielding( OFX::eFieldLower );
		}
	}
}

bool AVReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	ensureVideoIsOpen( _paramFilepath->getValue() );
	
	double duration = _inputFile->getProperties().duration;
	double fps = _inputFile->getProperties().videoStreams.at( _indexVideoStream ).fps;
	double nbFrames = fps * duration;
	
	range.min = 0.0;
	range.max = nbFrames - 1;
	
	//TUTTLE_LOG_VAR2( TUTTLE_INFO, range.min, range.max );
	
	return true;
}

bool AVReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ensureVideoIsOpen( _paramFilepath->getValue() );
	
	// get metadata of video stream
	const avtranscoder::Properties& properties = _inputFile->getProperties();
	size_t width = properties.videoStreams.at( _indexVideoStream ).width;
	size_t height = properties.videoStreams.at( _indexVideoStream ).height;
	avtranscoder::Ratio sar = properties.videoStreams.at( _indexVideoStream ).sar;
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
