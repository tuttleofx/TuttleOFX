#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <string>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <ffmpeg/VideoFFmpegWriter.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

struct FFMpegProcessParams
{
	std::string _filepath;      ///< Ffmpeg filepath
	int _format;				///< Format
	int _codec;					///< Codec
	int _bitrate;				///< Bit rate
};

/**
 * @brief FFMpeg plugin
 */
class FFMpegWriterPlugin : public OFX::ImageEffect
{
public:
    FFMpegWriterPlugin( OfxImageEffectHandle handle );
    OFX::Clip *getSrcClip( ) const;
    OFX::Clip *getDstClip( ) const;
	VideoFFmpegWriter & getWriter();

public:
    virtual void render( const OFX::RenderArguments &args );
    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	FFMpegProcessParams getProcessParams() const;
	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

protected:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip           *_srcClip;       ///< Source image clip
    OFX::Clip           *_dstClip;       ///< Destination image clip
	boost::scoped_ptr<std::string> _openedSource;  ///< Opened video file source
	OFX::StringParam    *_filepath;      ///< Ffmpeg filepath
	OFX::ChoiceParam    *_format;
	OFX::ChoiceParam    *_formatLong;
	OFX::ChoiceParam    *_codec;
	OFX::ChoiceParam    *_codecLong;
	OFX::IntParam       *_bitRate;
	VideoFFmpegWriter	_writer;
};

}
}
}
}

#endif
