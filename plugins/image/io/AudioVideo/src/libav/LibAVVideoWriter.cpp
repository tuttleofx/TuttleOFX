#include "LibAVVideoWriter.hpp"
#include "LibAVPreset.hpp"
#include "LibAVFormatPreset.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/foreach.hpp>

#include <cstdio>


namespace tuttle {
namespace plugin {
namespace av {

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT( 52, 40, 0 )
// compatibility with previous versions of libavformat
#define av_guess_format guess_format
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

static boost::int64_t pts = 0;

LibAVVideoWriter::LibAVVideoWriter()
	: LibAV()
	, _avFormatOptions   ( NULL )
	, _stream            ( NULL )
	, _avVideoOptions    ( NULL )
	, _avAudioOptions    ( NULL )
	, _sws_context       ( NULL )
	, _videoCodec        ( NULL )
	, _audioCodec        ( NULL )
	, _ofmt              ( NULL )
	, _statusCode        ( eWriterStatusIgnoreFinish )
	, _filename          ( "" )
	, _width             ( 0 )
	, _height            ( 0 )
	, _aspectRatio       ( 1 )
	, _out_pixelFormat   ( PIX_FMT_YUV420P )
	, _fps               ( 25.0f )
	, _formatName        ( "" )
	, _videoCodecName    ( "" )
	, _audioCodecName    ( "" )
{
	AVOutputFormat* fmt = av_oformat_next( NULL );
	
	while( fmt )
	{
		// add only format with video track
		if( fmt->video_codec != AV_CODEC_ID_NONE )
		{
			if( fmt->long_name )
			{
				_formatsLongNames.push_back( std::string( fmt->long_name ) + std::string( " (" ) + std::string( fmt->name ) + std::string( ")" ) );
				_formatsShortNames.push_back( std::string( fmt->name ) );
			}
			if( fmt->priv_class )
			{
				const AVOption *o = NULL;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 51, 12, 0 )
				while( ( o = av_next_option( &fmt->priv_class, o ) ) )
#else
				while( ( o = av_opt_next( &fmt->priv_class, o ) ) )
#endif
				{
					AVPrivOption avprivopt;
					avprivopt.o = *o;
					avprivopt.class_name = std::string( fmt->name );
					_formatPrivOpts.push_back( avprivopt );
				}
			}
		}
		fmt = av_oformat_next( fmt );
	}

	AVCodec* c = NULL;
	while( ( c = av_codec_next( c ) ) )
	{
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT( 53, 34, 0 )
		if( c->encode2 )
#else
		if( c->encode )
#endif
		{
			switch( c->type )
			{
				case AVMEDIA_TYPE_VIDEO:
				{
					if( c->long_name )
					{
						_videoCodecsLongNames.push_back( std::string( c->long_name ) );
						_videoCodecsShortNames.push_back( std::string( c->name ) );
					}
					if( c->priv_class )
					{
						const AVOption *o = NULL;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 51, 12, 0 )
						while( ( o = av_next_option( &c->priv_class, o ) ) )
#else
						while( ( o = av_opt_next( &c->priv_class, o ) ) )
#endif
						{
							AVPrivOption avprivopt;
							avprivopt.o = *o;
							avprivopt.class_name = std::string( c->name );
							_videoCodecPrivOpts.push_back( avprivopt );
						}
					}
					break;
				}
				case AVMEDIA_TYPE_AUDIO:
				{
					if( c->long_name )
					{
						_audioCodecsLongNames.push_back( std::string( c->long_name ) );
						_audioCodecsShortNames.push_back( std::string( c->name ) );
					}
					if( c->priv_class )
					{
						const AVOption *o = NULL;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 51, 12, 0 )
						while( ( o = av_next_option( &c->priv_class, o ) ) )
#else
						while( ( o = av_opt_next( &c->priv_class, o ) ) )
#endif
						{
							AVPrivOption avprivopt;
							avprivopt.o = *o;
							avprivopt.class_name = std::string( c->name );
							_audioCodecPrivOpts.push_back( avprivopt );
						}
					}
					break;
				}
				default:
					break;
			}
		}
	}

}

