#include "VideoFFmpegWriter.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/foreach.hpp>

#include <cstdio>


namespace tuttle {
namespace plugin {
namespace ffmpeg {

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT( 52, 40, 0 )
// compatibility with previous versions of libavformat
#define av_guess_format guess_format
#endif

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

static boost::int64_t pts = 0;

VideoFFmpegWriter::VideoFFmpegWriter()
	: FFmpeg()
	, FFmpegPreset()
	, _avFormatOptions   ( NULL )
	, _avVideoOptions    ( NULL )
	, _avAudioOptions    ( NULL )
	, _sws_context       ( NULL )
	, _stream            ( NULL )
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
	, _videoPresetName   ( "" )
{
	av_log_set_level( AV_LOG_WARNING );
	av_register_all();

	AVOutputFormat* fmt = av_oformat_next( NULL );
	
	while( fmt )
	{
		// add only format with video track
		if( fmt->video_codec != CODEC_ID_NONE )
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

int VideoFFmpegWriter::start( )
{
	if( !_avFormatOptions )
	{
		// TODO avformat_alloc_context2 can guess format from filename
		// if format name is NULL, find a way to expose the feature
		if( avformat_alloc_output_context2(&_avFormatOptions, NULL, _formatName.c_str(), getFilename().c_str()) < 0 )
		{
			TUTTLE_CERR( "ffmpegWriter: output context allocation failed" );
			return false;
		}
		_ofmt = _avFormatOptions->oformat;
		TUTTLE_CERR( "ffmpegWriter: " << std::string(_ofmt->name) << " format selected" );
	}

	if( !_stream )
	{
		_videoCodec = avcodec_find_encoder_by_name( _videoCodecName.c_str() );
		if (!_videoCodec)
		{
			BOOST_THROW_EXCEPTION( exception::Format()
				<< exception::user( "ffmpegWriter: codec not found." ) );
		}
		TUTTLE_CERR( "ffmpegWriter: " << std::string(_videoCodec->name) << " codec selected" );

		_stream = avformat_new_stream( _avFormatOptions, _videoCodec );
		if( !_stream )
		{
			BOOST_THROW_EXCEPTION( exception::File()
				<< exception::user( "ffmpegWriter: out of memory." ) );
		}
		avcodec_get_context_defaults3(_stream->codec, _videoCodec);

//		TUTTLE_COUT_VAR2( _videoCodecName, _videoPresetName );
		
		_stream->codec->width              = getWidth();
		_stream->codec->height             = getHeight();
		_stream->codec->time_base          = av_d2q( 1.0 / _fps, 100 );
		//_stream->codec->gop_size           = _gopSize;
		_stream->codec->sample_rate        = 48000; ///< samples per second
		_stream->codec->channels           = 0;     ///< number of audio channels
		/*if( _bFrames )
		{
			_stream->codec->max_b_frames     = _bFrames;
			_stream->codec->b_frame_strategy = 0;
			_stream->codec->b_quant_factor   = 2.0;
		}*/

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
			TUTTLE_CERR( "ffmpegWriter: pixel format " << av_get_pix_fmt_name(_out_pixelFormat) << " not available in codec" );
			_out_pixelFormat = _videoCodec->pix_fmts[0];
			TUTTLE_CERR( "ffmpegWriter: auto-selecting " << av_get_pix_fmt_name(_out_pixelFormat) );
		}
		_stream->codec->pix_fmt     = _out_pixelFormat;

		if( !strcmp( _avFormatOptions->oformat->name, "mp4" ) || !strcmp( _avFormatOptions->oformat->name, "mov" ) || !strcmp( _avFormatOptions->oformat->name, "3gp" ) || !strcmp( _avFormatOptions->oformat->name, "flv" ) )
			_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		av_dump_format( _avFormatOptions, 0, getFilename().c_str(), 1 );
		
		if( avcodec_open2( _stream->codec, _videoCodec, NULL ) < 0 )
		{
			freeFormat();
			BOOST_THROW_EXCEPTION( exception::Format()
				<< exception::user( "ffmpegWriter: unable to open codec." ) );
			return false;
		}

		if( !( _ofmt->flags & AVFMT_NOFILE ) )
		{
			if( avio_open2( &_avFormatOptions->pb, getFilename().c_str(),
							AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
			{
				freeFormat();
				BOOST_THROW_EXCEPTION( exception::FileNotExist()
					<< exception::user( "ffmpegWriter: unable to open file." ) );
			}
		}

		int error = avformat_write_header( _avFormatOptions, NULL );
		if( error )
		{
			TUTTLE_CERR( ffmpegError_toString( error) );
			BOOST_THROW_EXCEPTION( exception::Format()
				<< exception::user( "ffmpegWriter: unable to write header." ) );
		}
	}
	return true;
}

int VideoFFmpegWriter::execute( boost::uint8_t* const in_buffer, const int in_width, const int in_height, const PixelFormat in_pixelFormat )
{
	_statusCode = eWriterStatusIgnoreFinish;
	
	//start();
	
/*
	if( !_avFormatOptions )
	{
		// TODO avformat_alloc_context2 can guess format from filename
		// if format name is NULL, find a way to expose the feature
		if( avformat_alloc_output_context2(&_avFormatOptions, NULL, _formatName.c_str(), getFilename().c_str()) < 0 )
		{
			TUTTLE_CERR( "ffmpegWriter: output context allocation failed" );
			return false;
		}
		_ofmt = _avFormatOptions->oformat;
		TUTTLE_CERR( "ffmpegWriter: " << std::string(_ofmt->name) << " format selected" );
	}

	if( !_stream )
	{
		_videoCodec = avcodec_find_encoder_by_name( _videoCodecName.c_str() );
		if (!_videoCodec)
		{
			BOOST_THROW_EXCEPTION( exception::Format()
				<< exception::user( "ffmpegWriter: codec not found." ) );
		}
		TUTTLE_CERR( "ffmpegWriter: " << std::string(_videoCodec->name) << " codec selected" );

		_stream = avformat_new_stream( _avFormatOptions, _videoCodec );
		if( !_stream )
		{
			BOOST_THROW_EXCEPTION( exception::File()
				<< exception::user( "ffmpegWriter: out of memory." ) );
		}
		avcodec_get_context_defaults3(_stream->codec, _videoCodec);

		TUTTLE_COUT_VAR2( _videoCodecName, _videoPresetName );
		
//		if( _videoPresetName.length() !=0 )
//		{
//			TUTTLE_COUT( "ffmpegWriter: " << _videoPresetName << " preset selected" );
//			const std::string presetFilename = _preset.getFilename( std::string(_videoCodec->name), _videoPresetName );
//			
//			FFmpegPreset::PresetsOptions opts = _preset.getOptionsForPresetFilename( presetFilename );
//			BOOST_FOREACH( FFmpegPreset::PresetsOptions::value_type& itOpt, opts )
//			{
//				int ret = av_opt_set( (void*)_stream->codec, itOpt.first.c_str(), itOpt.second.c_str(), 0);
//				switch( ret )
//				{
//					case AVERROR_OPTION_NOT_FOUND: TUTTLE_CERR( "ffmpegPreset: unable to find " << itOpt.first ); break;
//					case AVERROR(EINVAL): TUTTLE_CERR( "ffmpegPreset: invalid value " << itOpt.second.c_str() << " for option " << itOpt.first ); break;
//					case AVERROR(ERANGE): TUTTLE_CERR( "ffmpegPreset: invalid range for parameter " << itOpt.first << " : " << itOpt.second.c_str() ); break;
//				}
//			}
//		}
		
		_stream->codec->width              = getWidth();
		_stream->codec->height             = getHeight();
		_stream->codec->time_base          = av_d2q( 1.0 / _fps, 100 );
		//_stream->codec->gop_size           = _gopSize;
		_stream->codec->sample_rate        = 48000; ///< samples per second
		_stream->codec->channels           = 0;     ///< number of audio channels
//		if( _bFrames )
//		{
//			_stream->codec->max_b_frames     = _bFrames;
//			_stream->codec->b_frame_strategy = 0;
//			_stream->codec->b_quant_factor   = 2.0;
//		}

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
			TUTTLE_CERR( "ffmpegWriter: pixel format " << av_get_pix_fmt_name(_out_pixelFormat) << " not available in codec" );
			_out_pixelFormat = _videoCodec->pix_fmts[0];
			TUTTLE_CERR( "ffmpegWriter: auto-selecting " << av_get_pix_fmt_name(_out_pixelFormat) );
		}
		_stream->codec->pix_fmt     = _out_pixelFormat;

		if( !strcmp( _avFormatOptions->oformat->name, "mp4" ) || !strcmp( _avFormatOptions->oformat->name, "mov" ) || !strcmp( _avFormatOptions->oformat->name, "3gp" ) || !strcmp( _avFormatOptions->oformat->name, "flv" ) )
			_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		av_dump_format( _avFormatOptions, 0, getFilename().c_str(), 1 );

		if( avcodec_open2( _stream->codec, _videoCodec, NULL ) < 0 )
		{
			freeFormat();
			BOOST_THROW_EXCEPTION( exception::Format()
				<< exception::user( "ffmpegWriter: unable to open codec." ) );
			return false;
		}

		if( !( _ofmt->flags & AVFMT_NOFILE ) )
		{
			if( avio_open2( &_avFormatOptions->pb, getFilename().c_str(),
							AVIO_FLAG_WRITE, NULL, NULL ) < 0 )
			{
				freeFormat();
				BOOST_THROW_EXCEPTION( exception::FileNotExist()
					<< exception::user( "ffmpegWriter: unable to open file." ) );
			}
		}

		avformat_write_header( _avFormatOptions, NULL );
	}*/
	
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

	TUTTLE_COUT( "ffmpegWriter: input format: " << av_get_pix_fmt_name( in_pixelFormat ) );
	TUTTLE_COUT( "ffmpegWriter: output format: " << av_get_pix_fmt_name( _out_pixelFormat ) );

	if( !_sws_context )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user() + "ffmpeg-conversion failed (" + in_pixelFormat + "->" + _out_pixelFormat + ")." );
	}
	const int error = sws_scale( _sws_context, in_frame->data, in_frame->linesize, 0, getHeight(), out_frame->data, out_frame->linesize );
	if( error < 0 )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user() + "ffmpeg-conversion failed (" + in_pixelFormat + "->" + _out_pixelFormat + ")." );
	}

