#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderProcess.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace reader
{

using namespace boost::gil;
namespace bfs = boost::filesystem;

template <class View>
OpenImageIOReaderProcess<View>::OpenImageIOReaderProcess(OpenImageIOReaderPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void OpenImageIOReaderProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    // no tiles and no multithreading supported
    BOOST_ASSERT(procWindowRoW == this->_dstPixelRod);

    std::string filename = _plugin.getProcessParams(this->_renderArgs.time)._filepath;

    boost::scoped_ptr<OpenImageIO::ImageInput> img(OpenImageIO::ImageInput::create(filename));

    if(img.get() == NULL)
    {
        BOOST_THROW_EXCEPTION(OFX::Exception::Suite(kOfxStatErrValue));
    }
    OpenImageIO::ImageSpec spec;

    if(!img->open(filename, spec))
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("OIIO Reader: " + img->geterror())
                                                   << exception::filename(filename));
    }

    switch(spec.format.basetype)
    {
        case OpenImageIO::TypeDesc::UINT8:
        case OpenImageIO::TypeDesc::INT8:
        {
            switch(spec.nchannels)
            {
                case 1:
                    readImage<bits8, gray_layout_t, gray8_view_t>(this->_dstView, img, 1);
                    break;
                case 3:
                    readImage<bits8, rgb_layout_t, rgb8_view_t>(this->_dstView, img, 1);
                    break;
                case 4:
                    readImage<bits8, rgba_layout_t, rgba8_view_t>(this->_dstView, img, 1);
                    break;
                default:
                    img->close();
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("bad input format"));
                    break;
            }
            break;
        }
        case OpenImageIO::TypeDesc::HALF:
        case OpenImageIO::TypeDesc::UINT16:
        case OpenImageIO::TypeDesc::INT16:
        {
            switch(spec.nchannels)
            {
                case 1:
                    readImage<bits16, gray_layout_t, gray16_view_t>(this->_dstView, img, 2);
                    break;
                case 3:
                    readImage<bits16, rgb_layout_t, rgb16_view_t>(this->_dstView, img, 2);
                    break;
                case 4:
                    readImage<bits16, rgba_layout_t, rgba16_view_t>(this->_dstView, img, 2);
                    break;
                default:
                    img->close();
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("bad input format"));
                    break;
            }
            break;
        }
        case OpenImageIO::TypeDesc::UINT32:
        case OpenImageIO::TypeDesc::INT32:
        case OpenImageIO::TypeDesc::UINT64:
        case OpenImageIO::TypeDesc::INT64:
        case OpenImageIO::TypeDesc::FLOAT:
        case OpenImageIO::TypeDesc::DOUBLE:
        {
            switch(spec.nchannels)
            {
                case 1:
                    readImage<bits32f, gray_layout_t, gray32f_view_t>(this->_dstView, img, 4);
                    break;
                case 3:
                    readImage<bits32f, rgb_layout_t, rgb32f_view_t>(this->_dstView, img, 4);
                    break;
                case 4:
                    readImage<bits32f, rgba_layout_t, rgba32f_view_t>(this->_dstView, img, 4);
                    break;
                default:
                    img->close();
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("bad input format"));
                    break;
            }
            break;
        }
        case OpenImageIO::TypeDesc::STRING:
        case OpenImageIO::TypeDesc::PTR:
        case OpenImageIO::TypeDesc::LASTBASE:
        case OpenImageIO::TypeDesc::UNKNOWN:
        case OpenImageIO::TypeDesc::NONE:
        default:
        {
            img->close();
            BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("bad input format"));
        }
    }

    img->close();
}

/**
 */
template <class View>
template <typename bitDepth, typename layout, typename fileView>
View& OpenImageIOReaderProcess<View>::readImage(View& dst, boost::scoped_ptr<OpenImageIO::ImageInput>& img, int pixelSize)
{
    using namespace boost;
    using namespace boost::gil;
    using namespace OpenImageIO;

    typedef pixel<bitDepth, layout> pixel_t;
    typedef image<pixel_t, false> image_t;

    image_t tmpImg(dst.width(), dst.height());
    fileView tmpView = view(tmpImg);

    const stride_t xstride = tmpView.num_channels() * pixelSize;
    const stride_t ystride = tmpView.pixels().row_size();
    const stride_t zstride = ystride * tmpView.height();

    img->read_image(TypeDesc::UNKNOWN,        // it's to not convert into OpenImageIO, convert with GIL
                    &((*tmpView.begin())[0]), // get the adress of the first channel value from the first pixel
                    xstride, ystride, zstride, &progressCallback, this);

    copy_and_convert_pixels(tmpView, dst);
    return dst;
}
}
}
}
}
