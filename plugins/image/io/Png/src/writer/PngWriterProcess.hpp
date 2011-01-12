#ifndef PNG_WRITER_PROCESS_HPP
#define PNG_WRITER_PROCESS_HPP

#include <tuttle/plugin/global.hpp>
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
class PngWriterProcess : public ImageGilFilterProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	PngWriterPlugin&    _plugin;        ///< Rendering plugin

	PngWriterProcessParams _params;
	
public:
	PngWriterProcess( PngWriterPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	template<class Bits>
	void writeImage( View& src );
};

}
}
}
}

#include "PngWriterProcess.tcc"

#endif
