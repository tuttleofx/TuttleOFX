#include "JpegReaderDefinitions.hpp"
#include "JpegReaderProcess.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg
{
namespace reader
{

using namespace boost::gil;
namespace bfs = boost::filesystem;

typedef any_image<boost::mpl::vector<rgba8_image_t, rgba16_image_t, rgba32f_image_t, rgb8_image_t, rgb16_image_t,
                                     rgb32f_image_t> > any_image_t;
typedef any_image_t::view_t any_view_t;

template <class View>
JpegReaderProcess<View>::JpegReaderProcess(JpegReaderPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
void JpegReaderProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void JpegReaderProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    // no tiles and no multithreading supported
    BOOST_ASSERT(procWindowRoW == this->_dstPixelRod);
    readImage(this->_dstView);
}

/**
 */
template <class View>
View& JpegReaderProcess<View>::readImage(View& dst)
{
    any_image_t anyImg;

    try
    {
        jpeg_read_image(_params._filepath, anyImg);

        any_view_t srcView = view(anyImg);
        srcView = subimage_view(srcView, 0, 0, dst.width(), dst.height());
        copy_and_convert_pixels(srcView, dst);
    }
    catch(boost::exception& e)
    {
        e << exception::user("Jpeg: Unable to open file.");
        e << exception::filename(_params._filepath);
        throw;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("Jpeg: Unable to open file.")
                                                << exception::dev(boost::current_exception_diagnostic_information())
                                                << exception::filename(_params._filepath));
    }
    return dst;
}
}
}
}
}
