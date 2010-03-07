#ifndef _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PROCESS_HPP_
#define _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PROCESS_HPP_

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
namespace debugImageEffectApi {

/**
 * @brief DebugImageEffectApi process
 *
 */
template<class View>
class DebugImageEffectApiProcess : public ImageGilFilterProcessor<View>
{
protected :
    DebugImageEffectApiPlugin&    _plugin;        ///< Rendering plugin

public:
    DebugImageEffectApiProcess( DebugImageEffectApiPlugin& instance );

    // Do some processing
    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}

#include "DebugImageEffectApiProcess.tcc"

#endif
