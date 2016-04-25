#include "TurboJpegReaderAlgorithm.hpp"

#include <boost/gil/gil_all.hpp>

#include <turbojpeg.h>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace reader
{

using namespace boost::gil;

template <class View>
TurboJpegReaderProcess<View>::TurboJpegReaderProcess(TurboJpegReaderPlugin& effect)
    : ImageGilProcessor<View>(effect, eImageOrientationFromTopToBottom)
    , _plugin(effect)
{
    this->setNoMultiThreading();
}

template <class View>
void TurboJpegReaderProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void TurboJpegReaderProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    BOOST_ASSERT(procWindowRoW == this->_dstPixelRod);
    readImage(this->_dstView);
}

template <class View>
void TurboJpegReaderProcess<View>::readImage(View& dst)
{
    FILE* file = NULL;
    unsigned char* jpegbuf = NULL;
    unsigned char* rgbbuf = NULL;
    unsigned long jpgbufsize = 0;

    const tjhandle jpeghandle = tjInitDecompress();
    int width = 0;
    int height = 0;
    int jpegsubsamp = -1;
    int yuvsize = 0;
    int bufsize = 0;
    int ret = 0;
    int ps = TJPF_RGB;
    int flags = 0;

    file = fopen(_params.filepath.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    jpgbufsize = ftell(file);
    jpegbuf = new unsigned char[jpgbufsize];

    fseek(file, 0, SEEK_SET);
    fread(jpegbuf, jpgbufsize, 1, file);

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

    if(_params.fastUpsampling)
    {
        flags |= TJ_FASTUPSAMPLE;
    }

    ret = tjDecompressHeader2(jpeghandle, jpegbuf, jpgbufsize, &width, &height, &jpegsubsamp);
    if(ret != 0)
    {
        BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::user(tjGetErrorStr())
                                                        << exception::filename(_params.filepath));
    }

    yuvsize = tjBufSizeYUV(width, height, jpegsubsamp);
    bufsize = tjBufSize(width, height, jpegsubsamp);

    rgbbuf = new unsigned char[bufsize];

    ret = tjDecompress2(jpeghandle, jpegbuf, jpgbufsize, rgbbuf, width, 0, height, ps, flags);
    if(ret != 0)
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user(tjGetErrorStr())
                                                << exception::filename(_params.filepath));
    }

    rgb8_view_t bufferView = interleaved_view(width, height, (typename rgb8_view_t::value_type*)(rgbbuf),
                                              width * sizeof(typename rgb8_view_t::value_type));

    boost::gil::copy_and_convert_pixels(bufferView, dst);

    delete[] jpegbuf;
    jpegbuf = NULL;
    delete[] rgbbuf;
    rgbbuf = NULL;
    tjDestroy(jpeghandle);
    fclose(file);
    file = NULL;
}
}
}
}
}
