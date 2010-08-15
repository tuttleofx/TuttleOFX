#ifndef IMAGEMAGICK_READER_PROCESS_HPP
#define IMAGEMAGICK_READER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
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
