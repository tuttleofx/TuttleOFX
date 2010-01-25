/**
 * @file Lin2LogProcess.hpp
 * @brief
 * @author
 * @date    08/01/10 15:19
 *
 */
#ifndef LIN2LOG_PROCESS_HPP
#define LIN2LOG_PROCESS_HPP


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
namespace lin2log {

/**
 * @brief Base class
 *
 */
template<class View>
class Lin2LogProcess : public tuttle::plugin::ImageGilProcessor<View>, public tuttle::plugin::Progress
{
    typedef typename View::value_type value_t;
protected :
    Lin2LogPlugin&    _plugin;        ///< Rendering plugin
    View                  _srcView;       ///< Source view

public :
    Lin2LogProcess<View>(Lin2LogPlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}
}
}
}

#include "Lin2LogProcess.tcc"

#endif  // LIN2LOG_PROCESS_HPP
