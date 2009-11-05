/**
 * @file InvertProcess.hpp
 * @brief
 * @author
 * @date    01/10/09 12:01
 *
 */
#ifndef INVERT_PROCESS_HPP
#define INVERT_PROCESS_HPP

#include "InvertPlugin.hpp"

#include "tuttle/common/utils/global.hpp"
#include "tuttle/plugin/ImageGilProcessor.hpp"
#include "tuttle/plugin/Progress.hpp"
#include "tuttle/plugin/PluginException.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>


namespace tuttle {

/**
 * @brief Base class
 *
 */
template<class View>
class InvertProcess : public tuttle::ofx::ImageGilProcessor<View>, public tuttle::ofx::Progress
{
    typedef typename View::value_type value_t;
protected :
    InvertPlugin&    _plugin;        ///< Rendering plugin
    View                  _srcView;       ///< Source view

public :
    InvertProcess<View>(InvertPlugin &instance);

    // set up and run a processor
    void setupAndProcess(const OFX::RenderArguments &args);

    // Do some processing
    void multiThreadProcessImages(OfxRectI procWindow);
};

}

#include "InvertProcess.tcc"

#endif  // INVERT_PROCESS_HPP
