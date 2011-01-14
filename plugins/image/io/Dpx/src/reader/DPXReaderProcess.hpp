#ifndef _TUTTLE_PLUGIN_DPX_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_DPX_READER_PROCESS_HPP_

#include <dpxEngine/dpxImage.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

/**
 * @brief Base class to read dpx files
 */
template<class View>
class DPXReaderProcess : public ImageGilProcessor<View>
{
public:
	DPXReaderProcess( DPXReaderPlugin& instance );
	~DPXReaderProcess();

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	// Read dpx image
	View& readImage( View& dst );

protected:
	template<class T, class DST_V>
	void bitStreamToView( DST_V& dst, const int nc, const int channelSize );

protected:
	DPXReaderPlugin&    _plugin;        ///< Rendering plugin
	DPXReaderProcessParams _params;
	
	tuttle::io::DpxImage _dpxImage;

};

}
}
}
}

#include "DPXReaderProcess.tcc"

#endif
