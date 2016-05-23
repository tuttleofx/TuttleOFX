#ifndef _TUTTLE_PLUGIN_JPEG2000_WRITER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_WRITER_PROCESS_HPP_

#include <openjpeg/J2KWriter.hpp>

#include "Jpeg2000WriterPlugin.hpp"
#include "Jpeg2000WriterProcess.hpp"

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace writer
{

/**
 * @brief process
 *
 */
template <class View>
class Jpeg2000WriterProcess : public ImageGilFilterProcessor<View>
{
public:
    Jpeg2000WriterProcess(Jpeg2000WriterPlugin& instance);
    ~Jpeg2000WriterProcess();

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

private:
    template <typename SImg>
    void writeImage(const View& srcView, const int& bitDepth);

protected:
    Jpeg2000WriterPlugin& _plugin; ///< Rendering plugin
    Jpeg2000ProcessParams _params;
    tuttle::io::J2KWriter _writer; ///< Writer engine
};
}
}
}
}

#include "Jpeg2000WriterProcess.tcc"

#endif
