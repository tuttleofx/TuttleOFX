#include "Jpeg2000ReaderPlugin.hpp"
#include "Jpeg2000ReaderProcess.hpp"
#include "Jpeg2000ReaderDefinitions.hpp"

#include <tuttle/ioplugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace reader
{

using namespace tuttle::io;
using namespace boost::gil;
using namespace boost::filesystem;

Jpeg2000ReaderPlugin::Jpeg2000ReaderPlugin(OfxImageEffectHandle handle)
    : ReaderPlugin(handle)
{
}

Jpeg2000ReaderPlugin::~Jpeg2000ReaderPlugin()
{
    _reader.close();
}

Jpeg2000ReaderProcessParams Jpeg2000ReaderPlugin::getProcessParams(const OfxTime time)
{
    if(varyOnTime() && (time < getFirstTime() || time > getLastTime()))
    {
        BOOST_THROW_EXCEPTION(exception::BadIndex() << exception::user("Time value outside bounds."));
    }
    Jpeg2000ReaderProcessParams params;

    params._paramFilepath = getAbsoluteFilenameAt(time);

    return params;
}

void Jpeg2000ReaderPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kTuttlePluginFilename)
    {
        _reader.close();
        _fileInfos._failed = true;
    }
    ReaderPlugin::changedParam(args, paramName);
}

bool Jpeg2000ReaderPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    FileInfo fileInfo = retrieveFileInfo(args.time);
    if(fileInfo._failed)
    {
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("Jpeg2000: Unable to open file")
                                                                  << exception::filename(getAbsoluteFilenameAt(args.time)));
    }

    rod.x1 = 0;
    rod.x2 = fileInfo._width;
    rod.y1 = 0;
    rod.y2 = fileInfo._height;

    return true;
}

void Jpeg2000ReaderPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, _paramFilepath->getValue() );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, getAbsoluteFirstFilename() );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, getFirstTime() );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, getAbsoluteFilenameAt(getFirstTime()) );

    ReaderPlugin::getClipPreferences(clipPreferences);

    FileInfo fileInfo = retrieveFileInfo(getFirstTime());
    if(fileInfo._failed)
    {
        BOOST_THROW_EXCEPTION(exception::Failed() << exception::user("Jpeg2000: Unable to read file infos."));
    }

    if(getExplicitBitDepthConversion() == eParamReaderBitDepthAuto)
    {
        clipPreferences.setClipBitDepth(*_clipDst, fileInfo._precisionType);
    }
    else // if we explicitly specify which conversion we want
    {
        clipPreferences.setClipBitDepth(*_clipDst, getOfxExplicitConversion());
    }

    if(getExplicitChannelConversion() == eParamReaderChannelAuto)
    {
        switch(fileInfo._components)
        {
            case 1:
                clipPreferences.setClipComponents(*_clipDst, OFX::ePixelComponentAlpha);
                break;
            case 3:
                clipPreferences.setClipComponents(*_clipDst, OFX::ePixelComponentRGB);
                break;
            case 4:
                clipPreferences.setClipComponents(*_clipDst, OFX::ePixelComponentRGBA);
                break;
            default:
            {
                BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user() +
                                                                      "Jpeg2000: Unexpected number of channels (" +
                                                                      fileInfo._components + ")");
            }
        }
    }

    clipPreferences.setPixelAspectRatio(*_clipDst, 1.0);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Jpeg2000ReaderPlugin::render(const OFX::RenderArguments& args)
{
    if(retrieveFileInfo(args.time)._failed)
    {
        BOOST_THROW_EXCEPTION(exception::BitDepthMismatch() << exception::user("Jpeg2000: get file info failed"));
    }
    // Full image decoding
    _reader.decode();

    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = this->_clipDst->getPixelDepth();
    OFX::EPixelComponent dstComponents = this->_clipDst->getPixelComponents();
    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                Jpeg2000ReaderProcess<rgba8_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                Jpeg2000ReaderProcess<rgba16_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                Jpeg2000ReaderProcess<rgba32f_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthNone:
            case OFX::eBitDepthCustom:
            {
                BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                      << exception::user("Jpeg2000: BitDepthCustom not recognize"));
            }
        }
    }
    else if(dstComponents == OFX::ePixelComponentRGB)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                Jpeg2000ReaderProcess<rgb8_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                Jpeg2000ReaderProcess<rgb16_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                Jpeg2000ReaderProcess<rgb32f_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthNone:
            case OFX::eBitDepthCustom:
            {
                BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                      << exception::user("Jpeg2000: BitDepthCustom not recognize"));
            }
        }
    }
    else if(dstComponents == OFX::ePixelComponentAlpha)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                Jpeg2000ReaderProcess<gray8_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                Jpeg2000ReaderProcess<gray16_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                Jpeg2000ReaderProcess<gray32f_view_t> fred(*this);
                fred.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthNone:
            case OFX::eBitDepthCustom:
            {
                BOOST_THROW_EXCEPTION(exception::BitDepthMismatch()
                                      << exception::user("Jpeg2000: BitDepthCustom not recognize"));
            }
        }
    }
    else
    {
        TUTTLE_LOG_FATAL(dstComponents << " not recognize.");
        BOOST_THROW_EXCEPTION(exception::Unknown());
    }
}

Jpeg2000ReaderPlugin::FileInfo Jpeg2000ReaderPlugin::retrieveFileInfo(const OfxTime time)
{
    if(!_fileInfos._failed)
    {
        if(time == _fileInfos._time && _reader.imageReady())
        {
            return _fileInfos;
        }

        if(_reader.imageReady())
        {
            _reader.close();
        }
    }

    // Open new source
    std::string filename = getAbsoluteFilenameAt(time);
    try
    {
        _reader.open(filename);
    }
    catch(boost::exception& e)
    {
        _fileInfos._failed = true;
        BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::user("Jpeg2000: Unable to open file")
                                                        << exception::filename(filename));
    }
    _fileInfos._failed = false;
    // No choice if we want to get
    // the image dimensions later
    try
    {
        _reader.decode(true);
    }
    catch(boost::exception& e)
    {
        _fileInfos._failed = true;
        BOOST_THROW_EXCEPTION(exception::File() << exception::user("Jpeg2000: Unable to decode file")
                                                << exception::filename(filename));
    }

    if(!_reader.componentsConform())
    {
        _fileInfos._failed = true;
        BOOST_THROW_EXCEPTION(exception::ImageFormat() << exception::user("Jpeg2000 image components aren't conform.")
                                                       << exception::filename(filename) << exception::time(time));
    }

    _fileInfos._time = time;
    _fileInfos._width = _reader.width();
    _fileInfos._height = _reader.height();
    _fileInfos._components = _reader.components();
    _fileInfos._precision = _reader.precision();

    switch(_fileInfos._precision)
    {
        case 8:
            _fileInfos._precisionType = OFX::eBitDepthUByte;
            break;
        case 12:
        case 16:
            _fileInfos._precisionType = OFX::eBitDepthUShort;
            break;
        case 32:
            _fileInfos._precisionType = OFX::eBitDepthFloat;
            break;
        default:
            _fileInfos._failed = true;
            BOOST_THROW_EXCEPTION(exception::ImageFormat()
                                  << exception::user() + "Jpeg2000: Bit depth not handled ! (" + _reader.precision() + ")"
                                  << exception::filename(filename) << exception::time(time));
    }

    return _fileInfos;
}
}
}
}
}
