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
	JpegWriterProcessParams _params;

public:
	JpegWriterProcess( JpegWriterPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Bits>
	void writeImage( View& src );
};

}
}
}
}

#include "JpegWriterProcess.tcc"

#endif
