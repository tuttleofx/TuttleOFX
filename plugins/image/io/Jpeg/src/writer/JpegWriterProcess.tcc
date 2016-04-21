#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg
{
namespace writer
{

template <class View>
JpegWriterProcess<View>::JpegWriterProcess(JpegWriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
void JpegWriterProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template <class View>
void JpegWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    BOOST_ASSERT(procWindowRoW == this->_srcPixelRod);
    using namespace boost::gil;

    View srcView = this->_srcView;

    try
    {
        writeImage<bits8>(srcView);
    }
    catch(exception::Common& e)
    {
        e << exception::filename(_params._filepath);
        throw;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("Unable to write image")
                                                   << exception::dev(boost::current_exception_diagnostic_information())
                                                   << exception::filename(_params._filepath));
    }
}

/**
 *
 */
template <class View>
template <class Bits>
void JpegWriterProcess<View>::writeImage(View& src)
{
    using namespace boost::gil;
    using namespace terry;

    typedef pixel<Bits, rgb_layout_t> OutPixelType;
    jpeg_write_view(_params._filepath, color_converted_view<OutPixelType>(src), _params._quality);
}
}
}
}
}
