#include "EXRWriterProcess.hpp"
#include "EXRWriterDefinitions.hpp"
#include "EXRWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/openexr/half.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/mpl/if.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace writer
{

using namespace boost::gil;

template <class View>
EXRWriterProcess<View>::EXRWriterProcess(EXRWriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
void EXRWriterProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.time);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void EXRWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    // no tiles and no multithreading supported
    BOOST_ASSERT((procWindowRoW == this->_dstPixelRod));
    BOOST_ASSERT((this->_srcPixelRod == this->_dstPixelRod));

    View src = this->_srcView;

    try
    {
        switch(_params._fileBitDepth)
        {
            case eTuttlePluginFileBitDepth16f:
            {
                switch(_params._componentsType)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray16h_pixel_t>(src, _params._filepath, Imf::HALF);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb16h_pixel_t>(src, _params._filepath, Imf::HALF);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba16h_pixel_t>(src, _params._filepath, Imf::HALF);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("Exr Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                    {
                        writeImage<gray16h_pixel_t>(src, _params._filepath, Imf::HALF);
                        break;
                    }
                    case eTuttlePluginComponentsRGB:
                    {
                        writeImage<rgb16h_pixel_t>(src, _params._filepath, Imf::HALF);
                        break;
                    }
                    case eTuttlePluginComponentsRGBA:
                    {
                        writeImage<rgba16h_pixel_t>(src, _params._filepath, Imf::HALF);
                        break;
                    }
                    default:
                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                              << exception::user("Exr Writer: components not supported"));
                }
                break;
            }
            case eTuttlePluginFileBitDepth32f:
            {
                switch(_params._componentsType)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("Exr Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                    {
                        writeImage<gray32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
                        break;
                    }
                    case eTuttlePluginComponentsRGB:
                    {
                        writeImage<rgb32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
                        break;
                    }
                    case eTuttlePluginComponentsRGBA:
                    {
                        writeImage<rgba32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
                        break;
                    }
                    default:
                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                              << exception::user("Exr Writer: components not supported"));
                }
                break;
            }
            case eTuttlePluginFileBitDepth32:
            {
                switch(_params._componentsType)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray32_pixel_t>(src, _params._filepath, Imf::HALF);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb32_pixel_t>(src, _params._filepath, Imf::HALF);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba32_pixel_t>(src, _params._filepath, Imf::HALF);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("Exr Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                    {
                        writeImage<gray32_pixel_t>(src, _params._filepath, Imf::UINT);
                        break;
                    }
                    case eTuttlePluginComponentsRGB:
                    {
                        writeImage<rgb32_pixel_t>(src, _params._filepath, Imf::UINT);
                        break;
                    }
                    case eTuttlePluginComponentsRGBA:
                    {
                        writeImage<rgba32_pixel_t>(src, _params._filepath, Imf::UINT);
                        break;
                    }
                    default:
                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                              << exception::user("Exr Writer: components not supported"));
                }
                break;
            }
            case eTuttlePluginFileBitDepthNone:
            {
                BOOST_THROW_EXCEPTION(exception::Bug());
            }
        }
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

template <int my_nb_channels>
struct FillFrameSwitch
{
    template <typename View>
    static void fillFrameBuffer(Imf::FrameBuffer& frameBuffer, const View& dvw, const Imf::PixelType pixType,
                                const std::size_t bitsTypeSize, const std::size_t rowBytes);
};

template <>
struct FillFrameSwitch<1>
{
    template <typename View>
    static void fillFrameBuffer(Imf::FrameBuffer& frameBuffer, const View& dvw, const Imf::PixelType pixType,
                                const std::size_t bitsTypeSize, const std::size_t rowBytes)
    {
        // Gray
        char* pixelsY = (char*)boost::gil::interleaved_view_get_raw_data(dvw);
        frameBuffer.insert("Y", Imf::Slice(pixType, pixelsY, bitsTypeSize, rowBytes));
    }
};

template <>
struct FillFrameSwitch<3>
{
    template <typename View>
    static void fillFrameBuffer(Imf::FrameBuffer& frameBuffer, const View& dvw, Imf::PixelType pixType,
                                const std::size_t bitsTypeSize, const std::size_t rowBytes)
    {
        // RGB
        char* pixelsR = (char*)boost::gil::planar_view_get_raw_data(dvw, 0);
        char* pixelsG = (char*)boost::gil::planar_view_get_raw_data(dvw, 1);
        char* pixelsB = (char*)boost::gil::planar_view_get_raw_data(dvw, 2);
        frameBuffer.insert("R", Imf::Slice(pixType, pixelsR, bitsTypeSize, rowBytes));
        frameBuffer.insert("G", Imf::Slice(pixType, pixelsG, bitsTypeSize, rowBytes));
        frameBuffer.insert("B", Imf::Slice(pixType, pixelsB, bitsTypeSize, rowBytes));
    }
};