int LibAVVideoWriter::start( )
{
	if( !_avFormatOptions )
	{
		_avFormatOptions = avformat_alloc_context();
		_ofmt = NULL;
		
		const char* filename = getFilename().c_str();
		
		if( !_avFormatOptions )
		{
			TUTTLE_LOG_ERROR( "avWriter: format context allocation failed" );
			return false;
		}
		
		if( !_formatName.empty() )
		{
			_ofmt = av_guess_format( _formatName.c_str(), NULL, NULL );
			if (!_ofmt)
			{
				TUTTLE_LOG_ERROR( "avWriter: Requested output format " << _formatName << " is not a suitable output format" );
				return false;
			}
		}
		else
		{
			_ofmt = av_guess_format( NULL, filename, NULL );
			if (!_ofmt)
			{
				TUTTLE_LOG_ERROR( "avWriter: Unable to find a suitable output format for " << filename );
				return false;
			}
		}
		
		_avFormatOptions->oformat = _ofmt;
		if( _avFormatOptions->oformat->priv_data_size > 0 )
		{
			_avFormatOptions->priv_data = av_mallocz( _avFormatOptions->oformat->priv_data_size );
			if( !_avFormatOptions->priv_data )
			{
				TUTTLE_LOG_ERROR( "avWriter: format context allocation failed" );
				return false;
			}
			if( _avFormatOptions->oformat->priv_class )
			{
				*(const AVClass**)_avFormatOptions->priv_data= _avFormatOptions->oformat->priv_class;
				av_opt_set_defaults( _avFormatOptions->priv_data );
			}
		}
		else
			_avFormatOptions->priv_data = NULL;
		
		if( filename )
			av_strlcpy( _avFormatOptions->filename, filename, sizeof( _avFormatOptions->filename ) );

		TUTTLE_LOG_TRACE( "avWriter: " << std::string( _ofmt->name ) << " format selected" );
	}

	if( !_stream )
	{
		_videoCodec = avcodec_find_encoder_by_name( _videoCodecName.c_str() );
		if (!_videoCodec)
		{
			BOOST_THROW_EXCEPTION( exception::Format()
				<< exception::user( "avWriter: codec not found." ) );
		}
		TUTTLE_LOG_TRACE( "avWriter: " << std::string(_videoCodec->name) << " codec selected" );
		
		_avVideoOptions = avcodec_alloc_context3( _videoCodec );

		_stream = avformat_new_stream( _avFormatOptions, _videoCodec );
		if( !_stream )
		{
			BOOST_THROW_EXCEPTION( exception::File()
				<< exception::user( "avWriter: out of memory." ) );
		}
		avcodec_get_context_defaults3(_stream->codec, _videoCodec);

//		TUTTLE_LOG_VAR2( TUTTLE_INFO, _videoCodecName, _videoPresetName );
		
		_stream->codec->width              = getWidth();
		_stream->codec->height             = getHeight();
		_stream->codec->time_base          = av_inv_q( av_d2q( _fps, INT_MAX ) );
		//TUTTLE_LOG_VAR( TUTTLE_INFO,  _fps );
		//TUTTLE_LOG_VAR2( TUTTLE_INFO, _stream->codec->time_base.num, _stream->codec->time_base.den );
		
		_stream->codec->sample_rate        = 48000; ///< samples per second
		_stream->codec->channels           = 0;     ///< number of audio channels
	}
	return true;
}

