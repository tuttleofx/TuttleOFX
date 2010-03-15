#ifndef _FFMPEG_HPP
#define	_FFMPEG_HPP

extern "C" {
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#include <string>

/**
 * @brief This is a base class for ffmpeg, don't use it directly.
 * It contains only utilities functions.
 */
class FFmpeg
{
protected:
	FFmpeg(){}
	~FFmpeg(){}

public:
	static const std::string ffmpegError_toString( int error );
	static const std::string codecID_toString( const CodecID codec_id );
	static const std::string codecType_toString( const CodecType codec_type );
	static const std::string pixelFormat_toString( const PixelFormat p );

private:
	static bool _hasBeenInit;
	static bool globalInit();
	
};


#endif	/* _FFMPEG_HPP */

