#include "FFmpeg.hpp"

#include <iostream>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

bool FFmpeg::_hasBeenInit = globalInit( );

void log_callback( void* /*ptr*/, int level, const char* format, va_list arglist )
{
	//	AVClass* p = (AVClass*)ptr;
	std::cerr << "FFmpegWriter: (" << level << ")";
	vfprintf( stderr, format, arglist );
	std::cerr << std::endl;
}

bool FFmpeg::globalInit( )
{
	av_register_all();
#ifdef TUTTLE_PRODUCTION
	// av_log_set_level( AV_LOG_QUIET );
	av_log_set_level( AV_LOG_ERROR );
#else
	av_log_set_level( AV_LOG_WARNING );
	//	av_log_set_level( AV_LOG_DEBUG );
#endif
	av_log_set_callback( log_callback );
	return true;
}

#define CASE_RETURN_STRING( e ) case e: return # e

const std::string FFmpeg::ffmpegError_toString( int error )
{
	switch( error )
	{
#if LIBAVCODEC_VERSION_MAJOR > 52
		case AVERROR_BSF_NOT_FOUND:
			return "Bitstream filter not found";
		case AVERROR_DECODER_NOT_FOUND:
			return "Decoder not found";
		case AVERROR_DEMUXER_NOT_FOUND:
			return "Demuxer not found";
		case AVERROR_ENCODER_NOT_FOUND:
			return "Encoder not found";
		case AVERROR_EXIT:
			return "Immediate exit was requested; the called function should not be restarted";
		case AVERROR_FILTER_NOT_FOUND:
			return "Filter not found";
		case AVERROR_MUXER_NOT_FOUND:
			return "Muxer not found";
		case AVERROR_OPTION_NOT_FOUND:
			return "Option not found";
		case AVERROR_PROTOCOL_NOT_FOUND:
			return "Protocol not found";
		case AVERROR_STREAM_NOT_FOUND:
			return "Stream not found";
#endif
		case AVERROR_EOF:
			return "End of file";
		case AVERROR_INVALIDDATA:
			return "Invalid data found when processing input";
		case AVERROR_PATCHWELCOME:
			return "Not yet implemented in FFmpeg, patches welcome";
		default:
			return "unknown error";
	}
}

const std::string FFmpeg::codecType_toString( const AVMediaType codec_type )
{
	switch( codec_type )
	{
		case AVMEDIA_TYPE_VIDEO:
			return "AVMEDIA_TYPE_VIDEO";
		case AVMEDIA_TYPE_AUDIO:
			return "AVMEDIA_TYPE_AUDIO";
		case AVMEDIA_TYPE_UNKNOWN:
			return "AVMEDIA_TYPE_UNKNOWN";
		case AVMEDIA_TYPE_DATA:
			return "AVMEDIA_TYPE_DATA";
		case AVMEDIA_TYPE_SUBTITLE:
			return "AVMEDIA_TYPE_SUBTITLE";
		case AVMEDIA_TYPE_ATTACHMENT:
			return "AVMEDIA_TYPE_ATTACHMENT";
		case AVMEDIA_TYPE_NB:
			return "AVMEDIA_TYPE_NB";
	}
	return "CODEC_TYPE not handle.";
}

}
}
}

