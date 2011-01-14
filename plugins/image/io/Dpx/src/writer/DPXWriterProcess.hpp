#ifndef _DPXWRITER_PROCESS_HPP_
#define _DPXWRITER_PROCESS_HPP_

#include <dpxEngine/dpxImage.hpp>

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

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
	DPXWriterProcessParams _params;
	tuttle::io::DpxHeader _dpxHeader;     ///< Dpx image header
	tuttle::io::DpxImage _dpxImg;         ///< Dpx image reader

	void setup( const OFX::RenderArguments& args );

	template<class WImage>
	void writeImage( View& src, const std::string& filepath, const int bitDepth, const tuttle::io::DpxImage::EDPX_CompType compType, const int packing );

public:
	DPXWriterProcess( DPXWriterPlugin& instance );

	void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "DPXWriterProcess.tcc"

#endif
