#ifndef _TUTTLE_PLUGIN_FLIP_PROCESS_HPP_
#define _TUTTLE_PLUGIN_FLIP_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle
{
namespace plugin
{
namespace flip
{

/**
 * @brief Base class
 *
 */
template <class View>
class FlipProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;

protected:
    FlipPlugin& _plugin;       ///< Rendering plugin
    FlipProcessParams _params; ///< process parameters

public:
    FlipProcess<View>(FlipPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "FlipProcess.tcc"

#endif
