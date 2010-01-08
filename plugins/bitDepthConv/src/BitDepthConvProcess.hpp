/**
 * @file BitDepthConvProcess.hpp
 * @brief
 * @author
 * @date    08/01/10 17:46
 *
 */
#ifndef BITDEPTHCONV_PROCESS_HPP
#define BITDEPTHCONV_PROCESS_HPP


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
namespace bitDepthConvert {

/**
 * @brief Base class
 *
 */
template<class SView, class DView>
class BitDepthConvProcess : public tuttle::plugin::ImageGilProcessor<DView>, public tuttle::plugin::Progress
{
	typedef typename SView::value_type svalue_t;
	typedef typename DView::value_type dvalue_t;
protected :
    BitDepthConvPlugin&    _plugin;        ///< Rendering plugin
    SView                  _srcView;       ///< Source view

public :
    BitDepthConvProcess<SView, DView>(BitDepthConvPlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}
}
}

#include "BitDepthConvProcess.tcc"

#endif  // BITDEPTHCONV_PROCESS_HPP
