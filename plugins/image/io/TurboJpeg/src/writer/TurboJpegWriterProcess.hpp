#ifndef _TUTTLE_PLUGIN_TURBOJPEG_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_TURBOJPEG_WRITER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace writer
{

/**
 * @brief TurboJpeg process
 *
 */
template <class View>
class TurboJpegWriterProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    TurboJpegWriterPlugin& _plugin;       ///< Rendering plugin
    TurboJpegWriterProcessParams _params; ///< parameters

public:
    TurboJpegWriterProcess(TurboJpegWriterPlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    void writeImage(View& src);
};
}
}
}
}

#include "TurboJpegWriterProcess.tcc"

#endif
