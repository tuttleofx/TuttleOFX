#ifndef PNG_WRITER_PROCESS_HPP
#define PNG_WRITER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class PNGWriterProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	PNGWriterPlugin&    _plugin;        ///< Rendering plugin

public:
	PNGWriterProcess( PNGWriterPlugin & instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Bits>
	void writeImage( View& src, const std::string& filepath );
};

}
}
}
}

#include "PNGWriterProcess.tcc"

#endif
