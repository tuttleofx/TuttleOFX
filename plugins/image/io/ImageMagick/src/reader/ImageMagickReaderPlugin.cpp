#include "ImageMagickReaderPlugin.hpp"
#include "ImageMagickReaderProcess.hpp"
#include "ImageMagickReaderDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

#include <cstring>

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace reader
{

namespace bfs = boost::filesystem;
using namespace boost::gil;

ImageMagickReaderPlugin::ImageMagickReaderPlugin(OfxImageEffectHandle handle)
    : ReaderPlugin(handle)
{
    MagickCoreGenesis("", MagickFalse);
}

ImageMagickReaderProcessParams ImageMagickReaderPlugin::getProcessParams(const OfxTime time)
{
    ImageMagickReaderProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);

    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageMagickReaderPlugin::render(const OFX::RenderArguments& args)
{
    ReaderPlugin::render(args);

    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent components = _clipDst->getPixelComponents();

    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            doGilRender<ImageMagickReaderProcess, false, boost::gil::rgba_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentRGB:
        {
            doGilRender<ImageMagickReaderProcess, false, boost::gil::rgb_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentAlpha:
        {
            doGilRender<ImageMagickReaderProcess, false, boost::gil::gray_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentCustom:
        case OFX::ePixelComponentNone:
        {
            BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Pixel components (" +
                                                                  mapPixelComponentEnumToString(components) +
                                                                  ") not supported by the plugin.");
        }
    }
    BOOST_THROW_EXCEPTION(exception::Unknown());
}

void ImageMagickReaderPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    ReaderPlugin::changedParam(args, paramName);
}

bool ImageMagickReaderPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    ImageInfo* imageInfo = AcquireImageInfo();

    GetImageInfo(imageInfo);
    std::string filename = getAbsoluteFilenameAt(args.time);
    strcpy(imageInfo->filename, filename.c_str());
    ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
    GetExceptionInfo(exceptionsInfo);

    Image* image = PingImage(imageInfo, exceptionsInfo);

    if(!image)
    {
        imageInfo = DestroyImageInfo(imageInfo);
        exceptionsInfo = DestroyExceptionInfo(exceptionsInfo);
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("ImageMagick: Unable to open file")
                                                                  << exception::filename(getAbsoluteFilenameAt(args.time)));
    }

    point2<ptrdiff_t> imagemagickDims(image->columns, image->rows);
    rod.x1 = 0;
    rod.x2 = imagemagickDims.x * this->_clipDst->getPixelAspectRatio();
    rod.y1 = 0;
    rod.y2 = imagemagickDims.y;

    image = DestroyImage(image);
    imageInfo = DestroyImageInfo(imageInfo);
    exceptionsInfo = DestroyExceptionInfo(exceptionsInfo);

    return true;
}

void ImageMagickReaderPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    ReaderPlugin::getClipPreferences(clipPreferences);
    const std::string filename = getAbsoluteFirstFilename();

    ImageInfo* imageInfo = AcquireImageInfo();
    GetImageInfo(imageInfo);
    strcpy(imageInfo->filename, filename.c_str());
    ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
    GetExceptionInfo(exceptionsInfo);

    //	Image* image = PingImage( imageInfo, exceptionsInfo );
    Image* image = ReadImage(imageInfo, exceptionsInfo); // necessary to obtain QuantumType

    if(!image)
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("ImageMagick: ") << exception::filename(filename));

    if(getExplicitBitDepthConversion() == eParamReaderBitDepthAuto)
    {
        clipPreferences.setClipBitDepth(*this->_clipDst, OFX::eBitDepthFloat); // by default
        unsigned long bitDepth = GetImageDepth(image, exceptionsInfo);         // if image information use it
        if(bitDepth <= 8)
        {
            clipPreferences.setClipBitDepth(*this->_clipDst, OFX::eBitDepthUByte);
        }
        else if(bitDepth <= 16)
        {
            clipPreferences.setClipBitDepth(*this->_clipDst, OFX::eBitDepthUShort);
        }
    }

    QuantumType colorType = GetQuantumType(image, exceptionsInfo);

    if(getExplicitChannelConversion() == eParamReaderChannelAuto)
    {
        switch(colorType)
        {
            case RGBQuantum:
            {
                if(OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGB))
                {
                    clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGB);
                }
                else
                {
                    clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                }
                break;
            }
            case RGBAQuantum:
            {
                clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                break;
            }
            case AlphaQuantum:
            case GrayQuantum:
            {
                clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentAlpha);
                break;
            }
            default:
            {
                // convert in RGB colorspace
                TUTTLE_LOG_WARNING("convert to RGB colorspace");
                clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                break;
            }
        }
    }

    clipPreferences.setPixelAspectRatio(*this->_clipDst, 1.0); /// @todo tuttle: retrieve info

    image = DestroyImage(image);
    imageInfo = DestroyImageInfo(imageInfo);
    exceptionsInfo = DestroyExceptionInfo(exceptionsInfo);
}
}
}
}
}
