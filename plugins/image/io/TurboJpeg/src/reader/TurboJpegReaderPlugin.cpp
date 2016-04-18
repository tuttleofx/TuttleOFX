#include "TurboJpegReaderPlugin.hpp"
#include "TurboJpegReaderProcess.hpp"
#include "TurboJpegReaderDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

#include <cstdio>
#include <turbojpeg.h>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace reader
{

TurboJpegReaderPlugin::TurboJpegReaderPlugin(OfxImageEffectHandle handle)
    : ReaderPlugin(handle)
{
    _optimization = fetchChoiceParam(kParamOptimization);
    _fastUpsampling = fetchBooleanParam(kParamFastUpsampling);
}

TurboJpegReaderProcessParams TurboJpegReaderPlugin::getProcessParams(const OfxTime time) const
{
    TurboJpegReaderProcessParams params;
    params.filepath = getAbsoluteFilenameAt(time);
    params.optimization = static_cast<ETurboJpegOptimization>(_optimization->getValue());
    params.fastUpsampling = _fastUpsampling->getValue();
    return params;
}

void TurboJpegReaderPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    ReaderPlugin::changedParam(args, paramName);
}

bool TurboJpegReaderPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    try
    {
        FILE* file = NULL;
        unsigned char* jpegbuf = NULL;
        unsigned long jpgbufsize = 0;

        const std::string filepath = getAbsoluteFilenameAt(args.time);

        file = fopen(filepath.c_str(), "rb");
        if(file == NULL)
        {
            BOOST_THROW_EXCEPTION(exception::File() << exception::user("TurboJpeg: Unable to open file")
                                                    << exception::filename(filepath));
        }

        fseek(file, 0, SEEK_END);
        jpgbufsize = ftell(file);
        jpegbuf = new unsigned char[jpgbufsize];

        fseek(file, 0, SEEK_SET);
        fread(jpegbuf, jpgbufsize, 1, file);

        const tjhandle jpeghandle = tjInitDecompress();
        int width = 0;
        int height = 0;
        int jpegsubsamp = -1;

        int ret = tjDecompressHeader2(jpeghandle, jpegbuf, jpgbufsize, &width, &height, &jpegsubsamp);

        if(ret != 0)
        {
            BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::user(tjGetErrorStr())
                                                            << exception::filename(filepath));
        }

        tjDestroy(jpeghandle);
        // free(jpegbuf);
        delete[] jpegbuf;
        jpegbuf = NULL;

        fclose(file);
        file = NULL;

        rod.x1 = 0;
        rod.x2 = width * this->_clipDst->getPixelAspectRatio();
        rod.y1 = 0;
        rod.y2 = height;
        // TUTTLE_LOG_VAR( TUTTLE_INFO, rod );
    }
    catch(std::exception& e)
    {
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("TurboJpeg: Unable to open file")
                                                                  << exception::filename(getAbsoluteFilenameAt(args.time)));
    }

    return true;
}

void TurboJpegReaderPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
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
void TurboJpegReaderPlugin::render(const OFX::RenderArguments& args)
{
    ReaderPlugin::render(args);
    doGilRender<TurboJpegReaderProcess>(*this, args);
}
}
}
}
}