bool LibAVVideoWriter::finishInit()
{
	int pixfmt_allowed = 0, k;
	if ( _videoCodec->pix_fmts )
	{
		for ( k = 0; _videoCodec->pix_fmts[k] != PIX_FMT_NONE; k++ )
		{
			if ( _videoCodec->pix_fmts[k] == _out_pixelFormat )
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
		TUTTLE_LOG_ERROR( "avWriter: pixel format " << av_get_pix_fmt_name(_out_pixelFormat) << " not available in codec" );
		_out_pixelFormat = _videoCodec->pix_fmts[0];
		TUTTLE_LOG_ERROR( "avWriter: auto-selecting " << av_get_pix_fmt_name(_out_pixelFormat) );
	}
	_stream->codec->pix_fmt     = _out_pixelFormat;
	
	if( !strcmp( _avFormatOptions->oformat->name, "mp4" ) || !strcmp( _avFormatOptions->oformat->name, "mov" ) || !strcmp( _avFormatOptions->oformat->name, "3gp" ) || !strcmp( _avFormatOptions->oformat->name, "flv" ) )
		_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	
	av_dump_format( _avFormatOptions, 0, getFilename().c_str(), 1 );
	
	if( avcodec_open2( _stream->codec, _videoCodec, NULL ) < 0 )
	{
		freeFormat();
		BOOST_THROW_EXCEPTION( exception::Format()
							   << exception::user( "avWriter: unable to open codec." ) );
		return false;
	}
	
	if( !( _ofmt->flags & AVFMT_NOFILE ) )
	{
		if( avio_open2( &_avFormatOptions->pb, getFilename().c_str(),
						AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
		{
			freeFormat();
			BOOST_THROW_EXCEPTION( exception::FileNotExist()
								   << exception::user( "avWriter: unable to open file." ) );
		}
	}
	
	int error = avformat_write_header( _avFormatOptions, NULL );
	if( error )
	{
		TUTTLE_LOG_ERROR( libavError_toString( error) );
		BOOST_THROW_EXCEPTION( exception::Format()
							   << exception::user( "avWriter: unable to write header." ) );
	}
	return true;
}

std::string LibAVVideoWriter::getErrorStr( const int errnum ) const
{
	static const std::size_t errbuf_size = 2048;
	char errbuf[errbuf_size];
	av_strerror( errnum, errbuf, errbuf_size );
	return std::string( errbuf );
}

int LibAVVideoWriter::execute( boost::uint8_t* const in_buffer, const int in_width, const int in_height, const PixelFormat in_pixelFormat )
{
	_statusCode = eWriterStatusIgnoreFinish;
	
	_statusCode = eWriterStatusCleanup;
	
	AVFrame* in_frame = avcodec_alloc_frame();
	avcodec_get_frame_defaults( in_frame );
	avpicture_fill( (AVPicture*)in_frame, in_buffer, in_pixelFormat, in_width, in_height );

	AVFrame* out_frame = avcodec_alloc_frame();
	avcodec_get_frame_defaults( out_frame );
	int out_picSize = avpicture_get_size( _out_pixelFormat, getWidth(), getHeight() );
	boost::uint8_t* out_buffer = (boost::uint8_t*) av_malloc( out_picSize );
	avpicture_fill( (AVPicture*) out_frame, out_buffer, _out_pixelFormat, getWidth(), getHeight() );

	_sws_context = sws_getCachedContext( _sws_context, in_width, in_height, in_pixelFormat, getWidth(), getHeight(), _out_pixelFormat, SWS_BICUBIC, NULL, NULL, NULL );

	TUTTLE_TLOG( TUTTLE_TRACE, "avWriter: input format: " << av_get_pix_fmt_name( in_pixelFormat ) << " -> output format: " << av_get_pix_fmt_name( _out_pixelFormat ) );

	if( !_sws_context )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user() + "libav-conversion failed (" + in_pixelFormat + "->" + _out_pixelFormat + ")." );
	}
	const int error = sws_scale( _sws_context, in_frame->data, in_frame->linesize, 0, getHeight(), out_frame->data, out_frame->linesize );
	if( error < 0 )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user() + "libav-conversion failed (" + in_pixelFormat + "->" + _out_pixelFormat + ")." );
	}

	int ret = 0;
	if( ( _avFormatOptions->oformat->flags & AVFMT_RAWPICTURE ) &&
		( _stream->codec->codec->id == AV_CODEC_ID_RAWVIDEO ) )
	{
		//TUTTLE_TLOG( TUTTLE_TRACE, "RAW : " << sizeof( AVPicture ) );
		AVPacket pkt;
		av_init_packet( &pkt );
		pkt.data         = (boost::uint8_t*) out_frame;
		pkt.size         = sizeof( AVPicture );
		pkt.pts          = av_rescale_q(out_frame->pts, _stream->codec->time_base, _stream->time_base);
		pkt.stream_index = _stream->index;
		pkt.flags       |= AV_PKT_FLAG_KEY;
		
		ret = av_interleaved_write_frame( _avFormatOptions, &pkt );
		if ( ret < 0 )
		{
			BOOST_THROW_EXCEPTION( exception::File()
				<< exception::user( "avWriter: error writing packet to file" ) );
		}
	}
	else
	{
		AVPacket pkt;
		_hasFrame = 0;
		av_init_packet( &pkt );
		pkt.size = 0;
		pkt.data = NULL;
		pkt.stream_index = _stream->index;

		if( ( _stream->codec->coded_frame ) &&
		    ( _stream->codec->coded_frame->pts != static_cast<boost::int64_t>( AV_NOPTS_VALUE ) ) )
		{
			pkt.pts = av_rescale_q( _stream->codec->coded_frame->pts, _stream->codec->time_base, _stream->time_base );
			TUTTLE_TLOG( TUTTLE_TRACE, "avWriter: Video Frame PTS: " << pkt.pts );
		}
		else
		{
			TUTTLE_TLOG( TUTTLE_TRACE, "avWriter: Video Frame PTS: not set" );
		}
		if( _stream->codec->coded_frame &&
		    _stream->codec->coded_frame->key_frame )
		{
			pkt.flags |= AV_PKT_FLAG_KEY;
		}
		
		out_frame->pts = pts;
		out_frame->quality = _stream->codec->global_quality;
		pts += _stream->codec->time_base.num;
		ret = avcodec_encode_video2( _stream->codec, &pkt, out_frame, &_hasFrame );
		if( ret < 0 )
		{
			TUTTLE_LOG_ERROR( "avWriter: error writing packet to file" );
			TUTTLE_LOG_ERROR( getErrorStr(ret) );
			return false;
		}

		
		if( _hasFrame )
		{
			ret = av_interleaved_write_frame( _avFormatOptions, &pkt );
			if ( ret < 0 )
			{
				BOOST_THROW_EXCEPTION( exception::File()
					<< exception::user( "avWriter: error writing packet to file" ) );
			}
		}
	}

	av_free( out_buffer );
	av_free( out_frame );
	av_free( in_frame );
	// in_buffer not free (function parameter)

	if( ret < 0 )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user( "avWriter: error writing frame to file." ) );
	}

	_statusCode = eWriterStatusSuccess;
	return true;
}

