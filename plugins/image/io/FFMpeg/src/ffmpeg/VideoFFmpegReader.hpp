#ifndef _VIDEOFFMPEGREADER_HPP_
#define _VIDEOFFMPEGREADER_HPP_

#include "FFmpeg.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <vector>

enum EIntrelacment { eInterlacmentNone, eInterlacmentUpper, eInterlacmentLower };

class VideoFFmpegReader : public FFmpeg
{
public:
	explicit VideoFFmpegReader( );
	~VideoFFmpegReader( );

	bool videosequence( ) const
	{
		return true;
	}

	bool open( const std::string &filename );
	void close( );
	bool read( const int frame );

private:
	bool setupStreamInfo( );

	bool hasVideo( ) const
	{
		return !_videoIdx.empty( );
	}

	AVStream* getVideoStream( )
	{
		return _context && _currVideoIdx >= 0 ? _context->streams[_currVideoIdx] : NULL;
	}

	void openVideoCodec( );
	void closeVideoCodec( );
	int64_t getTimeStamp( int pos ) const;
	int getFrame( int64_t timestamp ) const;

	/**
	 * @brief Seek to the nearest previous keyframe from pos.
	 * Write in _data the image result.
	 * @param pos frame number to seek
	 */
	bool seek( size_t pos );
	/**
	 * @brief Decode the current frame
	 * @param the number of the current frame
	 */
	bool decodeImage( const int frame );

public:

	int width( ) const
	{
		return _width;
	}

	int height( ) const
	{
		return _height;
	}

	double aspectRatio( ) const
	{
		return _aspect;
	}

	EIntrelacment interlacment()
	{
		return _interlacment;
	}

	double fps( ) const
	{
		if( _fpsDen )
			return _fpsNum / (double) _fpsDen;
		return 1.0f;
	}

	uint64_t nbFrames( )
	{
		return _nbFrames;
	}

	int frame( )
	{
		return _lastDecodedPos;
	}

	bool isOpen( ) const
	{
		return _isOpen;
	}

	unsigned char* data( )
	{
		return &_data[0];
	}

	std::string codecName( )
	{
		if( !_videoCodec )
			return "";
		return(_videoCodec->long_name ) +std::string( " (" ) + std::string( _videoCodec->name ) + std::string( ", " ) + boost::lexical_cast<std::string>( static_cast<int>( _videoCodec->id ) ) + std::string( ")" );
	}

	std::string formatName( )
	{
		if( !_format )
			return "";
		return(_format->long_name ) +std::string( " (" ) + std::string( _format->name ) + std::string( ")" );
	}

	std::string codecIDString( )
	{
		if( !_videoCodec )
			return "";
		return codecID_toString( _videoCodec->id );
	}

	std::string codecTypeString( )
	{
		if( !_videoCodec )
			return "";
		return codecType_toString( _videoCodec->type );
	}

	std::string pixelFormatString( )
	{
		if( !_videoCodec || !_avctxOptions ||_videoCodec->type <=0 || !_avctxOptions[_videoCodec->type] )
			return "";
		return pixelFormat_toString( _avctxOptions[_videoCodec->type]->pix_fmt );
	}

public: // private:
	AVFormatContext* _context;
	AVInputFormat* _format;
	AVFormatParameters* _params;
	AVFrame* _avFrame;
	AVCodec* _videoCodec;
	AVPacket _pkt;
	AVCodecContext* _avctxOptions[CODEC_TYPE_NB];
	AVFormatContext* _avformatOptions;
	struct SwsContext * _sws_context; ///< contexte de transformation swscale
	std::vector<int> _videoIdx;
	int _fpsNum;
	int _fpsDen;
	int _currVideoIdx;
	uint64_t _nbFrames;
	int _width;
	int _height;
	double _aspect;
	std::vector<unsigned char> _data;
	bool _offsetTime;
	int _lastSearchPos;
	int _lastDecodedPos;
	bool _isOpen;
	EIntrelacment _interlacment;
};

#endif
