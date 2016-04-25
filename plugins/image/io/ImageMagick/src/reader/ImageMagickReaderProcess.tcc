#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderProcess.hpp"

#include <tuttle/common/system/system.hpp>
#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <magick/MagickCore.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

#if(MAGICKCORE_QUANTUM_DEPTH == 8)
#ifdef _MSC_VER
#pragma NOTE("MAGICKCORE_QUANTUM_DEPTH is 8")
#else
#warning "MAGICKCORE_QUANTUM_DEPTH is 8"
#endif
#define MAGICKCORE_QUANTUM_SUPPORT boost::uint32_t
#elif(MAGICKCORE_QUANTUM_DEPTH == 16)
#ifdef _MSC_VER
#pragma NOTE("MAGICKCORE_QUANTUM_DEPTH is 16")
#else
#warning "MAGICKCORE_QUANTUM_DEPTH is 16"
#endif
#define MAGICKCORE_QUANTUM_SUPPORT boost::uint64_t
#elif(MAGICKCORE_QUANTUM_DEPTH == 32)
#ifdef _MSC_VER
#pragma NOTE("MAGICKCORE_QUANTUM_DEPTH is 32")
#else
#warning "MAGICKCORE_QUANTUM_DEPTH is 32"
#define MAGICKCORE_QUANTUM_SUPPORT boost::uint64_t
#endif
#endif

BOOST_STATIC_ASSERT(MAGICKCORE_QUANTUM_DEPTH != 8 || MAGICKCORE_QUANTUM_DEPTH != 16);

