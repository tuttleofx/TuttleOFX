#ifndef _TUTTLE_PLUGIN_IMAGEMAGICK_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_IMAGEMAGICK_READER_PROCESS_HPP_

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
	ImageMagickReaderProcessParams _params;

public:
	ImageMagickReaderProcess( ImageMagickReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "ImageMagickReaderProcess.tcc"

#endif
