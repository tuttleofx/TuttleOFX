#include "DPXWriterDefinitions.hpp"
#include "DPXWriterPlugin.hpp"

#include "DPXWriterAlgorithm.hpp"

#include <terry/typedefs.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/exception/errinfo_file_name.hpp>
#include <boost/assert.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/integer.hpp> // for boost::uint_t
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle
{
namespace plugin
{
namespace dpx
{
namespace writer
{

using namespace boost::gil;

template <class View>
DPXWriterProcess<View>::DPXWriterProcess(DPXWriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
{
    this->setNoMultiThreading();
}

template <class View>
void DPXWriterProcess<View>::setup(const OFX::RenderArguments& args)
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
void DPXWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View src = subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    ::dpx::Writer writer;
    ::dpx::DataSize dataSize = ::dpx::kByte;

    OutStream stream;

    if(!stream.Open(_params._filepath.c_str()))
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("Dpx: Unable to open output file"));
    }

    writer.SetOutStream(&stream);
    writer.Start();
    writer.SetFileInfo(_params._filepath.c_str(), 0, "TuttleOFX DPX Writer", _params._project.c_str(),
                       _params._copyright.c_str(), ~0, _params._swapEndian);
    writer.SetImageInfo(procWindowSize.x, procWindowSize.y);

#if(TUTTLE_EXPERIMENTAL)
    writer.header.SetImageOrientation(_params._orientation);
#endif

    switch(_params._bitDepth)
    {
        case eTuttlePluginBitDepth8:
            dataSize = ::dpx::kByte;
            break;
        case eTuttlePluginBitDepth10:
        case eTuttlePluginBitDepth12:
        case eTuttlePluginBitDepth16:
            dataSize = ::dpx::kWord;
            break;
        case eTuttlePluginBitDepth32:
        case eTuttlePluginBitDepth64:
            dataSize = ::dpx::kFloat;
            break;
    }

    writer.SetElement(0, _params._descriptor, _params._iBitDepth, _params._transfer, _params._colorimetric, _params._packed,
                      _params._encoding);

    if(!writer.WriteHeader())
    {
        BOOST_THROW_EXCEPTION(exception::Data() << exception::user("Dpx: Unable to write data (DPX Header)"));
    }

    // TUTTLE_LOG_VAR( TUTTLE_INFO, _params._descriptor);
    switch(_params._descriptor)
    {
        case ::dpx::kUserDefinedDescriptor:
            BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("Dpx: Unable to write user defined"));
            break;
        case ::dpx::kRed:
            break;
        case ::dpx::kGreen:
            break;
        case ::dpx::kBlue:
            break;
        case ::dpx::kAlpha:
            BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("Dpx: Unable to write user defined"));
            break;
        case ::dpx::kLuma:
            switch(_params._bitDepth)
            {
                case eTuttlePluginBitDepth8:
                    writeImage<gray8_pixel_t>(writer, src, dataSize, 1);
                    break;
                case eTuttlePluginBitDepth10:
                case eTuttlePluginBitDepth12:
                case eTuttlePluginBitDepth16:
                    writeImage<gray16_pixel_t>(writer, src, dataSize, 2);
                    break;
                case eTuttlePluginBitDepth32:
                case eTuttlePluginBitDepth64:
                    writeImage<gray32f_pixel_t>(writer, src, dataSize, 4);
                    break;
            }
            break;
            break;
        case ::dpx::kColorDifference:
            break;
        case ::dpx::kDepth:
            break;
        case ::dpx::kCompositeVideo:
            break;
        case ::dpx::kRGB:
            switch(_params._bitDepth)
            {
                case eTuttlePluginBitDepth8:
                    writeImage<rgb8_pixel_t>(writer, src, dataSize, 3);
                    break;
                case eTuttlePluginBitDepth10:
                case eTuttlePluginBitDepth12:
                case eTuttlePluginBitDepth16:
                    writeImage<rgb16_pixel_t>(writer, src, dataSize, 6);
                    break;
                case eTuttlePluginBitDepth32:
                case eTuttlePluginBitDepth64:
                    writeImage<rgb32f_pixel_t>(writer, src, dataSize, 12);
                    break;
            }
            break;
        case ::dpx::kRGBA:
            switch(_params._bitDepth)
            {
                case eTuttlePluginBitDepth8:
                    writeImage<rgba8_pixel_t>(writer, src, dataSize, 4);
                    break;
                case eTuttlePluginBitDepth10:
                case eTuttlePluginBitDepth12:
                case eTuttlePluginBitDepth16:
                    writeImage<rgba16_pixel_t>(writer, src, dataSize, 8);
                    break;
                case eTuttlePluginBitDepth32:
                case eTuttlePluginBitDepth64:
                    writeImage<rgba32f_pixel_t>(writer, src, dataSize, 16);
                    break;
            }
            break;
        case ::dpx::kABGR:
            switch(_params._bitDepth)
            {
                case eTuttlePluginBitDepth8:
                    writeImage<abgr8_pixel_t>(writer, src, dataSize, 4);
                    break;
                case eTuttlePluginBitDepth10:
                case eTuttlePluginBitDepth12:
                case eTuttlePluginBitDepth16:
                    writeImage<abgr16_pixel_t>(writer, src, dataSize, 8);
                    break;
                case eTuttlePluginBitDepth32:
                case eTuttlePluginBitDepth64:
                    writeImage<abgr32f_pixel_t>(writer, src, dataSize, 16);
                    break;
            }
            break;
        case ::dpx::kCbYCrY:
            break;
        case ::dpx::kCbYACrYA:
            break;
        case ::dpx::kCbYCr:
            break;
        case ::dpx::kCbYCrA:
            break;
        case ::dpx::kUserDefined2Comp:
            break;
        case ::dpx::kUserDefined3Comp:
            break;
        case ::dpx::kUserDefined4Comp:
            break;
        case ::dpx::kUserDefined5Comp:
            break;
        case ::dpx::kUserDefined6Comp:
            break;
        case ::dpx::kUserDefined7Comp:
            break;
        case ::dpx::kUserDefined8Comp:
            break;
        case ::dpx::kUndefinedDescriptor:
            break;
    }

    if(!writer.Finish())
    {
        BOOST_THROW_EXCEPTION(exception::Data() << exception::user("Dpx: Unable to write data (DPX finish)"));
    }

    stream.Close();
}

template <class View>
template <class WPixel>
void DPXWriterProcess<View>::writeImage(::dpx::Writer& writer, View& src, ::dpx::DataSize& dataSize, size_t pixelSize)
{
    using namespace terry;
    typedef image<WPixel, false> image_t; // interleaved image
    typedef typename image_t::view_t view_t;
    image_t img(src.width(), src.height());
    view_t dvw(view(img));
    copy_and_convert_pixels(src, dvw);

    typedef std::vector<char, OfxAllocator<char> > DataVector;
    const size_t rowBytesToCopy = src.width() * pixelSize;

    DataVector data(rowBytesToCopy * src.height());
    char* dataPtrIt = &data.front();

    for(int y = 0; y < src.height(); y++)
    {
        void* dataSrcPtr = reinterpret_cast<void*>(&dvw(0, y)[0]);
        memcpy(dataPtrIt, dataSrcPtr, rowBytesToCopy);

        dataPtrIt += rowBytesToCopy;
    }

    if(!writer.WriteElement(0, &data.front(), dataSize))
    {
        BOOST_THROW_EXCEPTION(exception::Data() << exception::user("Dpx: Unable to write data (DPX User Data)"));
    }
}
}
}
}
}
