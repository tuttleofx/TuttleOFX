#include "VideoFFmpegWriter.hpp"

#include <tuttle/plugin/global.hpp>

#include <boost/cstdint.hpp>

VideoFFmpegWriter::VideoFFmpegWriter()
	: _avformatOptions( 0 )
	, _sws_context( NULL )
	, _stream( 0 )
	, _error( IGNORE_FINISH )
	, _filename( "" )
	, _width( 0 )
	, _height( 0 )
	, _aspectRatio( 1 )
	, _out_pixelFormat( PIX_FMT_YUV420P )
	, _fps( 25.0f )
	, _format( "default" )
	, _codec( "default" )
	, _bitRate( 400000 )
	, _bitRateTolerance( 4000 * 10000 )
	, _gopSize( 12 )
	, _bFrames( 0 )
	, _mbDecision( FF_MB_DECISION_SIMPLE )
{
	av_log_set_level( AV_LOG_WARNING );
	av_register_all();

	for( int i = 0; i < CODEC_TYPE_NB; ++i )
		_avctxOptions[i] = avcodec_alloc_context2( CodecType( i ) );

	_formatsLongNames.push_back( std::string( "default" ) );
	_formatsShortNames.push_back( std::string(  "default" ) );
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

	_codecsLongNames.push_back( std::string( "default" ) );
	_codecsShortNames.push_back( std::string( "default" ) );
	AVCodec* c = av_codec_next( NULL );
	while( c )
	{
		if( c->type == CODEC_TYPE_VIDEO && c->encode )
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

VideoFFmpegWriter::~VideoFFmpegWriter()
{
	for( int i = 0; i < CODEC_TYPE_NB; ++i )
		av_free( _avctxOptions[i] );
}

int VideoFFmpegWriter::execute( boost::uint8_t* in_buffer, int in_width, int in_height, PixelFormat in_pixelFormat )
{
	_error = IGNORE_FINISH;

	AVOutputFormat* fmt = 0;
	fmt = guess_format( _format.c_str(), NULL, NULL );
	if( !fmt )
	{
		fmt = guess_format( NULL, filename().c_str(), NULL );
		if( !fmt )
		{
			std::cerr << "ffmpegWriter: could not deduce output format from file extension." << std::endl;
			return false;
		}
	}

	if( !_avformatOptions )
		_avformatOptions = avformat_alloc_context();

	_avformatOptions->oformat = fmt;
	snprintf( _avformatOptions->filename, sizeof( _avformatOptions->filename ), "%s", filename().c_str() );

	if( !_stream )
	{
		_stream = av_new_stream( _avformatOptions, 0 );
		if( !_stream )
		{
			std::cout << "ffmpegWriter: out of memory." << std::endl;
			return false;
		}

		CodecID codecId    = fmt->video_codec;
		AVCodec* userCodec = avcodec_find_encoder_by_name( _codec.c_str() );
		if( userCodec )
			codecId = userCodec->id;

		_stream->codec->codec_id           = codecId;
		_stream->codec->codec_type         = CODEC_TYPE_VIDEO;
		_stream->codec->bit_rate           = _bitRate;
		_stream->codec->bit_rate_tolerance = _bitRateTolerance;
		_stream->codec->width              = width();
		_stream->codec->height             = height();
		_stream->codec->time_base          = av_d2q( 1.0 / _fps, 100 );
		_stream->codec->gop_size           = _gopSize;
		if( _bFrames )
		{
			_stream->codec->max_b_frames     = _bFrames;
			_stream->codec->b_frame_strategy = 0;
			_stream->codec->b_quant_factor   = 2.0;
		}
		_stream->codec->mb_decision = _mbDecision;
		_stream->codec->pix_fmt     = _out_pixelFormat;

		if( !strcmp( _avformatOptions->oformat->name, "mp4" ) || !strcmp( _avformatOptions->oformat->name, "mov" ) || !strcmp( _avformatOptions->oformat->name, "3gp" ) || !strcmp( _avformatOptions->oformat->name, "flv" ) )
			_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if( av_set_parameters( _avformatOptions, NULL ) < 0 )
		{
			std::cout << "ffmpegWriter: unable to set parameters." << std::endl;
			freeFormat();
			return false;
		}

		dump_format( _avformatOptions, 0, filename().c_str(), 1 );

		AVCodec* videoCodec = avcodec_find_encoder( codecId );
		if( !videoCodec )
		{
			std::cout << "ffmpegWriter: unable to find codec." << std::endl;
			freeFormat();
			return false;
		}

		if( avcodec_open( _stream->codec, videoCodec ) < 0 )
		{
			std::cout << "ffmpegWriter: unable to open codec." << std::endl;
			freeFormat();
			return false;
		}

		if( !( fmt->flags & AVFMT_NOFILE ) )
		{
			if( url_fopen( &_avformatOptions->pb, filename().c_str(), URL_WRONLY ) < 0 )
			{
				std::cout << "ffmpegWriter: unable to open file." << std::endl;
				return false;
			}
		}

		av_write_header( _avformatOptions );
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

	std::cout << "ffmpegWriter: input format: " << pixelFormat_toString( in_pixelFormat ) << std::endl;
	std::cout << "ffmpegWriter: output format: " << pixelFormat_toString( _out_pixelFormat ) << std::endl;

	if( !_sws_context )
	{
		std::cout << "ffmpeg-conversion failed (" << in_pixelFormat << "->" << _out_pixelFormat << ")." << std::endl;
		return false;
	}
	int error = sws_scale( _sws_context, in_frame->data, in_frame->linesize, 0, height(), out_frame->data, out_frame->linesize );
	if( error < 0 )
	{
		std::cout << "ffmpeg-conversion failed (" << in_pixelFormat << "->" << _out_pixelFormat << ")." << std::endl;
		return false;
	}

	int ret = 0;
	if( ( _avformatOptions->oformat->flags & AVFMT_RAWPICTURE ) != 0 )
	{
		AVPacket pkt;
		av_init_packet( &pkt );
		pkt.flags       |= PKT_FLAG_KEY;
		pkt.stream_index = _stream->index;
		pkt.data         = (boost::uint8_t*) out_frame;
		pkt.size         = sizeof( AVPicture );
		ret              = av_interleaved_write_frame( _avformatOptions, &pkt );
	}
	else
	{
		boost::uint8_t* out_buffer = (boost::uint8_t*) av_malloc( out_picSize );

		ret = avcodec_encode_video( _stream->codec, out_buffer, out_picSize, out_frame );

		if( ret > 0 )
		{
			AVPacket pkt;
			av_init_packet( &pkt );

			if( _stream->codec->coded_frame && _stream->codec->coded_frame->pts != static_cast<boost::int64_t>( AV_NOPTS_VALUE ) ) // static_cast<unsigned long> (
				pkt.pts = av_rescale_q( _stream->codec->coded_frame->pts, _stream->codec->time_base, _stream->time_base );

			if( _stream->codec->coded_frame && _stream->codec->coded_frame->key_frame )
				pkt.flags |= PKT_FLAG_KEY;

			pkt.stream_index = _stream->index;
			pkt.data         = out_buffer;
			pkt.size         = ret;
			ret              = av_interleaved_write_frame( _avformatOptions, &pkt );
		}

		av_free( out_buffer );
	}

	av_free( out_buffer );
	av_free( out_frame );
	av_free( in_frame );
	// in_buffer not free (function parameter)

	if( ret )
	{
		std::cout << "ffmpegWriter: error writing frame to file." << std::endl;
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
		url_fclose( _avformatOptions->pb );
	freeFormat();
}

void VideoFFmpegWriter::freeFormat()
{
	for( int i = 0; i < static_cast<int>( _avformatOptions->nb_streams ); ++i )
		av_freep( &_avformatOptions->streams[i] );
	av_free( _avformatOptions );
	_avformatOptions = 0;
	_stream          = 0;
}

