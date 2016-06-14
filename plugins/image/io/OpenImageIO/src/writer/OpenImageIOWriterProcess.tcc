#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/openexr/half.hpp>

#include <tuttle/plugin/exceptions.hpp>

#include <imageio.h>
#include <filesystem.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/at.hpp>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace writer
{

template <class View>
OpenImageIOWriterProcess<View>::OpenImageIOWriterProcess(OpenImageIOWriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

/**
 * Deduce the best bitdepth when it hasn't been set by the user
 */
template <class View>
ETuttlePluginBitDepth OpenImageIOWriterProcess<View>::getDefaultBitDepth(const std::string& filepath,
                                                                         const ETuttlePluginBitDepth& bitDepth)
{
    if(bitDepth == eTuttlePluginBitDepthAuto)
    {
        std::string format = OpenImageIO::Filesystem::extension(filepath);
        if(format.find("exr") != std::string::npos || format.find("hdr") != std::string::npos ||
           format.find("rgbe") != std::string::npos)
        {
            return eTuttlePluginBitDepth32f;
        }
        else if(format.find("jpg") != std::string::npos || format.find("jpeg") != std::string::npos ||
                format.find("bmp") != std::string::npos || format.find("dds") != std::string::npos ||
                format.find("ico") != std::string::npos || format.find("jfi") != std::string::npos ||
                format.find("pgm") != std::string::npos || format.find("pnm") != std::string::npos ||
                format.find("ppm") != std::string::npos || format.find("pbm") != std::string::npos ||
                format.find("pic") != std::string::npos)
        {
            return eTuttlePluginBitDepth8;
        }
        else
        {
            // bmp, cin, dpx, fits, j2k, j2c, jp2, jpe, png, sgi, tga, tif, tiff, tpic, webp
            return eTuttlePluginBitDepth16;
        }
    }
    return bitDepth;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template <class View>
void OpenImageIOWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    BOOST_ASSERT(procWindowRoW == this->_srcPixelRod);
    using namespace boost::gil;
    using namespace terry;
    params = _plugin.getProcessParams(this->_renderArgs.time);

    ETuttlePluginBitDepth finalBitDepth = getDefaultBitDepth(params._filepath, params._bitDepth);

    try
    {
        switch(finalBitDepth)
        {
            case eTuttlePluginBitDepthAuto:
            {
                boost::filesystem::path p(params._filepath);
                std::string ext = p.extension().string();
                if(ext == ".cin")
                {
                    switch(_plugin._clipSrc->getPixelComponents())
                    {
                        case OFX::ePixelComponentAlpha:
                            writeImage<gray16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth10);
                            break;
                        case OFX::ePixelComponentRGB:
                            writeImage<rgb16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth10);
                            break;
                        case OFX::ePixelComponentRGBA:
                            writeImage<rgba16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth10);
                            break;
                        default:
                            BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                  << exception::user("OIIO Writer: components not supported"));
                            break;
                    }
                    break;
                }
                if(ext == ".dpx")
                {
                    writeImage<rgb16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth10);
                    break;
                }
                if(ext == ".hdr")
                {
                    writeImage<rgb32f_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth32f);
                    break;
                }
                if(ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".tga")
                {
                    writeImage<rgb8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                    break;
                }
                if(ext == ".j2k" || ext == ".jp2" || ext == ".j2c")
                {
                    writeImage<rgb16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth12);
                    break;
                }
                if(ext == ".tif" || ext == ".tiff")
                {
                    switch(_plugin._clipSrc->getPixelComponents())
                    {
                        case OFX::ePixelComponentAlpha:
                            writeImage<gray16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                            break;
                        case OFX::ePixelComponentRGB:
                            writeImage<rgb16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                            break;
                        case OFX::ePixelComponentRGBA:
                            writeImage<rgba16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                            break;
                        default:
                            BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                  << exception::user("OIIO Writer: components not supported"));
                            break;
                    }
                    break;
                }

                switch(_plugin._clipSrc->getPixelDepth())
                {
                    case OFX::eBitDepthUByte:
                    {
                        switch(params._components)
                        {
                            case eTuttlePluginComponentsAuto:
                            {
                                switch(_plugin._clipSrc->getPixelComponents())
                                {
                                    case OFX::ePixelComponentAlpha:
                                        writeImage<gray8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                                        break;
                                    case OFX::ePixelComponentRGB:
                                        writeImage<rgb8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                                        break;
                                    case OFX::ePixelComponentRGBA:
                                        writeImage<rgba8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                                        break;
                                    default:
                                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                              << exception::user("OIIO Writer: components not supported"));
                                        break;
                                }
                                break;
                            }
                            case eTuttlePluginComponentsGray:
                                writeImage<gray8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                                break;
                            case eTuttlePluginComponentsRGBA:
                                writeImage<rgba8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                                break;
                            case eTuttlePluginComponentsRGB:
                                writeImage<rgb8_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth8);
                                break;
                        }
                        break;
                    }
                    case OFX::eBitDepthUShort:
                    {
                        switch(params._components)
                        {
                            case eTuttlePluginComponentsAuto:
                            {
                                switch(_plugin._clipSrc->getPixelComponents())
                                {
                                    case OFX::ePixelComponentAlpha:
                                        writeImage<gray16_image_t>(this->_srcView, params._filepath,
                                                                   eTuttlePluginBitDepth16);
                                        break;
                                    case OFX::ePixelComponentRGB:
                                        writeImage<rgb16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                                        break;
                                    case OFX::ePixelComponentRGBA:
                                        writeImage<rgba16_image_t>(this->_srcView, params._filepath,
                                                                   eTuttlePluginBitDepth16);
                                        break;
                                    default:
                                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                              << exception::user("OIIO Writer: components not supported"));
                                        break;
                                }
                                break;
                            }
                            case eTuttlePluginComponentsGray:
                                writeImage<gray16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                                break;
                            case eTuttlePluginComponentsRGBA:
                                writeImage<rgba16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                                break;
                            case eTuttlePluginComponentsRGB:
                                writeImage<rgb16_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth16);
                                break;
                        }
                        break;
                        break;
                    }
                    case OFX::eBitDepthFloat:
                    {
                        switch(params._components)
                        {
                            case eTuttlePluginComponentsAuto:
                            {
                                switch(_plugin._clipSrc->getPixelComponents())
                                {
                                    case OFX::ePixelComponentAlpha:
                                        writeImage<gray32f_image_t>(this->_srcView, params._filepath,
                                                                    eTuttlePluginBitDepth32f);
                                        break;
                                    case OFX::ePixelComponentRGB:
                                        writeImage<rgb32f_image_t>(this->_srcView, params._filepath,
                                                                   eTuttlePluginBitDepth32f);
                                        break;
                                    case OFX::ePixelComponentRGBA:
                                        writeImage<rgba32f_image_t>(this->_srcView, params._filepath,
                                                                    eTuttlePluginBitDepth32f);
                                        break;
                                    default:
                                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                              << exception::user("OIIO Writer: components not supported"));
                                        break;
                                }
                                break;
                            }
                            case eTuttlePluginComponentsGray:
                                writeImage<gray32f_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth32f);
                                break;
                            case eTuttlePluginComponentsRGBA:
                                writeImage<rgba32f_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth32f);
                                break;
                            case eTuttlePluginComponentsRGB:
                                writeImage<rgb32f_image_t>(this->_srcView, params._filepath, eTuttlePluginBitDepth32f);
                                break;
                        }
                        break;
                        break;
                    }
                    case OFX::eBitDepthCustom:
                    case OFX::eBitDepthNone:
                        BOOST_THROW_EXCEPTION(exception::Unsupported()
                                              << exception::user("OIIO Writer: bit depth not supported"));
                }
                break;
            }
            case eTuttlePluginBitDepth8:
            {
                switch(params._components)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray8_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb8_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba8_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("OIIO Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                        writeImage<gray8_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGBA:
                        writeImage<rgba8_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGB:
                        writeImage<rgb8_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                }
                break;
            }
            case eTuttlePluginBitDepth10:
            case eTuttlePluginBitDepth12:
            case eTuttlePluginBitDepth16:
            {
                switch(params._components)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray16_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb16_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba16_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("OIIO Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                        writeImage<gray16_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGBA:
                        writeImage<rgba16_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGB:
                        writeImage<rgb16_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                }
                break;
            }
            case eTuttlePluginBitDepth16f:
            {
                switch(params._components)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray16h_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb16h_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba16h_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("OIIO Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                        writeImage<gray16h_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGBA:
                        writeImage<rgba16h_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGB:
                        writeImage<rgb16h_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                }
                break;
            }
            case eTuttlePluginBitDepth32:
            {
                switch(params._components)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray32_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb32_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba32_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("OIIO Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                        writeImage<gray32_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGBA:
                        writeImage<rgba32_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGB:
                        writeImage<rgb32_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                }
                break;
            }
            case eTuttlePluginBitDepth32f:
            {
                switch(params._components)
                {
                    case eTuttlePluginComponentsAuto:
                    {
                        switch(_plugin._clipSrc->getPixelComponents())
                        {
                            case OFX::ePixelComponentAlpha:
                                writeImage<gray32f_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGB:
                                writeImage<rgb32f_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            case OFX::ePixelComponentRGBA:
                                writeImage<rgba32f_image_t>(this->_srcView, params._filepath, params._bitDepth);
                                break;
                            default:
                                BOOST_THROW_EXCEPTION(exception::Unsupported()
                                                      << exception::user("OIIO Writer: components not supported"));
                                break;
                        }
                        break;
                    }
                    case eTuttlePluginComponentsGray:
                        writeImage<gray32f_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGBA:
                        writeImage<rgba32f_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                    case eTuttlePluginComponentsRGB:
                        writeImage<rgb32f_image_t>(this->_srcView, params._filepath, params._bitDepth);
                        break;
                }
                break;
            }
        }
    }
    catch(exception::Common& e)
    {
        e << exception::filename(params._filepath);
        throw;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("OIIO Writer: Unable to write image")
                                                   << exception::dev(boost::current_exception_diagnostic_information())
                                                   << exception::filename(params._filepath));
    }
}

