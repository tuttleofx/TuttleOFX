#ifndef _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PROCESS_HPP_
#define _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PROCESS_HPP_

#include "DebugImageEffectApiPlugin.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace debugImageEffectApi
{

/**
 * @brief DebugImageEffectApi process
 *
 */
template <class View>
class DebugImageEffectApiProcess : public ImageGilFilterProcessor<View>
{
protected:
    DebugImageEffectApiPlugin& _plugin; ///< Rendering plugin

public:
    DebugImageEffectApiProcess(DebugImageEffectApiPlugin& instance);

    void setup(const OFX::RenderArguments& args);

    // Do some processing
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "DebugImageEffectApiProcess.tcc"

#endif
