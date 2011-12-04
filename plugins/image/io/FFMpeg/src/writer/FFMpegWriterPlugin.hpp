#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <boost/scoped_ptr.hpp>

#include <string>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

struct FFMpegProcessParams
{
	std::string _filepath;      ///< Ffmpeg filepath
	int _format;                ///< Format
	int _codec;                 ///< Codec
	int _bitrate;               ///< Bit rate
        int _motionEstimation;      ///< Motion Estimation
};

/**
 * @brief FFMpeg plugin
 */
class FFMpegWriterPlugin : public WriterPlugin
{
public:
	FFMpegWriterPlugin( OfxImageEffectHandle handle );

public:
	FFMpegProcessParams getProcessParams() const;

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );

public:
	OFX::ChoiceParam*   _paramFormat;
	OFX::ChoiceParam*   _paramCodec;
	OFX::IntParam*      _paramBitRate;
        OFX::ChoiceParam*   _paramMotionEstimation;
	
        VideoFFmpegWriter   _writer;
};

}
}
}
}

#endif
