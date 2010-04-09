#ifndef DPXWRITER_PROCESS_HPP
#define DPXWRITER_PROCESS_HPP

#include "../dpxEngine/dpxImage.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

/**
 * @brief Dpx writer
 */
template<class View>
class DPXWriterProcess : public ImageGilFilterProcessor<View>
{
protected:
	DPXWriterPlugin&      _plugin;        ///< Rendering plugin
	tuttle::io::DpxHeader _dpxHeader;     ///< Dpx image header
	tuttle::io::DpxImage _dpxImg;         ///< Dpx image reader

	template<class CONV_IMAGE>
	void writeImage( View& src, std::string& filepath, int bitDepth, tuttle::io::DpxImage::EDPX_CompType compType, int packing ) throw( tuttle::plugin::PluginException );

public:
	DPXWriterProcess( DPXWriterPlugin & instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "DPXWriterProcess.tcc"

#endif
