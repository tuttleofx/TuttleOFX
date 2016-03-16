#ifndef _TUTTLE_PLUGIN_VIEWER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_VIEWER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace viewer
{

/**
 * @brief Viewer process
 *
 */
template <class View>
class ViewerProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    ViewerPlugin& _plugin;               ///< Rendering plugin
    ViewerProcessParams<Scalar> _params; ///< parameters

public:
    ViewerProcess(ViewerPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ViewerProcess.tcc"

#endif
