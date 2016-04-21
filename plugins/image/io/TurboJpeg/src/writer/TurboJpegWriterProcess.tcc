#include "TurboJpegWriterAlgorithm.hpp"

#include <terry/globals.hpp>

#include <cstdio>

#include <turbojpeg.h>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace writer
{

template <class View>
TurboJpegWriterProcess<View>::TurboJpegWriterProcess(TurboJpegWriterPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationFromTopToBottom)
    , _plugin(effect)
{
    this->setNoMultiThreading();
}

template <class View>
void TurboJpegWriterProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void TurboJpegWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    BOOST_ASSERT(procWindowRoW == this->_srcPixelRod);
    using namespace boost::gil;

    View srcView = this->_srcView;

    try
    {
        writeImage(srcView);
    }
    catch(exception::Common& e)
    {
        e << exception::filename(_params.filepath);
        throw;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("Unable to write image")
                                                   << exception::dev(boost::current_exception_diagnostic_information())
                                                   << exception::filename(_params.filepath));
    }
}

template <class View>
void TurboJpegWriterProcess<View>::writeImage(View& src)
{
    using namespace boost::gil;
    using namespace terry;

    FILE* file = NULL;
    unsigned char* jpegBuf = NULL;

    int width = src.width();
    int height = src.height();
    unsigned long jpegSize = 0;
    int ps = TJPF_RGB;
    int flags = 0;
    int pitch = 0;
    int subsampling = 0;

    file = fopen(_params.filepath.c_str(), "wb");
    if(file == NULL)
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("TurboJpeg: Unable to open file")
                                                << exception::filename(_params.filepath));
    }
    fseek(file, 0, SEEK_SET);

    switch(_params.optimization)
    {
        case eTurboJpegOptimizationNone:
            flags = 0;
            break;
        case eTurboJpegOptimizationSSE3:
            flags |= TJ_FORCESSE3;
        case eTurboJpegOptimizationSSE2:
            flags |= TJ_FORCESSE2;
        case eTurboJpegOptimizationSSE:
            flags |= TJ_FORCESSE;
        case eTurboJpegOptimizationMMX:
            flags |= TJ_FORCEMMX;
    }

    switch(_params.subsampling)
    {
        case eTurboJpegSubsampling444:
            subsampling = TJSAMP_444;
            break;
        case eTurboJpegSubsampling422:
            subsampling = TJSAMP_422;
            break;
        case eTurboJpegSubsampling420:
            subsampling = TJSAMP_420;
            break;
        case eTurboJpegSubsamplingGray:
            subsampling = TJSAMP_GRAY;
            break;
        case eTurboJpegSubsampling440:
            subsampling = TJSAMP_440;
            break;
    }

    const tjhandle jpeghandle = tjInitCompress();

    rgb8_image_t tmpImg(src.width(), src.height());
    rgb8_view_t tmpVw(view(tmpImg));

    boost::gil::copy_and_convert_pixels(src, tmpVw);

    unsigned char* data = (unsigned char*)boost::gil::interleaved_view_get_raw_data(tmpVw);

    // int ret =
    tjCompress2(jpeghandle, data, width, pitch, height, ps, &jpegBuf, &jpegSize, subsampling, _params.quality, flags);

    fwrite((void*)jpegBuf, 1, jpegSize, file);

    delete[] jpegBuf;
    jpegBuf = NULL;
    tjDestroy(jpeghandle);
    fclose(file);
    file = NULL;
}
}
}
}
}
