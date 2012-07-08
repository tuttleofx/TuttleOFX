#include "FFmpeg.hpp"


bool FFmpeg::_hasBeenInit = globalInit();

bool FFmpeg::globalInit()
{
        //	av_log_set_level( AV_LOG_QUIET );
        av_log_set_level( AV_LOG_ERROR );
        //	av_log_set_level( AV_LOG_WARNING );
        //	av_log_set_level( AV_LOG_DEBUG );
        av_register_all();
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


/*
 #define VALUE_AND_NAME(e) e, #e

   std::map<int, std::string> formats = boost::assign::map_list_of
   ( VALUE_AND_NAME( PIX_FMT_NONE ) )
   ( VALUE_AND_NAME( PIX_FMT_YUV420P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUYV422 ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB24 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR24 ) )
   ( VALUE_AND_NAME( PIX_FMT_YUV422P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUV444P ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB32 ) )
   ( VALUE_AND_NAME( PIX_FMT_YUV410P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUV411P ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB565 ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB555 ) )
   ( VALUE_AND_NAME( PIX_FMT_GRAY8 ) )
   ( VALUE_AND_NAME( PIX_FMT_MONOWHITE ) )
   ( VALUE_AND_NAME( PIX_FMT_MONOBLACK ) )
   ( VALUE_AND_NAME( PIX_FMT_PAL8 ) )
   ( VALUE_AND_NAME( PIX_FMT_YUVJ420P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUVJ422P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUVJ444P ) )
   ( VALUE_AND_NAME( PIX_FMT_XVMC_MPEG2_MC ) )
   ( VALUE_AND_NAME( PIX_FMT_XVMC_MPEG2_IDCT ) )
   ( VALUE_AND_NAME( PIX_FMT_UYVY422 ) )
   ( VALUE_AND_NAME( PIX_FMT_UYYVYY411 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR32 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR565 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR555 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR8 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR4 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR4_BYTE ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB8 ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB4 ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB4_BYTE ) )
   ( VALUE_AND_NAME( PIX_FMT_NV12 ) )
   ( VALUE_AND_NAME( PIX_FMT_NV21 ) )

   ( VALUE_AND_NAME( PIX_FMT_RGB32_1 ) )
   ( VALUE_AND_NAME( PIX_FMT_BGR32_1 ) )

   ( VALUE_AND_NAME( PIX_FMT_GRAY16BE ) )
   ( VALUE_AND_NAME( PIX_FMT_GRAY16LE ) )
   ( VALUE_AND_NAME( PIX_FMT_YUV440P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUVJ440P ) )
   ( VALUE_AND_NAME( PIX_FMT_YUVA420P ) )
   ( VALUE_AND_NAME( PIX_FMT_VDPAU_H264 ) )
   ( VALUE_AND_NAME( PIX_FMT_VDPAU_MPEG1 ) )
   ( VALUE_AND_NAME( PIX_FMT_VDPAU_MPEG2 ) )
   ( VALUE_AND_NAME( PIX_FMT_VDPAU_WMV3 ) )
   ( VALUE_AND_NAME( PIX_FMT_VDPAU_VC1 ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB48BE ) )
   ( VALUE_AND_NAME( PIX_FMT_RGB48LE ) )
   ( VALUE_AND_NAME( PIX_FMT_VAAPI_MOCO ) )
   ( VALUE_AND_NAME( PIX_FMT_VAAPI_IDCT ) )
   ( VALUE_AND_NAME( PIX_FMT_VAAPI_VLD ) )
   ( VALUE_AND_NAME( PIX_FMT_NB ) );
 */

