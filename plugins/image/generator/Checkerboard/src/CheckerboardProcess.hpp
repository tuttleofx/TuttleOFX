#ifndef _TUTTLE_PLUGIN_CHECKERBOARD_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CHECKERBOARD_PROCESS_HPP_


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
namespace checkerboard {

/**
 * @brief Base class
 *
 */
template<class View>
class CheckerboardProcess : public tuttle::plugin::ImageGilProcessor<View>, public tuttle::plugin::Progress
{
    typedef typename View::value_type value_t;
protected :
    CheckerboardPlugin&    _plugin;        ///< Rendering plugin
    View                  _srcView;       ///< Source view

public :
    CheckerboardProcess<View>(CheckerboardPlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}
}
}

#include "CheckerboardProcess.tcc"

#endif
