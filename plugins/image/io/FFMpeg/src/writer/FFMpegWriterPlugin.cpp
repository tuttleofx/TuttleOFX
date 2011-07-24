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
	: WriterPlugin( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

	_paramFormat            = fetchChoiceParam( kParamFormat );
	_paramFormatLong        = fetchChoiceParam( kParamFormatLong );
	_paramCodec             = fetchChoiceParam( kParamCodec );
	_paramCodecLong         = fetchChoiceParam( kParamCodecLong );
	_paramBitRate           = fetchIntParam( kParamBitrate );
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams() const
{
	FFMpegProcessParams params;

	_paramFormat->getValue( params._format );
	_paramCodec->getValue( params._codec );
	_paramBitRate->getValue( params._bitrate );
	return params;
}

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );

	if( paramName == kParamFormatLong && args.reason == OFX::eChangeUserEdit  )
	{
		_paramFormat->setValue( _paramFormatLong->getValue() );
	}
	else if( paramName == kParamFormat && args.reason == OFX::eChangeUserEdit )
	{
		_paramFormatLong->setValue( _paramFormat->getValue() );
	}
	else if( paramName == kParamCodecLong && args.reason == OFX::eChangeUserEdit )
	{
		_paramCodec->setValue( _paramCodecLong->getValue() );
	}
	else if( paramName == kParamCodec && args.reason == OFX::eChangeUserEdit )
	{
		_paramCodecLong->setValue( _paramCodec->getValue() );
	}
}

bool FFMpegWriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	return WriterPlugin::isIdentity( args, identityClip, identityTime );
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	WriterPlugin::beginSequenceRender( args );

	FFMpegProcessParams params = getProcessParams();

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
