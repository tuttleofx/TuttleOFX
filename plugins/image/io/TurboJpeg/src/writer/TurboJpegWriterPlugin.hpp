#ifndef _TUTTLE_PLUGIN_TURBOJPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TURBOJPEG_WRITER_PLUGIN_HPP_

#include "TurboJpegWriterDefinitions.hpp"

#include <tuttle/ioplugin/context/WriterPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace writer
{

struct TurboJpegWriterProcessParams
{
    std::string filepath;
    ETurboJpegOptimization optimization;
    ETurboJpegSubsampling subsampling;
    int quality;
    int premult;
};

/**
 * @brief TurboJpeg plugin
 */
class TurboJpegWriterPlugin : public WriterPlugin
{
public:
    TurboJpegWriterPlugin(OfxImageEffectHandle handle);

public:
    TurboJpegWriterProcessParams getProcessParams(const OfxTime time);

    void render(const OFX::RenderArguments& args);

public:
    OFX::BooleanParam* _paramPremult;     ///< premult output by alpha
    OFX::IntParam* _paramQuality;         ///< quality / compression for jpeg
    OFX::ChoiceParam* _paramSubsampling;  ///< select the subsampling method (4:4:4, 4:2:2, 4:2:0; Gray, 4:4:0)
    OFX::ChoiceParam* _paramOptimization; ///< TurboJpeg SIMD optimization
};
}
}
}
}

#endif
