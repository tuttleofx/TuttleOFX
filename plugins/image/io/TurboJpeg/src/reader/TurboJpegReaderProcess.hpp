#ifndef _TUTTLE_PLUGIN_TURBOJPEG_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TURBOJPEG_READER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace reader
{

/**
 * @brief TurboJpeg process
 *
 */
template <class View>
class TurboJpegReaderProcess : public ImageGilProcessor<View>
{
protected:
    TurboJpegReaderPlugin& _plugin;       ///< Rendering plugin
    TurboJpegReaderProcessParams _params; ///< parameters

public:
    TurboJpegReaderProcess(TurboJpegReaderPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    void readImage(View& dst);
};
}
}
}
}

#include "TurboJpegReaderProcess.tcc"

#endif
