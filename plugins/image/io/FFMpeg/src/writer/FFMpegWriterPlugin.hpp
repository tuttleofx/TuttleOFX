#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <boost/scoped_ptr.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

struct FFMpegProcessParams
{
	std::string _filepath;    ///< FFmpeg filepath
	int         _format;      ///< Format
	int         _videoCodec;  ///< Video codec
	int         _audioCodec;  ///< Audio codec
	
	int         _videoPreset; ///< video configuration (based on the video codec)
	int         _audioPreset; ///< video configuration (based on the video codec)
};

/**
 * @brief FFMpeg plugin
 */
class FFMpegWriterPlugin : public WriterPlugin
{
public:
	FFMpegWriterPlugin( OfxImageEffectHandle handle );

public:
	FFMpegProcessParams getProcessParams();

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );

public:
	OFX::ChoiceParam*   _paramFormat;
	OFX::ChoiceParam*   _paramVideoCodec;
	OFX::ChoiceParam*   _paramAudioCodec;
	OFX::IntParam*      _paramBitRate;
	
	std::vector<OFX::ChoiceParam*> _videoCodecPresetParams;
	std::vector<OFX::ChoiceParam*> _audioCodecPresetParams;
	std::vector<std::string>       _videoCodecListWithPreset;
	std::vector<std::string>       _audioCodecListWithPreset;
	
	VideoFFmpegWriter   _writer;
};

}
}
}
}

#endif
