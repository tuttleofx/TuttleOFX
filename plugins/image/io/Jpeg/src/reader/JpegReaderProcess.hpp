#ifndef JPEG_READER_PROCESS_HPP
#define JPEG_READER_PROCESS_HPP

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
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

public:
	JpegReaderProcess( JpegReaderPlugin& instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	View& readImage( View& dst, const std::string& filepath );
};

}
}
}
}

#include "JpegReaderProcess.tcc"

#endif
