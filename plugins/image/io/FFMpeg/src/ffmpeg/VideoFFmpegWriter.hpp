#ifndef _TUTTLE_PLUGIN_FFMPEG_VIDEOFFMPEGWRITER_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_VIDEOFFMPEGWRITER_HPP_

#include "FFmpeg.hpp"
#include "FFmpegPreset.hpp"
#include "VideoFFmpegReader.hpp"

#include <tuttle/plugin/global.hpp>

#include <boost/cstdint.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace ffmpeg {

class VideoFFmpegWriter : public FFmpeg
{
private:
	enum EWriterStatus
	{
		eWriterStatusSuccess = 0,
		eWriterStatusIgnoreFinish,
		eWriterStatusCleanup
	};

public:
	explicit VideoFFmpegWriter();

	bool movie() const
	{
		return true;
	}

	int  execute( boost::uint8_t* const in_buffer, const int in_width, const int height, const PixelFormat in_fmt = PIX_FMT_RGB24 );
	void finish();

private:
	void freeFormat();

public:
	void setFilename( const std::string& filename )
	{
		_filename = filename;
	}

	std::string getFilename() const
	{
		return _filename;
	}

	void setWidth( const int width )
	{
		_width = width;
	}

	int getWidth() const
	{
		return _width;
	}

	void setHeight( const int height )
	{
		_height = height;
	}

	int getHeight() const
	{
		return _height;
	}

	void setAspectRatio( const double aspectRatio )
	{
		_aspectRatio = aspectRatio;
	}

	double getAspectRatio() const
	{
		return _aspectRatio;
	}

	void setFps( const double fps )
	{
		_fps = fps;
	}

	double getFps() const
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
	
	void setVideoPreset( const int id );
	
	void setVideoPreset( const std::string& preset )
	{
		_videoPresetName = preset;
	}

	void configureFromRead( const VideoFFmpegReader& reader )
	{
		setWidth       ( reader.width() );
		setHeight      ( reader.height() );
		setAspectRatio ( reader.aspectRatio() );
		setFps         ( reader.fps() );
		setBitRate  ( reader.bitRate() );
		setFormat   ( reader.formatName() );
		setCodec    ( reader.codecName() );
	}

	FFmpegPreset& getPresets() { return _preset; }
	const FFmpegPreset& getPresets() const { return _preset; }

private:
	AVFormatContext*               _avformatOptions;
	struct SwsContext*             _sws_context; ///< swscale: transformation context
	AVStream*                      _stream;
	AVCodec*                       _codec;
	AVOutputFormat*                _ofmt;
	std::vector<std::string>       _formatsLongNames;
	std::vector<std::string>       _formatsShortNames;
	std::vector<std::string>       _codecsLongNames;
	std::vector<std::string>       _codecsShortNames;

	FFmpegPreset                   _preset;
	
	EWriterStatus                    _statusCode;
	std::string                    _filename;
	int                            _width;
	int                            _height;
	double                         _aspectRatio;
	PixelFormat                    _out_pixelFormat;
	
	double                          _fps;
	std::string                    _formatName;
	std::string                    _codecName;
	std::string                    _videoPresetName;
	int                            _bitRate;
	int                            _bitRateTolerance;
	int                            _gopSize;
	int                            _bFrames;
	int                            _mbDecision;
};

}
}
}

#endif

