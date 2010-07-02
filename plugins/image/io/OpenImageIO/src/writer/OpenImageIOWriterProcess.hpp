#ifndef OPENIMAGEIO_WRITER_PROCESS_HPP
#define OPENIMAGEIO_WRITER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class OpenImageIOWriterProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	OpenImageIOWriterPlugin&    _plugin;        ///< Rendering plugin

public:
	OpenImageIOWriterProcess( OpenImageIOWriterPlugin & instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Bits>
	void writeImage( View& src, const std::string& filepath );
};

}
}
}
}

#include "OpenImageIOWriterProcess.tcc"

#endif
