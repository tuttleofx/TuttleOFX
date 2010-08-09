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
 *
 */
template<class View>
class PngReaderProcess : public ImageGilProcessor<View>
{
protected:
	PngReaderPlugin&    _plugin;        ///< Rendering plugin

public:
	PngReaderProcess( PngReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "PngReaderProcess.tcc"

#endif
