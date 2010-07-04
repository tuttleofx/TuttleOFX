#ifndef OPENIMAGEIO_READER_PROCESS_HPP
#define OPENIMAGEIO_READER_PROCESS_HPP

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
namespace openImageIO {
namespace reader {

/**
 *
 */
template<class View>
class OpenImageIOReaderProcess : public ImageGilProcessor<View>
{
protected:
	OpenImageIOReaderPlugin&    _plugin;        ///< Rendering plugin

public:
	OpenImageIOReaderProcess( OpenImageIOReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "OpenImageIOReaderProcess.tcc"

#endif
