#ifndef _LIN2LOG_PROCESS_HPP_
#define _LIN2LOG_PROCESS_HPP_

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

/**
 * @brief Base class
 *
 */
template<class View>
class Lin2LogProcess : public ImageGilProcessor<View>
{
protected :
    Lin2LogPlugin& _plugin;        ///< Rendering plugin
    View           _srcView;       ///< Source view

public :
    Lin2LogProcess( Lin2LogPlugin &instance );

    void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "Lin2LogProcess.tcc"

#endif
