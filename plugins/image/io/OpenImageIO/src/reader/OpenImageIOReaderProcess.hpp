#ifndef _TUTTLE_PLUGIN_OPENIMAGEIO_READER_PROCESS_HPP_
#define _TUTTLE_PLUGIN_OPENIMAGEIO_READER_PROCESS_HPP_

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <terry/globals.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

#include <imageio.h>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace reader
{

/**
 *
 */
template <class View>
class OpenImageIOReaderProcess : public ImageGilProcessor<View>
{
    typedef OpenImageIOReaderProcess<View> This;

public:
    typedef typename terry::image_from_view<View>::type Image;
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef typename View::point_t Point;
    typedef typename View::coord_t Coord;

protected:
    OpenImageIOReaderPlugin& _plugin; ///< Rendering plugin

public:
    OpenImageIOReaderProcess(OpenImageIOReaderPlugin& instance);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);

    template <typename bitDepth, typename layout, typename fileView>
    View& readImage(View& dst, boost::scoped_ptr<OpenImageIO::ImageInput>& img, int pixelSize);

    static bool progressCallback(void* opaque_data, float portion_done)
    {
        This* this_ptr = reinterpret_cast<This*>(opaque_data);
        return this_ptr->progressUpdate(portion_done);
    }
};
}
}
}
}

#include "OpenImageIOReaderProcess.tcc"

#endif
