#include "DPXWriterPlugin.hpp"
#include "DPXWriterProcess.hpp"
#include "DPXWriterDefinitions.hpp"

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/filesystem.hpp>

#include <vector>

namespace tuttle
{
namespace plugin
{
namespace dpx
{
namespace writer
{

using namespace boost::gil;

DPXWriterPlugin::DPXWriterPlugin(OfxImageEffectHandle handle)
    : WriterPlugin(handle)
{
    _bitDepth = fetchChoiceParam(kTuttlePluginBitDepth);
    _descriptor = fetchChoiceParam(kTuttlePluginChannel);
    _transfer = fetchChoiceParam(kParamTransfer);
    _colorimetric = fetchChoiceParam(kParamColorimetric);
    _packed = fetchChoiceParam(kParamPacked);
    _swapEndian = fetchBooleanParam(kParamSwapEndian);
    _encoding = fetchChoiceParam(kParamEncoding);
    _orientation = fetchChoiceParam(kParamOrientation);
    _project = fetchStringParam(kParamProject);
    _copyright = fetchStringParam(kParamCopyright);
}

DPXWriterProcessParams DPXWriterPlugin::getProcessParams(const OfxTime time)
{
    DPXWriterProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);
    params._project = _project->getValue();
    params._copyright = _copyright->getValue();
    params._bitDepth = static_cast<ETuttlePluginBitDepth>(_bitDepth->getValue());

    switch(params._bitDepth)
    {
        case eTuttlePluginBitDepth8:
            params._iBitDepth = 8;
            break;
        case eTuttlePluginBitDepth10:
            params._iBitDepth = 10;
            break;
        case eTuttlePluginBitDepth12:
            params._iBitDepth = 12;
            break;
        case eTuttlePluginBitDepth16:
            params._iBitDepth = 16;
            break;
        case eTuttlePluginBitDepth32:
            params._iBitDepth = 32;
            break;
        case eTuttlePluginBitDepth64:
            params._iBitDepth = 64;
            break;
    }

    switch(_descriptor->getValue())
    {
        case 0:
            params._descriptor = ::dpx::kUserDefinedDescriptor;
            break;
        case 1:
            params._descriptor = ::dpx::kRed;
            break;
        case 2:
            params._descriptor = ::dpx::kGreen;
            break;
        case 3:
            params._descriptor = ::dpx::kBlue;
            break;
        case 4:
            params._descriptor = ::dpx::kAlpha;
            break;
        case 5:
            params._descriptor = ::dpx::kLuma;
            break;
        case 6:
            params._descriptor = ::dpx::kColorDifference;
            break;
        case 7:
            params._descriptor = ::dpx::kDepth;
            break;
        case 8:
            params._descriptor = ::dpx::kCompositeVideo;
            break;
        case 9:
            params._descriptor = ::dpx::kRGB;
            break;
        case 10:
            params._descriptor = ::dpx::kRGBA;
            break;
        case 11:
            params._descriptor = ::dpx::kABGR;
            break;
        case 12:
            params._descriptor = ::dpx::kCbYCrY;
            break;
        case 13:
            params._descriptor = ::dpx::kCbYACrYA;
            break;
        case 14:
            params._descriptor = ::dpx::kCbYCr;
            break;
        case 15:
            params._descriptor = ::dpx::kCbYCrA;
            break;
        case 16:
            params._descriptor = ::dpx::kUserDefined2Comp;
            break;
        case 17:
            params._descriptor = ::dpx::kUserDefined3Comp;
            break;
        case 18:
            params._descriptor = ::dpx::kUserDefined4Comp;
            break;
        case 19:
            params._descriptor = ::dpx::kUserDefined5Comp;
            break;
        case 20:
            params._descriptor = ::dpx::kUserDefined6Comp;
            break;
        case 21:
            params._descriptor = ::dpx::kUserDefined7Comp;
            break;
        case 22:
            params._descriptor = ::dpx::kUserDefined8Comp;
            break;
        case 23:
            params._descriptor = ::dpx::kUndefinedDescriptor;
            break;
        case 24:
            switch(_clipSrc->getPixelComponents())
            {
                case OFX::ePixelComponentAlpha:
                    params._descriptor = ::dpx::kLuma;
                    break;
                case OFX::ePixelComponentRGB:
                    params._descriptor = ::dpx::kRGB;
                    break;
                case OFX::ePixelComponentRGBA:
                    params._descriptor = ::dpx::kRGBA;
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("Dpx: unknown input channel type."));
                    break;
            }
            break;
        default:
            break;
    }
    switch(_transfer->getValue())
    {
        case 13:
            params._transfer = ::dpx::kUndefinedCharacteristic;
            break;
        default:
            params._transfer = static_cast< ::dpx::Characteristic>(_transfer->getValue());
            break;
    }
    switch(_colorimetric->getValue())
    {
        case 13:
            params._colorimetric = ::dpx::kUndefinedCharacteristic;
            break;
        default:
            params._colorimetric = static_cast< ::dpx::Characteristic>(_colorimetric->getValue());
            break;
    }