void LibAVVideoWriter::finish()
{
	if( _statusCode == eWriterStatusIgnoreFinish )
		return;
	
	int ret = 0;

	for( _hasFrame = 1; _hasFrame; )
	{
		TUTTLE_TLOG( TUTTLE_TRACE, "encode last frames ..." );
		AVPacket pkt;
		av_init_packet( &pkt );
		pkt.size = 0;
		pkt.data = NULL;
		pkt.stream_index = _stream->index;
		
		
		ret = avcodec_encode_video2( _stream->codec, &pkt, NULL, &_hasFrame );
		if( ret < 0 )
		{
			TUTTLE_LOG_ERROR( "avWriter: error writing packet to file" );
			TUTTLE_LOG_ERROR( getErrorStr(ret) );
			break;
		}
		
		if( _hasFrame )
		{
			TUTTLE_LOG_ERROR( "write packet" );
			ret = av_interleaved_write_frame( _avFormatOptions, &pkt );
			if ( ret < 0 )
			{
				BOOST_THROW_EXCEPTION( exception::File()
					<< exception::user( "avWriter: error writing packet to file" ) );
			}
			av_free_packet( &pkt );
		}
	}
	
	av_write_trailer( _avFormatOptions );
	avcodec_close( _stream->codec );
	if( !( _avFormatOptions->oformat->flags & AVFMT_NOFILE ) )
		avio_close( _avFormatOptions->pb );
	freeFormat();
	
}

void LibAVVideoWriter::freeFormat()
{
	avcodec_close( _stream->codec );
	for( int i = 0; i < static_cast<int>( _avFormatOptions->nb_streams ); ++i )
		av_freep( &_avFormatOptions->streams[i] );
	av_free( _avFormatOptions );
	_avFormatOptions = 0;
	_stream          = 0;
}

}
}
}
