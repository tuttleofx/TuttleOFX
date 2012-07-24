#include "VideoFFmpegWriter.hpp"

#include <tuttle/plugin/global.hpp>

#include <boost/cstdint.hpp>
#include <cstdio>

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT( 52, 40, 0 )
// compatibility with previous versions of libavformat
#define av_guess_format guess_format
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

static boost::int64_t pts = 0;

VideoFFmpegWriter::VideoFFmpegWriter()
	: FFmpeg             ( )
	, FFmpegPreset       ( )
	, _avformatOptions   ( NULL )
	, _sws_context       ( NULL )
	, _stream            ( NULL )
	, _codec             ( NULL )
	, _ofmt              ( NULL )
	, _error             ( IGNORE_FINISH )
	, _filename          ( "" )
	, _width             ( 0 )
	, _height            ( 0 )
	, _aspectRatio       ( 1 )
	, _out_pixelFormat   ( PIX_FMT_YUV420P )
	, _fps               ( 25.0f )
	, _formatName        ( "" )
	, _codecName         ( "" )
	, _videoPresetName   ( "" )
	, _bitRate           ( 400000 )
	, _bitRateTolerance  ( 4000 * 10000 )
	, _gopSize           ( 12 )
	, _bFrames           ( 0 )
	, _mbDecision        ( FF_MB_DECISION_SIMPLE )
{
	av_log_set_level( AV_LOG_WARNING );
	av_register_all();

	AVOutputFormat* fmt = av_oformat_next( NULL );
	while( fmt )
	{
		if( fmt->video_codec != CODEC_ID_NONE )
		{
			if( fmt->long_name )
			{
				_formatsLongNames.push_back( std::string( fmt->long_name ) + std::string( " (" ) + std::string( fmt->name ) + std::string( ")" ) );
				_formatsShortNames.push_back( std::string( fmt->name ) );
			}
		}
		fmt = av_oformat_next( fmt );
	}

	AVCodec* c = av_codec_next( NULL );
	while( c )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( c->type == AVMEDIA_TYPE_VIDEO && c->encode2 )
#else
		if( c->type == AVMEDIA_TYPE_VIDEO && c->encode )
#endif
		{
			if( c->long_name )
			{
				_codecsLongNames.push_back( std::string( c->long_name ) );
				_codecsShortNames.push_back( std::string( c->name ) );
			}
		}
		c = av_codec_next( c );
	}
}

