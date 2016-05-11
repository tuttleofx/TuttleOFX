#include "OpenImageIOReaderPlugin.hpp"
#include "OpenImageIOReaderProcess.hpp"
#include "OpenImageIOReaderDefinitions.hpp"

#include <imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace reader
{

namespace bfs = boost::filesystem;
using namespace boost::gil;

OpenImageIOReaderPlugin::OpenImageIOReaderPlugin(OfxImageEffectHandle handle)
    : ReaderPlugin(handle)
{
}

OpenImageIOReaderProcessParams OpenImageIOReaderPlugin::getProcessParams(const OfxTime time)
{
    OpenImageIOReaderProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);
    return params;
}

void OpenImageIOReaderPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    ReaderPlugin::changedParam(args, paramName);
}

bool OpenImageIOReaderPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    const std::string filename(getAbsoluteFilenameAt(args.time));

    if(!bfs::exists(filename))
    {
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("OpenImageIO: Unable to open file")
                                                                  << exception::filename(filename));
    }

    boost::scoped_ptr<OpenImageIO::ImageInput> in(OpenImageIO::ImageInput::create(filename));
    if(!in)
    {
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("OpenImageIO: Unable to open file")
                                                << exception::filename(filename));
    }
    OpenImageIO::ImageSpec spec;

    if(!in->open(filename, spec))
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("OIIO Reader: " + in->geterror())
                                                   << exception::filename(filename));
    }

    rod.x1 = 0;
    rod.x2 = spec.width * this->_clipDst->getPixelAspectRatio();
    rod.y1 = 0;
    rod.y2 = spec.height;

    in->close();
    return true;
}

void OpenImageIOReaderPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    ReaderPlugin::getClipPreferences(clipPreferences);

    const std::string filename(getAbsoluteFirstFilename());

    if(!bfs::exists(filename))
    {
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("OpenImageIO: Unable to open file")
                                                                  << exception::filename(filename));
    }

    // if no filename
    if(filename.size() == 0)
    {
        clipPreferences.setClipBitDepth(*this->_clipDst, OFX::eBitDepthFloat);
        clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
        clipPreferences.setPixelAspectRatio(*this->_clipDst, 1.0);
        return;
    }

    OpenImageIO::ImageInput* in = OpenImageIO::ImageInput::create(filename);

    if(!in)
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("OIIO Reader: " + in->geterror())
                                                   << exception::filename(filename));
    }

    OpenImageIO::ImageSpec spec;
    if(!in->open(filename, spec))
    {
        BOOST_THROW_EXCEPTION(exception::Unknown() << exception::user("OIIO Reader: " + in->geterror())
                                                   << exception::filename(filename));
    }

    if(getExplicitBitDepthConversion() == eParamReaderBitDepthAuto)
    {
        OFX::EBitDepth bd = OFX::eBitDepthNone;
        switch(spec.format.basetype)
        {
            //			case TypeDesc::UCHAR:
            case OpenImageIO::TypeDesc::UINT8:
            //			case TypeDesc::CHAR:
            case OpenImageIO::TypeDesc::INT8:
                bd = OFX::eBitDepthUByte;
                break;
            case OpenImageIO::TypeDesc::HALF:
            //			case TypeDesc::USHORT:
            case OpenImageIO::TypeDesc::UINT16:
            //			case TypeDesc::SHORT:
            case OpenImageIO::TypeDesc::INT16:
                bd = OFX::eBitDepthUShort;
                break;
            //			case TypeDesc::UINT:
            case OpenImageIO::TypeDesc::UINT32:
            //			case TypeDesc::INT:
            case OpenImageIO::TypeDesc::INT32:
            //			case TypeDesc::ULONGLONG:
            case OpenImageIO::TypeDesc::UINT64:
            //			case TypeDesc::LONGLONG:
            case OpenImageIO::TypeDesc::INT64:
            case OpenImageIO::TypeDesc::FLOAT:
            case OpenImageIO::TypeDesc::DOUBLE:
                bd = OFX::eBitDepthFloat;
                break;
            case OpenImageIO::TypeDesc::STRING:
            case OpenImageIO::TypeDesc::PTR:
            case OpenImageIO::TypeDesc::LASTBASE:
            case OpenImageIO::TypeDesc::UNKNOWN:
            case OpenImageIO::TypeDesc::NONE:
            default:
            {
                in->close();
                BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("bad input format"));
            }
        }
        clipPreferences.setClipBitDepth(*this->_clipDst, bd);
    }

    if(getExplicitChannelConversion() == eParamReaderChannelAuto)
    {
        switch(spec.nchannels)
        {
            case 1:
                clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentAlpha);
                break;
            case 3:
                if(OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGB))
                {
                    clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGB);
                }
                else
                {
                    clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                }
                break;
            case 4:
                clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                break;
            default:
                clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                break;
        }
    }

    const float par = spec.get_float_attribute("PixelAspectRatio", 1.0f);
    clipPreferences.setPixelAspectRatio(*this->_clipDst, par);
    in->close();
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOReaderPlugin::render(const OFX::RenderArguments& args)
{
    ReaderPlugin::render(args);

    doGilRender<OpenImageIOReaderProcess>(*this, args);
}
}
}
}
}
