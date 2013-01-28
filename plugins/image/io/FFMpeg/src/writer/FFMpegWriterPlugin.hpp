#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_

#include <ffmpeg/VideoFFmpegWriter.hpp>
#include <ffmpeg/LibAVPresetDefinitions.hpp>

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
	
	PixelFormat _videoPixelFormat; /// videoPixelFormat
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

	void disableAVOptionsForCodecOrFormat( const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec );
	void updatePixelFormat( const std::string& videoCodecName );

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );

private:
	void setParameters( const EAVParamType& type, void* av_class, int req_flags, int rej_flags );
	void setParameters( const EAVParamType& type, const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec );
	void setParameters( const PresetParameters& parameters );
	
public:
	OFX::ChoiceParam*   _paramFormat;
	OFX::ChoiceParam*   _paramVideoCodec;
	OFX::ChoiceParam*   _paramAudioCodec;
	
	OFX::ChoiceParam*   _paramMainPreset;
	OFX::ChoiceParam*   _paramFormatPreset;
	OFX::ChoiceParam*   _paramVideoCodecPreset;
	OFX::ChoiceParam*   _paramAudioCodecPreset;
	
	OFX::IntParam*      _paramBitRate;
	OFX::ChoiceParam*   _paramVideoPixelFormat;
	
	VideoFFmpegWriter   _writer;
	bool                _initWriter;
};

}
}
}
}

#endif
