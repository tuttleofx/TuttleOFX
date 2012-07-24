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
	
	codecListWithPreset = _writer.getCodecListWithConfig();
	std::vector<std::string>::iterator it;
	for( it = codecListWithPreset.begin(); it < codecListWithPreset.end(); it++ )
	{
		OFX::ChoiceParam* paramPreset = fetchChoiceParam( *it );
		videoCodecPresets.push_back( paramPreset );
	}
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams()
{
	FFMpegProcessParams params;

	params._filepath                       = _paramFilepath          ->getValue();
	params._format                         = _paramFormat            ->getValue();
	params._codec                          = _paramCodec             ->getValue();
	params._bitrate                        = _paramBitRate           ->getValue();
	
	_writer.setCodec            ( params._codec );
	const std::string codecName = _writer.getCodec();
	
	std::vector<std::string>::iterator it;
	size_t pos = 0;
	for( it = codecListWithPreset.begin(); it < codecListWithPreset.end(); it++, pos++ )
	{
		if( strcmp( codecName.c_str(), (*it).c_str() ) == 0 )
		{
			int presetIdx = videoCodecPresets.at( pos )->getValue();
			params._videoPreset = presetIdx;
		}
	}
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
	_writer.fps                 ( _clipSrc->getFrameRate() );
	_writer.aspectRatio         ( _clipSrc->getPixelAspectRatio() );
	_writer.setVideoPreset      ( params._videoPreset );
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