    params._packed = static_cast< ::dpx::Packing>(_packed->getValue());
    params._encoding = static_cast< ::dpx::Encoding>(_encoding->getValue());
    params._orientation = static_cast< ::dpx::Orientation>(_orientation->getValue());
    params._swapEndian = _swapEndian->getValue();

    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXWriterPlugin::render(const OFX::RenderArguments& args)
{
    WriterPlugin::render(args);
    DPXWriterProcessParams params = getProcessParams(args.time);

    OFX::EBitDepth eOfxBitDepth = _clipSrc->getPixelDepth();
    OFX::EPixelComponent components = _clipSrc->getPixelComponents();
    OfxPointI size = _clipSrc->getPixelRodSize(args.time);

    switch(params._bitDepth)
    {
        case eTuttlePluginBitDepth8:
            switch(eOfxBitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                          << exception::user("Dpx: Unable to write upper bit depth"));
                    break;
                case OFX::eBitDepthUShort:
                case OFX::eBitDepthFloat:
                    switch(components)
                    {
                        case OFX::ePixelComponentAlpha:
                            doGilRender<DPXWriterProcess, false, boost::gil::gray_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGB:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgb_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGBA:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgba_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        default:
                            BOOST_THROW_EXCEPTION(exception::InputMismatch() << exception::user("Dpx: Unknown component."));
                            break;
                    }
                    return;
                case OFX::eBitDepthUByte:
                    break;
            }
            break;
        case eTuttlePluginBitDepth10:
            switch(eOfxBitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                {
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                          << exception::user("Dpx: Unable to write upper bit depth"));
                    break;
                }
                case OFX::eBitDepthUShort: // break; // @TODO temporary use GIL, the conversion Gray => RGBA provide a
                                           // segfault after writer.Finish() function.
                case OFX::eBitDepthUByte:
                case OFX::eBitDepthFloat:
                {
                    switch(components)
                    {
                        case OFX::ePixelComponentAlpha:
                            doGilRender<DPXWriterProcess, false, boost::gil::gray_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGB:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgb_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGBA:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgba_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        default:
                            BOOST_THROW_EXCEPTION(exception::InputMismatch() << exception::user("Dpx: Unknown component."));
                            break;
                    }
                    return;
                }
            }
            break;
        case eTuttlePluginBitDepth12:
            switch(eOfxBitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                {
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                          << exception::user("Dpx: Unable to write upper bit depth"));
                    break;
                }
                case OFX::eBitDepthUByte:
                case OFX::eBitDepthFloat:
                {
                    switch(components)
                    {
                        case OFX::ePixelComponentAlpha:
                            doGilRender<DPXWriterProcess, false, boost::gil::gray_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGB:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgb_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGBA:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgba_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        default:
                            BOOST_THROW_EXCEPTION(exception::InputMismatch() << exception::user("Dpx: Unknown component."));
                            break;
                    }
                    return;
                }
                case OFX::eBitDepthUShort:
                    break;
            }
            break;
        case eTuttlePluginBitDepth16:
            switch(eOfxBitDepth)
            {
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                {
                    BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                          << exception::user("Dpx: Unable to write upper bit depth"));
                    break;
                }
                case OFX::eBitDepthUByte:
                case OFX::eBitDepthFloat:
                {
                    switch(components)
                    {
                        case OFX::ePixelComponentAlpha:
                            doGilRender<DPXWriterProcess, false, boost::gil::gray_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGB:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgb_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        case OFX::ePixelComponentRGBA:
                            doGilRender<DPXWriterProcess, false, boost::gil::rgba_layout_t>(*this, args, eOfxBitDepth);
                            break;
                        default:
                            BOOST_THROW_EXCEPTION(exception::InputMismatch() << exception::user("Dpx: Unknown component."));
                            break;
                    }
                    return;
                }
                case OFX::eBitDepthUShort:
                    break;
            }
            break;
        case eTuttlePluginBitDepth32:
        case eTuttlePluginBitDepth64:
            break;
    }

    std::string filename = getAbsoluteFilenameAt(args.time);

    ::dpx::Writer writer;
    ::dpx::DataSize dataSize = ::dpx::kByte;

    OutStream stream;

    if(!stream.Open(filename.c_str()))
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("Dpx: Unable to open output file"));
    }

    writer.SetOutStream(&stream);

    writer.Start();

    writer.SetFileInfo(filename.c_str(), 0, "TuttleOFX DPX Writer", params._project.c_str(), params._copyright.c_str(), ~0,
                       params._swapEndian);

    writer.SetImageInfo(size.x, size.y);

    writer.header.SetImageOrientation(params._orientation);

    int pixelSize = 0;
    std::string inputComponentString = "unknown";

    switch(eOfxBitDepth)
    {
        case OFX::eBitDepthCustom:
        case OFX::eBitDepthNone:
            BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                  << exception::user("Dpx: Unable to compute custom or non bit depth"));
            break;
        case OFX::eBitDepthUByte:
            dataSize = ::dpx::kByte;
            pixelSize = 1;
            break;
        case OFX::eBitDepthUShort:
            dataSize = ::dpx::kWord;
            pixelSize = 2;
            break;
        case OFX::eBitDepthFloat:
            dataSize = ::dpx::kFloat;
            pixelSize = 4;
            break;
    }
    switch(components)
    {
        case OFX::ePixelComponentAlpha:
            inputComponentString = "Gray/Alpha";
            break; // pixelSize *= 1;
        case OFX::ePixelComponentRGB:
            inputComponentString = "RGB";
            pixelSize *= 3;
            break;
        case OFX::ePixelComponentRGBA:
            inputComponentString = "RGBA";
            pixelSize *= 4;
            break;
        default:
            break;
    }

    typedef std::vector<char, OfxAllocator<char> > DataVector;
    const std::size_t rowBytesToCopy = size.x * pixelSize;
    // const std::size_t rowBytesToCopy = size.x * 4; // allocation of 4 channels, to allow conversion after on this buffer
    // (Gray=>RGBA). Better option to do this ?

    DataVector data(rowBytesToCopy * size.y);
    char* dataPtrIt = &data.front();

    boost::scoped_ptr<OFX::Image> src(_clipSrc->fetchImage(args.time));

    for(int y = size.y; y > 0; --y)
    {
        void* dataSrcPtr = src->getPixelAddress(0, y - 1);
        memcpy(dataPtrIt, dataSrcPtr, rowBytesToCopy);

        dataPtrIt += rowBytesToCopy;
    }

    switch(params._descriptor)
    {
        case ::dpx::kUserDefinedDescriptor:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("Dpx: Unable to write user defined"));
                    break;
            }
            break;
        case ::dpx::kRed:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write Red channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kGreen:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write Green channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kBlue:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write Blue channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kAlpha:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write Alpha channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kLuma:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write Luma channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kColorDifference:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat()
                                          << exception::user("Dpx: Unable to write ColorDifference channel (input is " +
                                                             inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kDepth:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write Depth channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kCompositeVideo:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kRGB:
            switch(components)
            {
                case OFX::ePixelComponentAlpha:
                {
                    convertGrayToRGB(data, size.x, size.y, pixelSize);
                    break;
                }
                case OFX::ePixelComponentRGB:
                    break;
                case OFX::ePixelComponentRGBA:
                {
                    convertRGBAToRGB(data, size.x, size.y, pixelSize);
                    break;
                }
                default:
                {
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write RGB channels (input is " + inputComponentString + ")."));
                    break;
                }
            }
            break;
        case ::dpx::kRGBA:
            switch(components)
            {
                case OFX::ePixelComponentRGBA:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write RGBA channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kABGR:
            switch(components)
            {
                case OFX::ePixelComponentRGBA:
                    convertRGBAToABGR(data, size.x, size.y, pixelSize);
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write ABGR channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kCbYCrY:
            switch(components)
            {
                case OFX::ePixelComponentRGB:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write CbYCrY channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kCbYACrYA:
            switch(components)
            {
                case OFX::ePixelComponentRGBA:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat()
                                          << exception::user("Dpx: Unable to write CbYCrYA channels (input is " +
                                                             inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kCbYCr:
            switch(components)
            {
                case OFX::ePixelComponentRGB:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write CbYCr channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kCbYCrA:
            switch(components)
            {
                case OFX::ePixelComponentRGBA:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(
                        exception::ImageFormat()
                        << exception::user("Dpx: Unable to write CbYCrA channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined2Comp:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 2 channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined3Comp:
            switch(components)
            {
                case OFX::ePixelComponentRGB:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 3 channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined4Comp:
            switch(components)
            {
                case OFX::ePixelComponentRGBA:
                    break;
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 4 channel (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined5Comp:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 5 channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined6Comp:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 6 channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined7Comp:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 7 channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUserDefined8Comp:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user(
                                              "Dpx: Unable to write 8 channels (input is " + inputComponentString + ")."));
                    break;
            }
            break;
        case ::dpx::kUndefinedDescriptor:
            switch(components)
            {
                default:
                    BOOST_THROW_EXCEPTION(exception::ImageFormat()
                                          << exception::user("Dpx: Unable to write undefined descriptor (input is " +
                                                             inputComponentString + ")."));
                    break;
            }
            break;
    }

    writer.SetElement(0, params._descriptor, params._iBitDepth, params._transfer, params._colorimetric, params._packed,
                      params._encoding);

    if(!writer.WriteHeader())
    {
        BOOST_THROW_EXCEPTION(exception::Data() << exception::user("Dpx: Unable to write data (DPX Header)"));
    }
    if(!writer.WriteElement(0, &data.front(), dataSize))
    {
        BOOST_THROW_EXCEPTION(exception::Data() << exception::user("Dpx: Unable to write data (DPX User Data)"));
    }

    if(!writer.Finish())
    {
        BOOST_THROW_EXCEPTION(exception::Data() << exception::user("Dpx: Unable to write data (DPX finish)"));
    }

    stream.Close();
}

void DPXWriterPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    WriterPlugin::changedParam(args, paramName);
}
}
}
}
}
