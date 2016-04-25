#include "Jpeg2000WriterPlugin.hpp"

#include <boost/assert.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace writer
{

template <class View>
Jpeg2000WriterProcess<View>::Jpeg2000WriterProcess(Jpeg2000WriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
Jpeg2000WriterProcess<View>::~Jpeg2000WriterProcess()
{
    _writer.close();
}

template <class View>
void Jpeg2000WriterProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void Jpeg2000WriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    using namespace terry;
    BOOST_ASSERT(procWindowRoW == this->_srcPixelRod);

    _writer.setCinemaMode((OPJ_CINEMA_MODE)_params._cineProfil);
    _writer.setLossless(_params._lossless);

    View srcView = this->_srcView;

    //	TUTTLE_LOG_VAR( TUTTLE_INFO, this->_srcView.dimensions() );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, srcView.dimensions() );

    switch(_params._bitDepth)
    {
        case eTuttlePluginBitDepthAuto:
        {
            switch(_plugin._clipSrc->getPixelDepth())
            {
                case OFX::eBitDepthUByte:
                {
                    writeImage<rgb8_image_t>(srcView, 8);
                    break;
                }
                case OFX::eBitDepthUShort:
                {
                    writeImage<rgb16_image_t>(srcView, 16);
                    break;
                }
                case OFX::eBitDepthFloat:
                {
                    writeImage<rgb16_image_t>(srcView, 16);
                    // writeImage<rgb32_image_t>( srcView, 32 );
                    break;
                }
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                    BOOST_THROW_EXCEPTION(exception::Unsupported()
                                          << exception::user("Jpeg2000 Writer: bit depth not supported"));
            }
            break;
        }
        case eTuttlePluginBitDepth8:
        {
            writeImage<rgb8_image_t>(srcView, 8);
            break;
        }
        case eTuttlePluginBitDepth12:
        case eTuttlePluginBitDepth16:
        {
            writeImage<rgb16_image_t>(srcView, 16);
            break;
        }
        case eTuttlePluginBitDepth32:
        {
            writeImage<rgb32_image_t>(srcView, 32);
            break;
        }
    }
}

template <typename View>
template <typename SImg>
void Jpeg2000WriterProcess<View>::writeImage(const View& srcView, const int& bitDepth)
{
    using namespace terry;
    SImg img(srcView.dimensions());
    typename SImg::view_t vw(view(img));

    copy_and_convert_pixels(srcView, vw);

    uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);

    _writer.open(_params._filepath, srcView.width(), srcView.height(),
                 num_channels<typename SImg::view_t::value_type>::type::value, bitDepth);
    _writer.encode(pixels, bitDepth);
}
}
}
}
}
