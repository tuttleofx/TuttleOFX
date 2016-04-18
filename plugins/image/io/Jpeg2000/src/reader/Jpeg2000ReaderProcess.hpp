#ifndef _TUTTLE_PLUGIN_JPEG2000_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_READER_PROCESS_HPP_

#include <openjpeg/J2KReader.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace reader
{

/**
 * @brief process
 *
 */
template <class View>
class Jpeg2000ReaderProcess : public ImageGilProcessor<View>
{
protected:
    Jpeg2000ReaderPlugin& _plugin; ///< Rendering plugin
    Jpeg2000ReaderProcessParams _params;

public:
    Jpeg2000ReaderProcess(Jpeg2000ReaderPlugin& instance);
    ~Jpeg2000ReaderProcess();

    void setup(const OFX::RenderArguments& args);

    // Do some processing
    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    template <class Layout>
    void switchLayoutCopy(const View& dstView);

    template <class WorkingPixel>
    void switchPrecisionCopy(const View& dstView);
};
}
}
}
}

#include "Jpeg2000ReaderProcess.tcc"

#endif
