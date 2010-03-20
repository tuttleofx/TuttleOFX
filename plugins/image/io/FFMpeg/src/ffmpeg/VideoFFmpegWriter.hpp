#ifndef __VIDEOFFMPEGWRITER_HPP__
#define __VIDEOFFMPEGWRITER_HPP__

#include "FFmpeg.hpp"

#include <iostream>
#include <string>
#include <vector>


class VideoFFmpegWriter : public FFmpeg
{
private:

	enum WriterError
	{
		SUCCESS = 0, IGNORE_FINISH, CLEANUP
	};

public:
	explicit VideoFFmpegWriter();
	~VideoFFmpegWriter( );

	bool movie( ) const
	{
		return true;
	}

	int execute( uint8_t* in_buffer, int in_width, int height, PixelFormat in_fmt = PIX_FMT_RGB24 );
	void finish( );
//	void knobs( Knob_Callback f );

private:
	void freeFormat( );

public:

	void filename( std::string filename )
	{
		_filename = filename;
	}
	std::string filename( ) const
	{
		return _filename;
	}

	void width( int width )
	{
		_width = width;
	}
	int width( ) const
	{
		return _width;
	}

	void height( int height )
	{
		_height = height;
	}
	int height( ) const
	{
		return _height;
	}

	void aspectRatio( double aspectRatio )
	{
		_aspectRatio = aspectRatio;
	}
	double aspectRatio( ) const
	{
		return _aspectRatio;
	}

	void framerate( double framerate )
	{
		_framerate = framerate;
	}
	double framerate( ) const
	{
		return _framerate;
	}

public:
//private:
	AVCodecContext* _avctxOptions[CODEC_TYPE_NB];
	AVFormatContext* _avformatOptions;
	struct SwsContext * _sws_context; ///< contexte de transformation swscale
	AVStream* _stream;
	std::vector<std::string> _formatsLongNames;
	std::vector<std::string> _formatsShortNames;
	std::vector<std::string> _codecsLongNames;
	std::vector<std::string> _codecsShortNames;

	WriterError _error;
	std::string _filename;
	int _width;
	int _height;
	double _aspectRatio;
	double _framerate; // not used....
	PixelFormat _out_pixelFormat;
	// knobs variables
	float _fps;
	int _format;
	int _codec;
	int _bitrate;
	int _bitrateTolerance;
	int _gopSize;
	int _bFrames;
	int _mbDecision;
};


#endif //__CLIPVIDEOFFMPEGWRITER_HPP__

