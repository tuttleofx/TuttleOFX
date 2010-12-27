#ifndef _TUTTLE_PLUGIN_PNG_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_PNG_READER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

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

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "PngReaderProcess.tcc"

#endif
