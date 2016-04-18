#ifndef _TUTTLE_PLUGIN_COMPONENT_PROCESS_HPP_
#define _TUTTLE_PLUGIN_COMPONENT_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <terry/color/components.hpp>

namespace tuttle
{
namespace plugin
{
namespace component
{

/**
 * @brief Component process
 *
 */
template <class SView, class DView>
class ComponentProcess : public ImageGilFilterProcessor<SView, DView>
{
public:
    typedef typename SView::value_type Pixel;
    typedef typename boost::gil::channel_type<SView>::type Channel;
    typedef float Scalar;

protected:
    ComponentPlugin& _plugin;                               ///< Rendering plugin
    terry::color::components::ConvertionParameters _params; ///< parameters

public:
    ComponentProcess(ComponentPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ComponentProcess.tcc"

#endif
