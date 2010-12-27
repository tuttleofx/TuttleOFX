#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

FFMpegWriterPlugin::FFMpegWriterPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

	_filepath          = fetchStringParam( kParamFilename );
	_format            = fetchChoiceParam( kParamFormat );
	_formatLong        = fetchChoiceParam( kParamFormatLong );
	_codec             = fetchChoiceParam( kParamCodec );
	_codecLong         = fetchChoiceParam( kParamCodecLong );
	_bitRate           = fetchIntParam( kParamBitrate );
	_paramRenderAlways = fetchBooleanParam( kParamRenderAlways );
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams() const
{
	FFMpegProcessParams params;

	_filepath->getValue( params._filepath );
	_format->getValue( params._format );
	_codec->getValue( params._codec );
	_bitRate->getValue( params._bitrate );
	return params;
}

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamFFMpegHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kFFMpegHelpString );
	}
	else if( paramName == kParamFormatLong && args.reason == OFX::eChangeUserEdit  )
	{
		_format->setValue( _formatLong->getValue() );
	}
	else if( paramName == kParamFormat && args.reason == OFX::eChangeUserEdit )
	{
		_formatLong->setValue( _format->getValue() );
	}
	else if( paramName == kParamCodecLong && args.reason == OFX::eChangeUserEdit )
	{
		_codec->setValue( _codecLong->getValue() );
	}
	else if( paramName == kParamCodec && args.reason == OFX::eChangeUserEdit )
	{
		_codecLong->setValue( _codec->getValue() );
	}
}

bool FFMpegWriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	if( OFX::getImageEffectHostDescription()->hostIsBackground )
		return false;

	if( _paramRenderAlways->getValue() )
		return false;

	identityClip = _clipSrc;
	identityTime = args.time;
	return true;
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	FFMpegProcessParams params = getProcessParams();

	boost::filesystem::path filepath(params._filepath);
	if( !exists( filepath.parent_path() ) )
	{
		BOOST_THROW_EXCEPTION( exception::NoDirectory( filepath.parent_path().string() ) );
	}

	_writer.filename( params._filepath );
	_writer.setFormat( params._format );
	_writer.setCodec( params._codec );
	_writer.fps( _clipSrc->getFrameRate() );
	_writer.aspectRatio( _clipSrc->getPixelAspectRatio() );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<FFMpegWriterProcess>( *this, args );
	
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
//		case OFX::ePixelComponentAlpha:
//		{
//			doGilRender<BasicKeyerPlugin, boost::gil::gray_layout_t>( *this, args, bitDepth );
//			return;
//		}
//		case OFX::ePixelComponentRGB:
//		case OFX::ePixelComponentCustom:
//		case OFX::ePixelComponentNone:
//		{
//			BOOST_THROW_EXCEPTION( exception::Unsupported()
//				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
//		}
//	}
//	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
}

}
}
}
}
