#ifndef __VIDEOFFMPEGWRITER_HPP__
#define __VIDEOFFMPEGWRITER_HPP__

#include "FFmpeg.hpp"
#include "VideoFFmpegReader.hpp"

#include <tuttle/plugin/global.hpp>

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
	~VideoFFmpegWriter();

	bool movie() const
	{
		return true;
	}

	int  execute( uint8_t* in_buffer, int in_width, int height, PixelFormat in_fmt = PIX_FMT_RGB24 );
	void finish();

private:
	void freeFormat();

public:
	void filename( std::string filename )
	{
		_filename = filename;
	}

	std::string filename() const
	{
		return _filename;
	}

	void width( const int width )
	{
		_width = width;
	}

	int width() const
	{
		return _width;
	}

	void height( const int height )
	{
		_height = height;
	}

	int height() const
	{
		return _height;
	}

	void aspectRatio( double aspectRatio )
	{
		_aspectRatio = aspectRatio;
	}

	double aspectRatio() const
	{
		return _aspectRatio;
	}

	void fps( const double fps )
	{
		_fps = fps;
	}

	double fps() const
	{
		return _fps;
	}

	const std::string& getFormat() const
	{
		return _format;
	}

	const std::vector<std::string>& getFormatsShort() const
	{
		return _formatsShortNames;
	}

	const std::vector<std::string>& getFormatsLong() const
	{
		return _formatsLongNames;
	}

	void setBitRate( const int bitRate )
	{
		_bitRate = bitRate;
	}

	void setFormat( const unsigned int id )
	{
		_format = _formatsShortNames[id];
	}

	void setFormat( const std::string& format )
	{
		_format = format;
	}

	const std::string& getCodec() const
	{
		return _codec;
	}

	const std::vector<std::string>& getCodecsShort() const
	{
		return _codecsShortNames;
	}

	const std::vector<std::string>& getCodecsLong() const
	{
		return _codecsLongNames;
	}

	void setCodec( const unsigned int id )
	{
		_codec = _codecsShortNames[id];
	}

	void setCodec( const std::string& codec )
	{
		_codec = codec;
	}

	void configureFromRead( const VideoFFmpegReader& reader )
	{
		width( reader.width() );
		height( reader.height() );
		aspectRatio( reader.aspectRatio() );
		fps( reader.fps() );
		setBitRate( reader.bitRate() );
		setFormat( reader.formatName() );
		setCodec( reader.codecName() );
	}

private:
	AVCodecContext* _avctxOptions[CODEC_TYPE_NB];
	AVFormatContext* _avformatOptions;
	struct SwsContext* _sws_context;  ///< contexte de transformation swscale
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
	PixelFormat _out_pixelFormat;
	// knobs variables
	float _fps;
	std::string _format;
	std::string _codec;
	int _bitRate;
	int _bitRateTolerance;
	int _gopSize;
	int _bFrames;
	int _mbDecision;
};

#endif

