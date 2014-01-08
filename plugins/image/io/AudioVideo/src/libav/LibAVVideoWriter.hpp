#ifndef _TUTTLE_PLUGIN_LIBAV_LIBAVVIDEOWRITER_HPP_
#define _TUTTLE_PLUGIN_LIBAV_LIBAVVIDEOWRITER_HPP_

#include "LibAV.hpp"
#include "LibAVVideoReader.hpp"

#include <tuttle/plugin/global.hpp>

#include <boost/cstdint.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {

class LibAVVideoWriter : public LibAV
{
private:
	enum EWriterStatus
	{
		eWriterStatusSuccess = 0,
		eWriterStatusIgnoreFinish,
		eWriterStatusCleanup
	};

public:
	explicit LibAVVideoWriter();

	bool movie() const
	{
		return true;
	}

	int  start( const std::map<std::string, std::string>& metas );
	bool finishInit();
	int  execute( boost::uint8_t* const in_buffer, const int in_width, const int height, const PixelFormat in_fmt = PIX_FMT_RGB24 );
	void finish();

private:
	void freeFormat();
	std::string getErrorStr( const int errnum ) const;

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

	void setPixelFormat( const PixelFormat pxlFmt )
	{
		_out_pixelFormat = pxlFmt;
	}

	PixelFormat getPixelFormat() const
	{
		return _out_pixelFormat;
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

	void setFormat( const unsigned int id )
	{
		_formatName = _formatsShortNames[id];
	}

	void setFormat( const std::string& format )
	{
		_formatName = format;
	}

	const std::string& getVideoCodec() const
	{
		return _videoCodecName;
	}
	
	const std::vector<std::string>& getVideoCodecsShort() const
	{
		return _videoCodecsShortNames;
	}

	const std::vector<std::string>& getVideoCodecsLong() const
	{
		return _videoCodecsLongNames;
	}
	
	const std::string& getAudioCodec() const
	{
		return _audioCodecName;
	}

	const std::vector<std::string>& getAudioCodecsShort() const
	{
		return _audioCodecsShortNames;
	}

	const std::vector<std::string>& getAudioCodecsLong() const
	{
		return _audioCodecsLongNames;
	}
	
	const std::vector<AVPrivOption>& getFormatPrivOpts() const
	{
		return _formatPrivOpts;
	}

	const std::vector<AVPrivOption>& getVideoCodecPrivOpts() const
	{
		return _videoCodecPrivOpts;
	}
	
	const std::vector<AVPrivOption>& getAudioCodecPrivOpts() const
	{
		return _audioCodecPrivOpts;
	}
	
	void setVideoCodec( const unsigned int id )
	{
		_videoCodecName = _videoCodecsShortNames[id];
	}

	void setVideoCodec( const std::string& codec )
	{
		_videoCodecName = codec;
	}
	
	void setAudioCodec( const unsigned int id )
	{
		_audioCodecName = _audioCodecsShortNames[id];
	}

	void setAudioCodec( const std::string& codec )
	{
		_audioCodecName = codec;
	}
	
	void configureFromRead( const LibAVVideoReader& reader )
	{
		setWidth       ( reader.width() );
		setHeight      ( reader.height() );
		setAspectRatio ( reader.aspectRatio() );
		setFps         ( reader.fps() );
		setFormat      ( reader.formatName() );
		setVideoCodec  ( reader.codecName() );
		//setAudioCodec  ( reader.codecName() );
	}

public:
	AVFormatContext*               _avFormatOptions;
	AVStream*                      _stream;
	
private:

	AVCodecContext*                _avVideoOptions;
	AVCodecContext*                _avAudioOptions;
	struct SwsContext*             _sws_context; ///< swscale: transformation context

	AVCodec*                       _videoCodec;
	AVCodec*                       _audioCodec;
	AVOutputFormat*                _ofmt;
	
	std::vector<AVPrivOption>      _formatPrivOpts;
	std::vector<AVPrivOption>      _videoCodecPrivOpts;
	std::vector<AVPrivOption>      _audioCodecPrivOpts;
	
	std::vector<std::string>       _formatsLongNames;
	std::vector<std::string>       _formatsShortNames;
	
	std::vector<std::string>       _videoCodecsLongNames;
	std::vector<std::string>       _videoCodecsShortNames;

	std::vector<std::string>       _audioCodecsLongNames;
	std::vector<std::string>       _audioCodecsShortNames;
	
	EWriterStatus                  _statusCode;
	int                            _hasFrame;
	
	std::string                    _filename;
	int                            _width;
	int                            _height;
	double                         _aspectRatio;
	PixelFormat                    _out_pixelFormat;
	
	double                          _fps;
	std::string                    _formatName;
	std::string                    _videoCodecName;
	std::string                    _audioCodecName;
};

}
}
}

#endif

