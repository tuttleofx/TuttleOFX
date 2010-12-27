#ifndef IMAGEMAGICK_WRITER_PROCESS_HPP
#define IMAGEMAGICK_WRITER_PROCESS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace writer {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class ImageMagickWriterProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	ImageMagickWriterPlugin&    _plugin;        ///< Rendering plugin

public:
	ImageMagickWriterProcess( ImageMagickWriterPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Bits>
	void writeImage( View& src, const std::string& filepath );
};

}
}
}
}

#include "ImageMagickWriterProcess.tcc"

#endif
