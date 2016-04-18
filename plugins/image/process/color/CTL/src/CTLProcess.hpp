#ifndef _TUTTLE_PLUGIN_CTL_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CTL_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

#include <CtlSimdInterpreter.h>

namespace tuttle
{
namespace plugin
{
namespace ctl
{

/**
 * @brief CTL process
 *
 */
template <class View>
class CTLProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    CTLPlugin& _plugin;               ///< Rendering plugin
    CTLProcessParams<Scalar> _params; ///< parameters

    Ctl::SimdInterpreter _interpreter;

public:
    CTLProcess(CTLPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "CTLProcess.tcc"

#endif
