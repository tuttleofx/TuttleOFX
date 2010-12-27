#ifndef IMAGEMAGICK_READER_PROCESS_HPP
#define IMAGEMAGICK_READER_PROCESS_HPP

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace reader {

/**
 *
 */
template<class View>
class ImageMagickReaderProcess : public ImageGilProcessor<View>
{
protected:
	ImageMagickReaderPlugin&    _plugin;        ///< Rendering plugin

public:
	ImageMagickReaderProcess( ImageMagickReaderPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "ImageMagickReaderProcess.tcc"

#endif
