#include "AVReaderPlugin.hpp"
#include "AVReaderProcess.hpp"
#include "AVReaderDefinitions.hpp"

#include <libav/LibAVVideoReader.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

using namespace boost::gil;
namespace fs = boost::filesystem;

AVReaderPlugin::AVReaderPlugin( OfxImageEffectHandle handle )
	: AVOptionPlugin<ReaderPlugin>( handle )
	, _errorInFile( false )
	, _initReader( false )
{
	// We want to render a sequence
	setSequentialRender( true );

	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kTuttlePluginFilename );
	_paramBitDepth = fetchChoiceParam( kTuttlePluginBitDepth );
	_paramUseCustomSAR = fetchBooleanParam( kParamUseCustomSAR );
	_paramCustomSAR = fetchDoubleParam( kParamCustomSAR );

	updateVisibleTools();
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

/**
 * @brief Open the video if not already opened.
 * @return If the video file is now open,
 *         else the file doesn't exist, is unrecognized or is corrupted.
 */
bool AVReaderPlugin::ensureVideoIsOpen()
{
	if( _reader.isOpen() )
		return true;

	// if we have not already tried
	if( !_errorInFile )
	{
		_errorInFile = !_reader.open( _paramFilepath->getValue() );
	}
	return !_errorInFile;
}

void AVReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	ReaderPlugin::changedParam( args, paramName );
	if( paramName == kTuttlePluginFilename )
	{
		_errorInFile = false;
	}
	else if( paramName == kParamUseCustomSAR )
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
	ReaderPlugin::getClipPreferences( clipPreferences );
	clipPreferences.setOutputFrameVarying( true );
	clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
	if( getExplicitBitDepthConversion() == eParamReaderBitDepthAuto )
	{
		clipPreferences.setClipBitDepth( *_clipDst, OFX::eBitDepthUByte ); /// @todo tuttle: some video format may need other bit depth (how we can detect this ?)
	}

	if( !ensureVideoIsOpen() )
		return;

	// options depending on input file
	const bool useCustomSAR = _paramUseCustomSAR->getValue();
	const double customSAR = _paramCustomSAR->getValue();
	clipPreferences.setPixelAspectRatio( *_clipDst, useCustomSAR ? customSAR : _reader.aspectRatio() );
	clipPreferences.setOutputFrameRate( _reader.fps() );
	
	// Setup fielding
	switch( _reader.interlacment() )
	{
		case  eInterlacmentNone:
		{
			clipPreferences.setOutputFielding( OFX::eFieldNone );
			break;
		}
		case  eInterlacmentUpper:
		{
			clipPreferences.setOutputFielding( OFX::eFieldUpper );
			break;
		}
		case  eInterlacmentLower:
		{
			clipPreferences.setOutputFielding( OFX::eFieldLower );
			break;
		}
	}
}

bool AVReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	if( !ensureVideoIsOpen() )
		return false;

	range.min = 0.0;
	range.max = (double)(_reader.nbFrames()-1);
	
	//TUTTLE_LOG_VAR2( TUTTLE_INFO, range.min, range.max );
	
	return true;
}

bool AVReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	if( !ensureVideoIsOpen() )
		return false;

	const bool useCustomSAR = _paramUseCustomSAR->getValue();
	const double customSAR = _paramCustomSAR->getValue();

	rod.x1 = 0;
	rod.x2 = _reader.width() * ( useCustomSAR ? customSAR : _reader.aspectRatio() );
	rod.y1 = 0;
	rod.y2 = _reader.height();
	return true;
}

void AVReaderPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	ensureVideoIsOpen();
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AVReaderPlugin::render( const OFX::RenderArguments& args )
{
	if( !ensureVideoIsOpen() )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown() );
	}

	if( !_initReader )
	{
		// set Format parameters
		AVFormatContext* avFormatContext;
		avFormatContext = avformat_alloc_context();
		setParameters( _reader, eAVParamFormat, (void*)avFormatContext, AV_OPT_FLAG_DECODING_PARAM, 0 );
		avformat_free_context( avFormatContext );
		
		// set Video Codec parameters
		AVCodecContext* avCodecContext;
		
	#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 8, 0 )
		avCodecContext = avcodec_alloc_context();
		// deprecated in the same version
		//avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
	#else
		AVCodec* avCodec = NULL;
		avCodecContext = avcodec_alloc_context3( avCodec );
	#endif
		setParameters( _reader, eAVParamVideo, (void*)avCodecContext, AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, 0 );
	}
	
	doGilRender<AVReaderProcess>( *this, args );
}

void AVReaderPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_reader.close();
}

}
}
}
}
