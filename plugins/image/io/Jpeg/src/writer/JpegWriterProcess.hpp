#ifndef JPEG_WRITER_PROCESS_HPP
#define JPEG_WRITER_PROCESS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class JpegWriterProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	JpegWriterPlugin&    _plugin;        ///< Rendering plugin

public:
	JpegWriterProcess( JpegWriterPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Bits>
	void writeImage( View& src, const std::string& filepath );
};

}
}
}
}

#include "JpegWriterProcess.tcc"

#endif
