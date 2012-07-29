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

	_paramFormat                      = fetchChoiceParam( kParamFormat           );
	_paramCodec                       = fetchChoiceParam( kParamCodec            );
	_paramBitRate                     = fetchIntParam   ( kParamBitrate          );
	_paramMotionEstimation            = fetchChoiceParam( kParamMotionEstimation );
	_paramColorspace                  = fetchChoiceParam( kParamColorSpace       );
	_paramColorPrimaries              = fetchChoiceParam( kParamColorPrimaries   );
	_paramColorTransferCharacteristic = fetchChoiceParam( kParamColorTRC         );
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams() const
{
	FFMpegProcessParams params;

	params._filepath                       = _paramFilepath          ->getValue();
	params._format                         = _paramFormat            ->getValue();
	params._codec                          = _paramCodec             ->getValue();
	params._bitrate                        = _paramBitRate           ->getValue();
	params._motionEstimation               = _paramMotionEstimation  ->getValue() + 1; // Motion_Est_ID start a 1
	params._colorspace                     = (AVColorSpace)                  _paramColorspace                  ->getValue();
	params._colorPrimaries                 = (AVColorPrimaries)              _paramColorPrimaries              ->getValue();
	params._colorTransferCharacteristic    = (AVColorTransferCharacteristic) _paramColorTransferCharacteristic ->getValue();

	return params;
}

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
}

void FFMpegWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( true );
}

bool FFMpegWriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	return WriterPlugin::isIdentity( args, identityClip, identityTime );
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	WriterPlugin::beginSequenceRender( args );

	FFMpegProcessParams params = getProcessParams();

	_writer.filename            ( params._filepath );
	_writer.setFormat           ( params._format );
	_writer.setCodec            ( params._codec );
	_writer.setBitRate          ( params._bitrate );
	_writer.setColorspace       ( params._colorspace );
	_writer.setColorPrimaries   ( params._colorPrimaries );
	_writer.setColorPrimaries   ( params._colorPrimaries );
	_writer.setMotionEstimation ( params._motionEstimation );
	_writer.fps                 ( _clipSrc->getFrameRate() );
	_writer.aspectRatio         ( _clipSrc->getPixelAspectRatio() );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

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
