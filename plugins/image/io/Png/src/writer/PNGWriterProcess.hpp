#ifndef PNG_WRITER_PROCESS_HPP
#define PNG_WRITER_PROCESS_HPP

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
namespace writer {

/**
 * @brief Base class for the denoising processor
 *
 */
template<class View>
class PNGWriterProcess : public ImageGilProcessor<View>
{
public:
	typedef typename View::value_type Pixel;

protected:
	OFX::StringParam*   _filepath;      ///< File path
	PNGWriterPlugin&    _plugin;        ///< Rendering plugin
	View _srcView;      ///< Input view
	View _dstView;      ///< Input view

public:
	PNGWriterProcess( PNGWriterPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindow );

	static void writeImage( View& src, std::string& filepath ) throw( tuttle::plugin::PluginException );
};

}
}
}
}

#include "PNGWriterProcess.tcc"

#endif
