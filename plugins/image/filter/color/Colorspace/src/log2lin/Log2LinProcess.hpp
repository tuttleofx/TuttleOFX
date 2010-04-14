#ifndef _LOG2LIN_PROCESS_HPP_
#define _LOG2LIN_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

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
namespace colorspace {
namespace log2lin {

template<class View>
class Log2LinProcess : public ImageGilFilterProcessor<View>
{
private:
	void log2lin(View & src, View & dst);
protected :
    Log2LinPlugin& _plugin; ///< Rendering plugin

public :
    Log2LinProcess( Log2LinPlugin &instance );

    void multiThreadProcessImages( const OfxRectI& procWindowRoW );
};

}
}
}
}

#include "Log2LinProcess.tcc"

#endif
