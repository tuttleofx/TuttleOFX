#ifndef PNG_READER_PROCESS_HPP
#define PNG_READER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class PNGReaderProcess : public ImageGilProcessor<View>
{
protected:
	PNGReaderPlugin&    _plugin;        ///< Rendering plugin

public:
	PNGReaderProcess( PNGReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );

	View& readImage( View& dst, const std::string& filepath ) throw( PluginException );
};

}
}
}
}

#include "PNGReaderProcess.tcc"

#endif