/**
 *
 */
template <class View>
template <class WImage>
void OpenImageIOWriterProcess<View>::writeImage(View& src, const std::string& filepath,
                                                const ETuttlePluginBitDepth& bitDepth)
{
    using namespace boost;
    using namespace OpenImageIO;

    boost::scoped_ptr<ImageOutput> out(ImageOutput::create(filepath));
    if(out.get() == NULL)
    {
        BOOST_THROW_EXCEPTION(exception::Bug() << exception::user("OIIO Writer: error from writer while opening file."));
    }
    WImage img(src.width(), src.height());

    typename WImage::view_t vw(view(img));
    copy_and_convert_pixels(src, vw);

    OpenImageIO::TypeDesc oiioBitDepth;
    size_t sizeOfChannel = 0;
    int bitsPerSample = 0;

    ETuttlePluginBitDepth finalBitDepth = getDefaultBitDepth(filepath, bitDepth);

    switch(finalBitDepth)
    {
        case eTuttlePluginBitDepthAuto:
            BOOST_THROW_EXCEPTION(exception::Bug() << exception::user("OIIO Writer: undefined output bit depth."));
        case eTuttlePluginBitDepth8:
            oiioBitDepth = TypeDesc::UINT8;
            bitsPerSample = 8;
            sizeOfChannel = 1;
            break;
        case eTuttlePluginBitDepth10:
            oiioBitDepth = TypeDesc::UINT16;
            bitsPerSample = 10;
            sizeOfChannel = 2;
            break;
        case eTuttlePluginBitDepth12:
            oiioBitDepth = TypeDesc::UINT16;
            bitsPerSample = 12;
            sizeOfChannel = 2;
            break;
        case eTuttlePluginBitDepth16:
            oiioBitDepth = TypeDesc::UINT16;
            bitsPerSample = 16;
            sizeOfChannel = 2;
            break;
        case eTuttlePluginBitDepth16f:
            oiioBitDepth = TypeDesc::HALF;
            bitsPerSample = 16;
            sizeOfChannel = 2;
            break;
        case eTuttlePluginBitDepth32:
            oiioBitDepth = TypeDesc::UINT;
            bitsPerSample = 32;
            sizeOfChannel = 4;
            break;
        case eTuttlePluginBitDepth32f:
            oiioBitDepth = TypeDesc::FLOAT;
            bitsPerSample = 32;
            sizeOfChannel = 4;
            break;
    }

    ImageSpec spec(src.width(), src.height(), gil::num_channels<WImage>::value, oiioBitDepth);

    spec.attribute("oiio:BitsPerSample", bitsPerSample);
    spec.attribute("oiio:UnassociatedAlpha", params._premultiply);
    spec.attribute("CompressionQuality", params._quality);
    spec.attribute("Orientation", params._orientation);

    // controlling chroma-subsampling of jpeg
    // Other formats don't have this attribute and ignore it.
    switch(params._subsampling)
    {
	case eETuttlePluginSubsampling420:
	    spec.attribute("jpeg:subsampling", "4:2:0");
	    break;
	case eETuttlePluginSubsampling422:
	    spec.attribute("jpeg:subsampling", "4:2:2");
	    break;
	case eETuttlePluginSubsampling411:
	    spec.attribute("jpeg:subsampling", "4:1:1");
	    break;
	case eETuttlePluginSubsampling444:
	    spec.attribute("jpeg:subsampling", "4:4:4");
	    break;
    }

    // write par, xdensity and ydensity to the output
    // Some formats (ie. TIF, JPEG...) make the difference between those attributes.
    // Some others (ie. DPX...) don't have density attributes and ignore them.
    const float par = _plugin._clipSrc->getPixelAspectRatio();
    spec.attribute("PixelAspectRatio", par);
    spec.attribute("XResolution", par);
    spec.attribute("YResolution", 1);

    if(!out->open(filepath, spec))
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("OIIO Writer: " + out->geterror())
                                                   << exception::filename(params._filepath));
    }

    const stride_t xstride = gil::is_planar<WImage>::value ? sizeOfChannel : vw.num_channels() * sizeOfChannel;
    const stride_t ystride = vw.pixels().row_size();
    const stride_t zstride = ystride * vw.height();

    typedef typename boost::gil::channel_type<WImage>::type channel_t;

    out->write_image(oiioBitDepth,
                     &((*vw.begin())[0]), // get the adress of the first channel value from the first pixel
                     xstride, ystride, zstride, &progressCallback, this);

    out->close();
}
}
}
}
}
