#ifndef _TUTTLE_PLUGIN_CHECKERBOARD_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CHECKERBOARD_PROCESS_HPP_


#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
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
class CheckerboardProcess : public ImageGilProcessor<View>
{
protected :
    CheckerboardPlugin&    _plugin;        ///< Rendering plugin
    View                  _srcView;       ///< Source view

public :
    CheckerboardProcess(CheckerboardPlugin &instance);

    void setup(const OFX::RenderArguments &args);

    void multiThreadProcessImages( const OfxRectI& procWindow);
};

}
}
}

#include "CheckerboardProcess.tcc"

#endif
