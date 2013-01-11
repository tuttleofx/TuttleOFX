#ifndef _TUTTLE_PLUGIN_FFMPEG_FFMPEG_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_FFMPEG_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/pixdesc.h>
	#include <libswscale/swscale.h>
}

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

typedef struct AVPrivOption
{
	AVOption o;
	std::string class_name;
} AVPrivOption;

enum EAVParamType
{
	eAVParamFormat = 0,
	eAVParamVideo,
	eAVParamAudio
};

/**
 * @brief This is a base class for ffmpeg, don't use it directly.
 * It contains only utilities functions.
 */
class FFmpeg
{
protected:
	FFmpeg() {}
	virtual ~FFmpeg() {}

public:
	static const std::string ffmpegError_toString( int error );
	static const std::string ffmpegLogLevel_toString( int logLevel );
	static const std::string codecType_toString( const AVMediaType codec_type );

	std::vector<AVPrivOption> getAVOptions(const AVClass *av_class);
	
	void getPixelsFormatList();
	
private:
	static bool _hasBeenInit;
	static bool globalInit();

};

}
}
}

#endif