int VideoFFmpegWriter::execute( boost::uint8_t* in_buffer, int in_width, int in_height, PixelFormat in_pixelFormat )
{
	_error = IGNORE_FINISH;

	if( !_avformatOptions )
	{
		// TODO avformat_alloc_context2 can guess format from filename
		// if format name is NULL, find a way to expose the feature
		if (avformat_alloc_output_context2(&_avformatOptions, NULL, _formatName.c_str(), filename().c_str()) < 0)
		{
			TUTTLE_CERR( "ffmpegWriter: output context allocation failed" );
			return false;
		}
		_ofmt = _avformatOptions->oformat;
		TUTTLE_CERR( "ffmpegWriter: " << std::string(_ofmt->name) << " format selected" );
	}

	if( !_stream )
	{
		_codec = avcodec_find_encoder_by_name( _codecName.c_str() );
		if (!_codec)
		{
			TUTTLE_CERR( "ffmpegWriter: codec not found" );
			return false;
		}
		TUTTLE_CERR( "ffmpegWriter: " << std::string(_codec->name) << " codec selected" );

		_stream = avformat_new_stream( _avformatOptions, _codec );
		if( !_stream )
		{
			TUTTLE_CERR( "ffmpegWriter: out of memory." );
			return false;
		}
		avcodec_get_context_defaults3(_stream->codec, _codec);

		if( _videoPresetName.length() !=0 )
		{
			TUTTLE_CERR( "ffmpegWriter: " << _videoPresetName << " preset selected" );
		}
		
		_stream->codec->bit_rate           = _bitRate;
		_stream->codec->bit_rate_tolerance = _bitRateTolerance;
		_stream->codec->width              = width();
		_stream->codec->height             = height();
		_stream->codec->time_base          = av_d2q( 1.0 / _fps, 100 );
		_stream->codec->gop_size           = _gopSize;
		_stream->codec->sample_rate        = 48000; ///< samples per second
		_stream->codec->channels           = 0;     ///< number of audio channels
		if( _bFrames )
		{
			_stream->codec->max_b_frames     = _bFrames;
			_stream->codec->b_frame_strategy = 0;
			_stream->codec->b_quant_factor   = 2.0;
		}
		_stream->codec->mb_decision = _mbDecision;

		int pixfmt_allowed = 0, k;
		if ( _codec->pix_fmts )
		{
			for ( k = 0; _codec->pix_fmts[k] != PIX_FMT_NONE; k++ )
			{
				if ( _codec->pix_fmts[k] == _out_pixelFormat )
				{
					pixfmt_allowed = 1;
					break;
				}
			}
		}
		else
		{
			// If a codec does not contain a list of supported pixel
			// formats, just assume that _out_PixelFormat is valid
			pixfmt_allowed = 1;
		}

		if ( !pixfmt_allowed )
		{
			// av_get_pix_fmt_name requires lavu 51.3.0 or higher
			TUTTLE_CERR( "ffmpegWriter: pixel format " << av_get_pix_fmt_name(_out_pixelFormat) << " not available in codec" );
			_out_pixelFormat = _codec->pix_fmts[0];
			TUTTLE_CERR( "ffmpegWriter: auto-selecting " << av_get_pix_fmt_name(_out_pixelFormat) );
		}
		_stream->codec->pix_fmt     = _out_pixelFormat;

		if( !strcmp( _avformatOptions->oformat->name, "mp4" ) || !strcmp( _avformatOptions->oformat->name, "mov" ) || !strcmp( _avformatOptions->oformat->name, "3gp" ) || !strcmp( _avformatOptions->oformat->name, "flv" ) )
			_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		av_dump_format( _avformatOptions, 0, filename().c_str(), 1 );

		if( avcodec_open2( _stream->codec, _codec, NULL ) < 0 )
		{
			TUTTLE_CERR( "ffmpegWriter: unable to open codec." );
			freeFormat();
			return false;
		}

		if( !( _ofmt->flags & AVFMT_NOFILE ) )
		{
			if( avio_open2( &_avformatOptions->pb, filename().c_str(),
                                        AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
			{
				TUTTLE_CERR( "ffmpegWriter: unable to open file." );
				freeFormat();
				return false;
			}
		}

		avformat_write_header( _avformatOptions, NULL );
	}

	_error = CLEANUP;

	AVFrame* in_frame = avcodec_alloc_frame();
	avcodec_get_frame_defaults( in_frame );
	avpicture_fill( (AVPicture*)in_frame, in_buffer, in_pixelFormat, in_width, in_height );

	AVFrame* out_frame = avcodec_alloc_frame();
	avcodec_get_frame_defaults( out_frame );
	int out_picSize            = avpicture_get_size( _out_pixelFormat, width(), height() );
	boost::uint8_t* out_buffer = (boost::uint8_t*) av_malloc( out_picSize );
	avpicture_fill( (AVPicture*) out_frame, out_buffer, _out_pixelFormat, width(), height() );

	_sws_context = sws_getCachedContext( _sws_context, in_width, in_height, in_pixelFormat, width(), height(), _out_pixelFormat, SWS_BICUBIC, NULL, NULL, NULL );

	TUTTLE_COUT( "ffmpegWriter: input format: " << av_get_pix_fmt_name( in_pixelFormat ) );
	TUTTLE_COUT( "ffmpegWriter: output format: " << av_get_pix_fmt_name( _out_pixelFormat ) );

	if( !_sws_context )
	{
		TUTTLE_CERR( "ffmpeg-conversion failed (" << in_pixelFormat << "->" << _out_pixelFormat << ")." );
		return false;
	}
	int error = sws_scale( _sws_context, in_frame->data, in_frame->linesize, 0, height(), out_frame->data, out_frame->linesize );
	if( error < 0 )
	{
		TUTTLE_CERR( "ffmpeg-conversion failed (" << in_pixelFormat << "->" << _out_pixelFormat << ")." );
		return false;
	}

	int ret = 0;
	if( ( _avformatOptions->oformat->flags & AVFMT_RAWPICTURE ) != 0 )
	{
		AVPacket pkt;
		av_init_packet( &pkt );
		pkt.flags       |= AV_PKT_FLAG_KEY;
		pkt.stream_index = _stream->index;
		pkt.data         = (boost::uint8_t*) out_frame;
		pkt.size         = sizeof( AVPicture );
		ret              = av_interleaved_write_frame( _avformatOptions, &pkt );
	}
	else
	{
		AVPacket pkt;
		int hasFrame = 0;
		av_init_packet( &pkt );
		pkt.size = 0;
		pkt.data = NULL;
		pkt.stream_index = _stream->index;

		if( _stream->codec->coded_frame && _stream->codec->coded_frame->pts != static_cast<boost::int64_t>( AV_NOPTS_VALUE ) ) // static_cast<unsigned long> (
			pkt.pts = av_rescale_q( _stream->codec->coded_frame->pts, _stream->codec->time_base, _stream->time_base );

		if( _stream->codec->coded_frame && _stream->codec->coded_frame->key_frame )
			pkt.flags |= AV_PKT_FLAG_KEY;

		out_frame->pts = pts++;
		ret = avcodec_encode_video2( _stream->codec, &pkt, out_frame,  &hasFrame );
		if ( ret < 0 )
			return false;

		if ( hasFrame )
		{
			ret = av_interleaved_write_frame( _avformatOptions, &pkt );
			if ( ret < 0 )
			{
				TUTTLE_CERR( "ffmpegWriter: error writing packet to file" );
				return false;
			}
		}
	}

	av_free( out_buffer );
	av_free( out_frame );
	av_free( in_frame );
	// in_buffer not free (function parameter)

	if( ret )
	{
		TUTTLE_CERR( "ffmpegWriter: error writing frame to file." );
		return false;
	}

	_error = SUCCESS;
	return true;
}

void VideoFFmpegWriter::finish()
{
	if( _error == IGNORE_FINISH )
		return;
	av_write_trailer( _avformatOptions );
	avcodec_close( _stream->codec );
	if( !( _avformatOptions->oformat->flags & AVFMT_NOFILE ) )
		avio_close( _avformatOptions->pb );
	freeFormat();
}

void VideoFFmpegWriter::freeFormat()
{
	avcodec_close( _stream->codec );
	for( int i = 0; i < static_cast<int>( _avformatOptions->nb_streams ); ++i )
		av_freep( &_avformatOptions->streams[i] );
	av_free( _avformatOptions );
	_avformatOptions = 0;
	_stream          = 0;
}

void VideoFFmpegWriter::setVideoPreset( const unsigned int id )
{
	FFmpegPreset::Presets p;
	p = getConfigList( _codecName );
	
	_videoPresetName = p.at( id );
}
