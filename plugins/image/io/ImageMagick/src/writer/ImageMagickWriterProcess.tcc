#include "ImageMagickWriterDefinitions.hpp"
#include "ImageMagickWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace writer
{

template <class View>
ImageMagickWriterProcess<View>::ImageMagickWriterProcess(ImageMagickWriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template <class View>
void ImageMagickWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    BOOST_ASSERT(procWindowRoW == this->_srcPixelRod);
    using namespace boost::gil;
    ImageMagickWriterProcessParams params = _plugin.getProcessParams(this->_renderArgs.time);
    try
    {
        writeImage<bits8>(this->_srcView, params._filepath);
    }
    catch(exception::Common& e)
    {
        e << exception::filename(params._filepath);
        throw;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("Unable to write image")
                                                   << exception::dev(boost::current_exception_diagnostic_information())
                                                   << exception::filename(params._filepath));
    }
}

/**
 *
 */
template <class View>
template <class Bits>
void ImageMagickWriterProcess<View>::writeImage(View& src, const std::string& filepath)
{
    using namespace boost::gil;

    //	if( params._premult )
    //	{
    //		typedef pixel<Bits, rgb_layout_t> OutPixelType;
    //		imagemagick_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( src ) ),
    //params._quality );
    //	}
    //	else
    //	{
    //		typedef pixel<Bits, layout<typename color_space_type<View>::type> > OutPixelType;
    //		imagemagick_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( src ) ) );
    //	}
}
}
}
}
}
