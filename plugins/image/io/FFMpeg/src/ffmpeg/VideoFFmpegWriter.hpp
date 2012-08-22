#ifndef __VIDEOFFMPEGWRITER_HPP__
#define __VIDEOFFMPEGWRITER_HPP__

#include "FFmpeg.hpp"
#include "FFmpegPreset.hpp"
#include "VideoFFmpegReader.hpp"

#include <tuttle/plugin/global.hpp>

#include <iostream>
#include <string>
#include <vector>

typedef struct AVPrivOption
{
	AVOption    o;
	std::string class_name;
} AVPrivOption;

class VideoFFmpegWriter : public FFmpeg, public FFmpegPreset
{
private:
	enum WriterError
	{
		SUCCESS = 0, IGNORE_FINISH, CLEANUP
	};

public:
	explicit VideoFFmpegWriter();

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
		return _formatName;
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
		_formatName = _formatsShortNames[id];
	}

	void setFormat( const std::string& format )
	{
		_formatName = format;
	}

	const std::string& getCodec() const
	{
		return _codecName;
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
		_codecName = _codecsShortNames[id];
	}

	void setCodec( const std::string& codec )
	{
		_codecName = codec;
	}
	
	void setVideoPreset( const unsigned int id );
	
	void setVideoPreset( const std::string& preset )
	{
		_videoPresetName = preset;
	}

	void configureFromRead( const VideoFFmpegReader& reader )
	{
		width       ( reader.width() );
		height      ( reader.height() );
		aspectRatio ( reader.aspectRatio() );
		fps         ( reader.fps() );
		setBitRate  ( reader.bitRate() );
		setFormat   ( reader.formatName() );
		setCodec    ( reader.codecName() );
	}

	const std::vector<AVPrivOption>& getFormatPrivOpts() const
	{
		return _formatPrivOpts;
	}
	
	const std::vector<AVPrivOption>& getCodecPrivOpts() const
	{
		return _codecPrivOpts;
	}

private:
	AVFormatContext*               _avformatOptions;
	struct SwsContext*             _sws_context;         ///< contexte de transformation swscale
	AVStream*                      _stream;
	AVCodec*                       _codec;
	AVOutputFormat*                _ofmt;
	std::vector<std::string>       _formatsLongNames;
	std::vector<std::string>       _formatsShortNames;
	std::vector<std::string>       _codecsLongNames;
	std::vector<std::string>       _codecsShortNames;

	std::vector<AVPrivOption>      _formatPrivOpts;
	std::vector<AVPrivOption>      _codecPrivOpts;

	WriterError                    _error;
	std::string                    _filename;
	int                            _width;
	int                            _height;
	double                         _aspectRatio;
	PixelFormat                    _out_pixelFormat;
	// knobs variables
	float                          _fps;
	std::string                    _formatName;
	std::string                    _codecName;
	std::string                    _videoPresetName;
	int                            _bitRate;
	int                            _bitRateTolerance;
	int                            _gopSize;
	int                            _bFrames;
	int                            _mbDecision;
};

#endif

