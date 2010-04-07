#ifndef _DPX_READER_PROCESS_HPP_
#define _DPX_READER_PROCESS_HPP_

#include <dpxEngine/dpxImage.hpp>

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
namespace dpx {
namespace reader {

/**
 * @brief Base class to read dpx files
 */
template<class View>
class DPXReaderProcess : public ImageGilProcessor<View>
{
protected:
	OFX::StringParam*   _filepath;      ///< File path
	DPXReaderPlugin&    _plugin;        ///< Rendering plugin

	template<class T, class DST_V>
	void bitStreamToView( DST_V& dst, const int nc, const int channelSize );

public:
	DPXReaderProcess( DPXReaderPlugin & instance );

	void setup( const OFX::RenderArguments& args );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );

	// Read dpx image
	View& readImage( View& dst, std::string& filepath ) throw( tuttle::plugin::PluginException );
};

}
}
}
}

#include "DPXReaderProcess.tcc"

#endif
