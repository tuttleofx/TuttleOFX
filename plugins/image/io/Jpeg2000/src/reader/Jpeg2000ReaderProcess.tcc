#include "Jpeg2000ReaderPlugin.hpp"
#include <terry/typedefs.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace reader
{

template <class View>
Jpeg2000ReaderProcess<View>::Jpeg2000ReaderProcess(Jpeg2000ReaderPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
Jpeg2000ReaderProcess<View>::~Jpeg2000ReaderProcess()
{
    _plugin._reader.close();
}

template <class View>
void Jpeg2000ReaderProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void Jpeg2000ReaderProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;

    View dstView = this->_dstView;

    switch(_plugin._reader.components())
    {
        case 1:
        {
            switchLayoutCopy<gray_layout_t>(dstView);
            break;
        }
        case 3:
        {
            switchLayoutCopy<rgb_layout_t>(dstView);
            break;
        }
        case 4:
        {
            switchLayoutCopy<rgba_layout_t>(dstView);
            break;
        }
        default:
        {
            break;
        }
    }
}

template <class View>
template <class Layout>
void Jpeg2000ReaderProcess<View>::switchLayoutCopy(const View& dstView)
{
    using namespace boost::gil;

    switch(_plugin._reader.precision())
    {
        case 8:
        {
            typedef pixel<bits8, Layout> PixelT;
            switchPrecisionCopy<PixelT>(dstView);
            break;
        }
        case 12:
        {
            typedef pixel<bits12, Layout> PixelT;
            switchPrecisionCopy<PixelT>(dstView);
            break;
        }
        case 16:
        {
            typedef pixel<bits16, Layout> PixelT;
            switchPrecisionCopy<PixelT>(dstView);
            break;
        }
        case 32:
        {
            typedef pixel<bits32, Layout> PixelT;
            switchPrecisionCopy<PixelT>(dstView);
            break;
        }
        default:
            BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("Unhandled image precision!"));
            break;
    }
}

template <class View>
template <class WorkingPixel>
void Jpeg2000ReaderProcess<View>::switchPrecisionCopy(const View& dstView)
{
    using namespace boost::gil;
    tuttle::io::J2KReader& reader = _plugin._reader;
    int w = reader.width();
    int h = reader.height();

    unsigned int* data[num_channels<WorkingPixel>::type::value];
    for(int i = 0; i < num_channels<WorkingPixel>::type::value; ++i)
    {
        data[i] = (unsigned int*)reader.compData(i);
    }
    WorkingPixel pix;

    for(typename View::y_coord_t y = 0; y < h; ++y)
    {
        typename View::x_iterator it = dstView.row_begin(y);

        for(typename View::x_coord_t x = 0; x < w; ++x)
        {
            for(int i = 0; i < num_channels<WorkingPixel>::type::value; ++i)
            {
                pix[i] = *(data[i]);
                ++data[i];
            }
            color_convert(pix, *it);
            ++it;
        }
    }
}
}
}
}
}