namespace boost
{
namespace gil
{

typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 0, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgba_quantum_packed_channel0_t;
typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 1, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgba_quantum_packed_channel1_t;
typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 2, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgba_quantum_packed_channel2_t;
typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 3, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgba_quantum_packed_channel3_t;

typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 0, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgb_quantum_packed_channel0_t;
typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 1, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgb_quantum_packed_channel1_t;
typedef const packed_channel_reference<MAGICKCORE_QUANTUM_SUPPORT, MAGICKCORE_QUANTUM_DEPTH * 2, MAGICKCORE_QUANTUM_DEPTH,
                                       true> rgb_quantum_packed_channel2_t;

// RGBA
typedef mpl::vector4<rgba_quantum_packed_channel0_t, rgba_quantum_packed_channel1_t, rgba_quantum_packed_channel2_t,
                     rgba_quantum_packed_channel3_t> rgba_quantum_packed_channels_t;
typedef packed_pixel<MAGICKCORE_QUANTUM_SUPPORT, rgba_quantum_packed_channels_t, bgra_layout_t> bgra_quantum_packed_pixel_t;
typedef view_type_from_pixel<bgra_quantum_packed_pixel_t>::type bgra_quantum_packed_view_t;
typedef image<bgra_quantum_packed_pixel_t, false> bgra_quantum_packed_image_t;

// RGB
typedef mpl::vector3<rgb_quantum_packed_channel0_t, rgb_quantum_packed_channel1_t, rgb_quantum_packed_channel2_t>
    rgb_quantum_packed_channels_t;
typedef packed_pixel<MAGICKCORE_QUANTUM_SUPPORT, rgb_quantum_packed_channels_t, bgr_layout_t> bgr_quantum_packed_pixel_t;
typedef view_type_from_pixel<bgr_quantum_packed_pixel_t>::type bgr_quantum_packed_view_t;
typedef image<bgr_quantum_packed_pixel_t, false> bgr_quantum_packed_image_t;
}
}

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace reader
{

using namespace boost::gil;
namespace bfs = boost::filesystem;

template <class View>
ImageMagickReaderProcess<View>::ImageMagickReaderProcess(ImageMagickReaderPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
void ImageMagickReaderProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void ImageMagickReaderProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    // no tiles and no multithreading supported
    BOOST_ASSERT(procWindowRoW == this->_dstPixelRod);
    readGilImage(this->_dstView, _plugin.getProcessParams(this->_renderArgs.time)._filepath);
}

template <class SView, class DView>
void copy_and_convert_from_buffer(Image* image, DView& dst)
{
    ExceptionInfo exception;
    PixelPacket* buffer = GetAuthenticPixels(image, 0, 0, dst.width(), dst.height(), &exception);

    SView bufferView = interleaved_view(dst.width(), dst.height(), (typename SView::value_type*)(buffer),
                                        dst.width() * sizeof(typename SView::value_type));

    boost::gil::copy_and_convert_pixels(bufferView, dst);
}

/**
 */
template <class View>
View& ImageMagickReaderProcess<View>::readGilImage(View& dst, const std::string& filepath)
{
    ImageInfo* imageInfo = AcquireImageInfo();
    GetImageInfo(imageInfo);
    strcpy(imageInfo->filename, filepath.c_str());
    ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
    GetExceptionInfo(exceptionsInfo);

    ::Image* image = ReadImage(imageInfo, exceptionsInfo);

    CatchException(exceptionsInfo);

    if(image->colorspace != RGBColorspace && image->colorspace != GRAYColorspace &&
       image->colorspace != TransparentColorspace)
    {
        TUTTLE_LOG_WARNING(" Change colorspace to RGB.");
        SetImageColorspace(image, RGBColorspace);
    }

    QuantumType colorType = GetQuantumType(image, exceptionsInfo);

    switch(colorType)
    {
        case RGBQuantum:
        {
#if(MAGICKCORE_QUANTUM_DEPTH == 8)
            rgb8_image_t tmpImg(dst.width(), dst.height());
            rgb8_view_t tmpVw = view(tmpImg);

            copy_and_convert_from_buffer<bgr_quantum_packed_view_t, rgb8_view_t>(image, tmpVw);
#elif(MAGICKCORE_QUANTUM_DEPTH == 16)
            rgb16_image_t tmpImg(dst.width(), dst.height());
            rgb16_view_t tmpVw = view(tmpImg);

            copy_and_convert_from_buffer<bgr_quantum_packed_view_t, rgb16_view_t>(image, tmpVw);
#elif(MAGICKCORE_QUANTUM_DEPTH == 32)
            rgb32_image_t tmpImg(dst.width(), dst.height());
            rgb32_view_t tmpVw = view(tmpImg);

            copy_and_convert_from_buffer<bgr_quantum_packed_view_t, rgb32_view_t>(image, tmpVw);
#endif
            copy_and_convert_pixels(tmpVw, dst);
            break;
        }
        case RGBAQuantum:
        {
#if(MAGICKCORE_QUANTUM_DEPTH == 8)
            copy_and_convert_from_buffer<bgra8_view_t, View>(image, dst);
#elif(MAGICKCORE_QUANTUM_DEPTH == 16)
            copy_and_convert_from_buffer<bgra16_view_t, View>(image, dst);
#elif(MAGICKCORE_QUANTUM_DEPTH == 32)
            copy_and_convert_from_buffer<bgra32_view_t, View>(image, dst);
#endif
            break;
        }
        case AlphaQuantum:
        case GrayQuantum:
        {

#if(MAGICKCORE_QUANTUM_DEPTH == 8)
            rgba8_image_t tmpImg(dst.width(), dst.height());
            rgba8_view_t tmpVw = view(tmpImg);

            copy_and_convert_from_buffer<bgra_quantum_packed_view_t, rgba8_view_t>(image, tmpVw);
#elif(MAGICKCORE_QUANTUM_DEPTH == 16)
            rgba16_image_t tmpImg(dst.width(), dst.height());
            rgba16_view_t tmpVw = view(tmpImg);

            copy_and_convert_from_buffer<bgra_quantum_packed_view_t, rgba16_view_t>(image, tmpVw);
#elif(MAGICKCORE_QUANTUM_DEPTH == 32)
            rgba32_image_t tmpImg(dst.width(), dst.height());
            rgba32_view_t tmpVw = view(tmpImg);

            copy_and_convert_from_buffer<bgra_quantum_packed_view_t, rgba32_view_t>(image, tmpVw);
#endif
            copy_and_convert_pixels(boost::gil::nth_channel_view(tmpVw, 0), dst);

            break;
        }
        default:
        {
            BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user(
                                      "Unknown color type (" + boost::lexical_cast<std::string>(colorType) + ")"));
            break;
        }
    }

    image = DestroyImage(image);
    imageInfo = DestroyImageInfo(imageInfo);
    exceptionsInfo = DestroyExceptionInfo(exceptionsInfo);
    return dst;
}
}
}
}
}
