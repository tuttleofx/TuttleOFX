#ifndef _TUTTLE_PLUGIN_JPEG_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_JPEG_READER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace reader {

/**
 *
 */
template<class View>
class JpegReaderProcess : public ImageGilProcessor<View>
{
protected:
	JpegReaderPlugin&    _plugin;        ///< Rendering plugin

	JpegReaderProcessParams _params;
	
public:
	JpegReaderProcess( JpegReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );
	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst );
};

}
}
}
}

#include "JpegReaderProcess.tcc"

#endif
