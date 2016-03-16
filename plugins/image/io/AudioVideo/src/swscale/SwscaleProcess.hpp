#ifndef _TUTTLE_PLUGIN_SWSCALE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_SWSCALE_PROCESS_HPP_

#include "SwscalePlugin.hpp"

#include <tuttle/plugin/ImageFilterProcessor.hpp>

#include <AvTranscoder/transform/VideoTransform.hpp>

namespace tuttle
{
namespace plugin
{
namespace swscale
{

/**
 * @brief Swscale process
 *
 */
class SwscaleProcess : public ImageFilterProcessor
{
protected:
    SwscalePlugin& _plugin;       ///< Rendering plugin
    SwscaleProcessParams _params; ///< parameters
    avtranscoder::VideoTransform _transform;

public:
    SwscaleProcess(SwscalePlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#endif