template <>
struct FillFrameSwitch<4>
{
    template <typename View>
    static void fillFrameBuffer(Imf::FrameBuffer& frameBuffer, const View& dvw, const Imf::PixelType pixType,
                                const std::size_t bitsTypeSize, const std::size_t rowBytes)
    {
        // RGBA
        char* pixelsR = (char*)boost::gil::planar_view_get_raw_data(dvw, 0);
        char* pixelsG = (char*)boost::gil::planar_view_get_raw_data(dvw, 1);
        char* pixelsB = (char*)boost::gil::planar_view_get_raw_data(dvw, 2);
        char* pixelsA = (char*)boost::gil::planar_view_get_raw_data(dvw, 3);
        frameBuffer.insert("R", Imf::Slice(pixType, pixelsR, bitsTypeSize, rowBytes));
        frameBuffer.insert("G", Imf::Slice(pixType, pixelsG, bitsTypeSize, rowBytes));
        frameBuffer.insert("B", Imf::Slice(pixType, pixelsB, bitsTypeSize, rowBytes));
        frameBuffer.insert("A", Imf::Slice(pixType, pixelsA, bitsTypeSize, rowBytes));
    }
};

template <class View>
template <class WPixel>
void EXRWriterProcess<View>::writeImage(View& src, std::string& filepath, Imf::PixelType pixType)
{
    // Can't use planar images if there is only one channel.
    typedef typename boost::mpl::if_c<(boost::gil::num_channels<WPixel>::value == 1), boost::mpl::false_,
                                      boost::mpl::true_>::type is_planar_t;
    typedef boost::gil::image<WPixel, is_planar_t::value> image_t;
    typedef typename image_t::view_t view_t;

    image_t img(src.width(), src.height());
    view_t dvw(view(img));
    boost::gil::copy_and_convert_pixels(src, dvw);
    Imf::Header header(src.width(), src.height(), (float)_plugin._clipSrc->getPixelAspectRatio());

    switch(_params._compression)
    {
        case eParamCompression_RLE:
            header.compression() = Imf::RLE_COMPRESSION;
            break;
        case eParamCompression_ZIPS:
            header.compression() = Imf::ZIPS_COMPRESSION;
            break;
        case eParamCompression_ZIP:
            header.compression() = Imf::ZIP_COMPRESSION;
            break;
        case eParamCompression_PIZ:
            header.compression() = Imf::PIZ_COMPRESSION;
            break;
        case eParamCompression_PXR24:
            header.compression() = Imf::PXR24_COMPRESSION;
            break;
        case eParamCompression_B44:
            header.compression() = Imf::B44_COMPRESSION;
            break;
        case eParamCompression_B44A:
            header.compression() = Imf::B44A_COMPRESSION;
            break;
        case eParamCompression_None:
            header.compression() = Imf::NO_COMPRESSION;
            break;
        default:
            BOOST_THROW_EXCEPTION(exception::Bug()
                                  << exception::dev("EXRWriter: unrecognized compression parameter value."));
            break;
    }

    // TODO
    // RoI
    //	header.dataWindow()
    // RoD
    //	header.displayWindow()
    //	header.lineOrder() = Imf::INCREASING_Y;

    std::size_t bitsTypeSize = 0;
    switch(pixType)
    {
        case Imf::HALF:
            bitsTypeSize = sizeof(half);
            break;
        case Imf::FLOAT:
            bitsTypeSize = sizeof(float);
            break;
        case Imf::UINT:
            bitsTypeSize = sizeof(boost::uint32_t);
            break;
        default:
            break;
    }

    switch(dvw.num_channels())
    {
        case 1: // Gray
            header.channels().insert("Y", Imf::Channel(pixType));
            break;
        case 3:
        {
            header.channels().insert("R", Imf::Channel(pixType));
            header.channels().insert("G", Imf::Channel(pixType));
            header.channels().insert("B", Imf::Channel(pixType));
            break;
        }
        case 4:
        {
            header.channels().insert("R", Imf::Channel(pixType));
            header.channels().insert("G", Imf::Channel(pixType));
            header.channels().insert("B", Imf::Channel(pixType));
            header.channels().insert("A", Imf::Channel(pixType));
            break;
        }
        default:
            BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("ExrWriter: incompatible image type"));
    }

    Imf::OutputFile file(filepath.c_str(), header);
    Imf::FrameBuffer frameBuffer;

    const std::size_t rowBytes = bitsTypeSize * src.width();

    static const std::size_t view_nb_channels = boost::gil::num_channels<view_t>::value;
    FillFrameSwitch<view_nb_channels>::template fillFrameBuffer<view_t>(frameBuffer, dvw, pixType, bitsTypeSize, rowBytes);

    file.setFrameBuffer(frameBuffer);
    // Finalize output
    file.writePixels(src.height());
}
}
}
}
}
