#ifndef _TUTTLE_PLUGIN_PNG_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_PNG_READER_PROCESS_HPP_

#include <png.h>

#define png_infopp_NULL (png_infopp) NULL
#ifndef int_p_NULL
#define int_p_NULL (int*) NULL
#endif

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle
{
namespace plugin
{
namespace png
{
namespace reader
{

/**
 *
 */
template <class View>
class PngReaderProcess : public ImageGilProcessor<View>
{
protected:
    PngReaderPlugin& _plugin; ///< Rendering plugin

    PngReaderProcessParams _params;

public:
    PngReaderProcess(PngReaderPlugin& instance);

    void setup(const OFX::RenderArguments& args);
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    View& readImage(View& dst);
};
}
}
}
}

#include "PngReaderProcess.tcc"

#endif
