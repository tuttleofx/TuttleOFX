#include "FFMpegReaderPlugin.hpp"
#include "FFMpegReaderProcess.hpp"
#include "FFMpegReaderDefinitions.hpp"

#include <ffmpeg/VideoFFmpegReader.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

using namespace boost::gil;
namespace fs = boost::filesystem;

FFMpegReaderPlugin::FFMpegReaderPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
	, _errorInFile( false )
{
	// We want to render a sequence
	setSequentialRender( true );

	_clipDst       = fetchClip( kOfxImageEffectOutputClipName );
	_paramFilepath = fetchStringParam( kFilename );
}

FFMpegReaderParams FFMpegReaderPlugin::getProcessParams() const
{
	FFMpegReaderParams params;

	_paramFilepath->getValue( params._filepath );
	return params;
}

/**
 * @brief Open the video if not already opened.
 * @return If the video file is now open,
 *         else the file doesn't exist, is unrecognized or is corrupted.
 */
bool FFMpegReaderPlugin::ensureVideoIsOpen()
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

void FFMpegReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kFFMpegHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kFFMpegHelpString );
	}
	else if( paramName == kFilename )
	{
		_errorInFile = false;
	}
}

void FFMpegReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setOutputFrameVarying( true );
	clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setClipBitDepth( *_clipDst, OFX::eBitDepthUByte ); /// @todo tuttle: some video format may need other bit depth (how we can detect this ?)

	if( !ensureVideoIsOpen() )
		return;

	// options depending on input file
	clipPreferences.setPixelAspectRatio( *_clipDst, _reader.aspectRatio() );
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

bool FFMpegReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	if( !ensureVideoIsOpen() )
		return false;

	range.min = 0.0;
	range.max = (double)_reader.nbFrames();
	return true;
}

bool FFMpegReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	if( !ensureVideoIsOpen() )
		return false;

	rod.x1 = 0;
	rod.x2 = _reader.width() * _reader.aspectRatio();
	rod.y1 = 0;
	rod.y2 = _reader.height();
	return true;
}

void FFMpegReaderPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	ensureVideoIsOpen();
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegReaderPlugin::render( const OFX::RenderArguments& args )
{
	if( !ensureVideoIsOpen() )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown() );
	}

	doGilRender<FFMpegReaderProcess>( *this, args );

//	// instantiate the render code based on the pixel depth of the dst clip
//	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
//	OFX::EPixelComponent components = _clipDst->getPixelComponents();
//
//	switch( components )
//	{
//		case OFX::ePixelComponentRGBA:
//		{
//			doGilRender<BasicKeyerPlugin, boost::gil::rgba_layout_t>( *this, args, bitDepth );
//			return;
//		}
//		case OFX::ePixelComponentRGB:
//		{
//			doGilRender<BasicKeyerPlugin, boost::gil::rgb_layout_t>( *this, args, bitDepth );
//			return;
//		}
//		case OFX::ePixelComponentAlpha:
//		{
//			doGilRender<BasicKeyerPlugin, boost::gil::gray_layout_t>( *this, args, bitDepth );
//			return;
//		}
//		case OFX::ePixelComponentCustom:
//		case OFX::ePixelComponentNone:
//		{
//			BOOST_THROW_EXCEPTION( exception::Unsupported()
//				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
//		}
//	}
//	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

void FFMpegReaderPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_reader.close();
}

}
}
}
}
