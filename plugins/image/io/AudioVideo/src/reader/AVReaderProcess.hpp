#ifndef _TUTTLE_PLUGIN_AV_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_AV_READER_PROCESS_HPP_

#include "AVReaderPlugin.hpp"

#include <terry/globals.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace reader
{

/**
 * @brief Audio Video process
 *
 */
template <class View>
class AVReaderProcess : public ImageGilProcessor<View>
{
protected:
    AVReaderPlugin& _plugin;

public:
    AVReaderProcess(AVReaderPlugin& instance);

    void setup(const OFX::RenderArguments& args);
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    template <typename FileView>
    View& readImage(View& dst, avtranscoder::VideoFrame& image);
};
}
}
}
}

#include "AVReaderProcess.tcc"

#endif
