#include "JpegReaderPlugin.hpp"
#include "JpegReaderProcess.hpp"
#include "JpegReaderDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg
{
namespace reader
{

namespace bfs = boost::filesystem;
using namespace boost::gil;

JpegReaderPlugin::JpegReaderPlugin(OfxImageEffectHandle handle)
    : ReaderPlugin(handle)
{
}

JpegReaderProcessParams JpegReaderPlugin::getProcessParams(const OfxTime time)
{
    JpegReaderProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);

    return params;
}

void JpegReaderPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    ReaderPlugin::changedParam(args, paramName);
}

bool JpegReaderPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    try
    {
        point2<ptrdiff_t> jpegDims = jpeg_read_dimensions(getAbsoluteFilenameAt(args.time));
        rod.x1 = 0;
        rod.x2 = jpegDims.x * this->_clipDst->getPixelAspectRatio();
        rod.y1 = 0;
        rod.y2 = jpegDims.y;
    }
    catch(std::exception& e)
    {
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("Jpeg: Unable to open file")
                                                                  << exception::filename(getAbsoluteFilenameAt(args.time)));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::user("Jpeg: Unable to open file")
                                                        << exception::filename(getAbsoluteFilenameAt(args.time)));
    }

    return true;
}

void JpegReaderPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    ReaderPlugin::getClipPreferences(clipPreferences);

    if(getExplicitBitDepthConversion() == eParamReaderBitDepthAuto)
    {
        clipPreferences.setClipBitDepth(*this->_clipDst, OFX::eBitDepthUByte);
    }

    if(getExplicitChannelConversion() == eParamReaderChannelAuto)
    {
        if(OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGB))
        {
            clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGB);
        }
        else
        {
            clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
        }
    }

    clipPreferences.setPixelAspectRatio(*this->_clipDst, 1.0);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void JpegReaderPlugin::render(const OFX::RenderArguments& args)
{
    ReaderPlugin::render(args);
    doGilRender<JpegReaderProcess>(*this, args);
}
}
}
}
}
