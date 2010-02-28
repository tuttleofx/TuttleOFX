#ifndef LOG2LIN_PROCESS_HPP
#define LOG2LIN_PROCESS_HPP


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
namespace colorspace {
namespace log2lin {

template<class View>
class Log2LinProcess : public ImageGilProcessor<View>
{
protected :
    Log2LinPlugin& _plugin;        ///< Rendering plugin
    View           _srcView;       ///< Source view

public :
    Log2LinProcess( Log2LinPlugin &instance );

    void setup( const OFX::RenderArguments& args );

    void multiThreadProcessImages( const OfxRectI& procWindow );
};

}
}
}
}

#include "Log2LinProcess.tcc"

#endif  // LOG2LIN_PROCESS_HPP