	int ret = 0;
	if( ( _avFormatOptions->oformat->flags & AVFMT_RAWPICTURE ) != 0 )
	{
		AVPacket pkt;
		av_init_packet( &pkt );
		pkt.flags       |= AV_PKT_FLAG_KEY;
		pkt.stream_index = _stream->index;
		pkt.data         = (boost::uint8_t*) out_frame;
		pkt.size         = sizeof( AVPicture );
		ret              = av_interleaved_write_frame( _avFormatOptions, &pkt );
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
			ret = av_interleaved_write_frame( _avFormatOptions, &pkt );
			if ( ret < 0 )
			{
				BOOST_THROW_EXCEPTION( exception::File()
					<< exception::user( "ffmpegWriter: error writing packet to file" ) );
			}
		}
	}

	av_free( out_buffer );
	av_free( out_frame );
	av_free( in_frame );
	// in_buffer not free (function parameter)

	if( ret )
	{
		BOOST_THROW_EXCEPTION( exception::File()
			<< exception::user( "ffmpegWriter: error writing frame to file." ) );
	}

	_statusCode = eWriterStatusSuccess;
	return true;
}

void VideoFFmpegWriter::finish()
{
	if( _statusCode == eWriterStatusIgnoreFinish )
		return;
	av_write_trailer( _avFormatOptions );
	avcodec_close( _stream->codec );
	if( !( _avFormatOptions->oformat->flags & AVFMT_NOFILE ) )
		avio_close( _avFormatOptions->pb );
	freeFormat();
}

void VideoFFmpegWriter::freeFormat()
{
	avcodec_close( _stream->codec );
	for( int i = 0; i < static_cast<int>( _avFormatOptions->nb_streams ); ++i )
		av_freep( &_avFormatOptions->streams[i] );
	av_free( _avFormatOptions );
	_avFormatOptions = 0;
	_stream          = 0;
}

void VideoFFmpegWriter::setVideoPreset( const int id )
{
	if( id == -1 )
	{
		_videoPresetName = "";
		return;
	}
	
	FFmpegPreset::Presets p = getPresets().getConfigList( _videoCodecName );
	_videoPresetName = p[id];
}

void VideoFFmpegWriter::setAudioPreset( const int id )
{
	if( id == -1 )
	{
		_audioPresetName = "";
		return;
	}
	
	FFmpegPreset::Presets p = getPresets().getConfigList( _audioCodecName );
	_audioPresetName = p[id];
}

void VideoFFmpegWriter::optionSet( const EAVParamType& type, AVOption& opt, bool& value )
{
	int error;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			break;
		}
		case eAVParamVideo:
		{
			error = ( (void*)_stream->codec->av_class, opt.name, value, 0 );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_CERR( "FFMpeg writer: " << ffmpegError_toString( error ) << " : " << opt.name << " ( " << ( value ? "True" : "False" ) << " )" );
}

void VideoFFmpegWriter::optionSet( const EAVParamType& type, AVOption& opt, int& value )
{
	int error;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			break;
		}
		case eAVParamVideo:
		{
			//error = av_opt_set_int( (void*)_stream->codec->av_class, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			error = av_opt_set_int( _avFormatOptions, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_CERR( "FFMpeg writer: " << ffmpegError_toString( error ) << " : " << opt.name << " ( " << value << " )" );
}


void VideoFFmpegWriter::optionSet( const EAVParamType& type, AVOption &opt, double &value )
{
	int error;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_double( _avFormatOptions, opt.name, value, 0 );
			break;
		}
		case eAVParamVideo:
		{
			//error = av_opt_set_double( (void*)_stream->codec->av_class, opt.name, value, 0 );
			error = av_opt_set_double( _avFormatOptions, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_double( _avFormatOptions, opt.name, value, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_CERR( "FFMpeg writer: " << ffmpegError_toString( error ) << " : " << opt.name << " ( " << value << " )" );
}

void VideoFFmpegWriter::optionSet( const EAVParamType& type, AVOption &opt, int &valueNum, int& valueDen )
{
	int error;
	AVRational q;
	q.num = valueNum;
	q.den = valueDen;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_q( _avFormatOptions, opt.name, q, 0 );
			break;
		}
		case eAVParamVideo:
		{
			//error = av_opt_set_q( (void*)_stream->codec->av_class, opt.name, q, 0 );
			error = av_opt_set_q( _avFormatOptions, opt.name, q, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_q( _avFormatOptions, opt.name, q, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_CERR( "FFMpeg writer: " << ffmpegError_toString( error ) << " : " << opt.name << " ( " << valueNum << "/" << valueDen<< " )" );
}

void VideoFFmpegWriter::optionSet( const EAVParamType& type, AVOption &opt, std::string &value )
{
	int error;
	if( ! value.length() )
		return;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set( _avFormatOptions, opt.name, value.c_str(), 0 );
			break;
		}
		case eAVParamVideo:
		{
			//error = av_opt_set( (void*)_stream->codec->av_class, opt.name, value.c_str(), 0 );
			error = av_opt_set( _avFormatOptions, opt.name, value.c_str(), AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set( _avFormatOptions, opt.name, value.c_str(), 0 );
			break;
		}
	}
	if( error )
		TUTTLE_CERR( "FFMpeg writer: " << ffmpegError_toString( error ) << " : " << opt.name << " ( " << value << " )" );
}

void VideoFFmpegWriter::optionSetImageSize( const EAVParamType &type, AVOption &opt, int &width, int& height)
{
	/*
	switch( type )
	{
		case eAVParamFormat:
		{
			av_opt_set_image_size( _avFormatOptions, opt.name, width, height, 0 );
			break;
		}
		case eAVParamVideo:
		{
			av_opt_set_image_size( _avFormatOptions, opt.name, width, height, 0 );
			break;
		}
		case eAVParamAudio:
		{
			av_opt_set_image_size( _avFormatOptions, opt.name, width, height, 0 );
			break;
		}
	}*/
}

}
}
}
