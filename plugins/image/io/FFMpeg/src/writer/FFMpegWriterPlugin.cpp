#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/foreach.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

FFMpegWriterPlugin::FFMpegWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

	_paramFormat                      = fetchChoiceParam( kParamFormat );
	_paramCodec                       = fetchChoiceParam( kParamCodec );
	_paramBitRate                     = fetchIntParam   ( kParamBitrate );
	
	_codecListWithPreset = _writer.getPresets().getCodecListWithConfig();
	BOOST_FOREACH( const std::string& s, _codecListWithPreset )
	{
		OFX::ChoiceParam* paramPreset = fetchChoiceParam( s + kParamPresetPostfix );
		_videoCodecPresetParams.push_back( paramPreset );
	}
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams()
{
	FFMpegProcessParams params;

	params._filepath = _paramFilepath->getValue();
	params._format = _paramFormat->getValue();
	params._codec = _paramCodec->getValue();
	params._bitrate = _paramBitRate->getValue();
	
	_writer.setCodec( params._codec );
	const std::string codecPreset = _writer.getCodec();
	
	params._videoPreset = -1;
	std::vector<OFX::ChoiceParam*>::const_iterator choiceParamIt = _videoCodecPresetParams.begin();
	BOOST_FOREACH( const std::string& codecName, _codecListWithPreset )
	{
		if( codecPreset == codecName )
		{
			const int presetIdx = (*choiceParamIt)->getValue();
			params._videoPreset = presetIdx;
		}
		++choiceParamIt;
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

	_writer.setFilename( params._filepath );
	_writer.setFormat( params._format );
	_writer.setCodec( params._codec );
	_writer.setBitRate( params._bitrate );
	_writer.setFps( _clipSrc->getFrameRate() );
	_writer.setAspectRatio( _clipSrc->getPixelAspectRatio() );
	_writer.setVideoPreset( params._videoPreset );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );
	
	doGilRender<FFMpegWriterProcess>( *this, args );
}

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
}

}
}
}
}
