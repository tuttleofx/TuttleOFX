#ifndef _TUTTLE_PLUGIN_OPENIMAGEIO_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_OPENIMAGEIO_READER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

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

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "OpenImageIOReaderProcess.tcc"

#endif
