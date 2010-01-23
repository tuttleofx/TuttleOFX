#ifndef DPXWRITER_PROCESS_HPP
#define DPXWRITER_PROCESS_HPP

#include "../dpxEngine/dpxImage.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

/**
 * @brief Base class
 *
 */
template<class View>
class DPXWriterProcess : public tuttle::plugin::ImageGilProcessor<View>,
	public tuttle::plugin::Progress
{
typedef typename View::value_type value_t;

protected:
	DPXWriterPlugin&      _plugin;        ///< Rendering plugin
	OFX::StringParam*     _filepath;      ///< File path
	OFX::ChoiceParam*     _bitDepth;      ///< Bit depth
	OFX::ChoiceParam*     _componentsType; ///< Components type
	OFX::BooleanParam*    _compressed;    ///< Bit streaming
	View _srcView;                        ///< Source view
	tuttle::io::DpxHeader _dpxHeader;     ///< Dpx image header
	tuttle::io::DpxImage _dpxImg;         ///< Dpx image reader

	template<class CONV_IMAGE>
	void writeImage( View& src, std::string& filepath, int bitDepth, tuttle::io::DpxImage::EDPX_CompType compType, int packing ) throw( tuttle::plugin::PluginException );

public:
	DPXWriterProcess<View>( DPXWriterPlugin & instance );

	// set up and run a processor
	void setupAndProcess( const OFX::RenderArguments& args );

	// Do some processing
	void multiThreadProcessImages( OfxRectI procWindow );
};

}
}
}
}

#include "DPXWriterProcess.tcc"

#endif
