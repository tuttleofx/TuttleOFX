/**
 * @file MergeProcess.hpp
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */
#ifndef MERGE_PROCESS_HPP
#define MERGE_PROCESS_HPP


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
namespace merge {

/**
 * @brief Base class
 *
 */
template<class View>
class MergeProcess : public tuttle::plugin::ImageGilProcessor<View>, public tuttle::plugin::Progress
{
    typedef typename View::value_type value_t;
protected :
    MergePlugin&        _plugin;    ///< Rendering plugin
    View                _srcViewA;  ///< Source view A
    View                _srcViewB;  ///< Source view B

public :
    MergeProcess<View>(MergePlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}
}
}

#include "MergeProcess.tcc"

#endif  // MERGE_PROCESS_HPP
