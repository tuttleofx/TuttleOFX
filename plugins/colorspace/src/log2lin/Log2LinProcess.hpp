/**
 * @file Log2LinProcess.hpp
 * @brief
 * @author
 * @date    08/01/10 15:39
 *
 */
#ifndef LOG2LIN_PROCESS_HPP
#define LOG2LIN_PROCESS_HPP


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
namespace colorspace {
namespace log2lin {

/**
 * @brief Base class
 *
 */
template<class View>
class Log2LinProcess : public tuttle::plugin::ImageGilProcessor<View>, public tuttle::plugin::Progress
{
    typedef typename View::value_type value_t;
protected :
    Log2LinPlugin&    _plugin;        ///< Rendering plugin
    View                  _srcView;       ///< Source view

public :
    Log2LinProcess<View>(Log2LinPlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}
}
}
}

#include "Log2LinProcess.tcc"

#endif  // LOG2LIN_PROCESS_HPP
