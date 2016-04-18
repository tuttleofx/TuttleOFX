#include "OCIOColorSpacePlugin.hpp"
#include "OCIOColorSpaceProcess.hpp"

#include <tuttle/common/utils/color.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace OCIO = OCIO_NAMESPACE;

namespace bfs = boost::filesystem;

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace colorspace
{

using namespace boost::gil;

OCIOColorSpacePlugin::OCIOColorSpacePlugin(OfxImageEffectHandle handle, bool wasOCIOVarFund)
    : ImageEffectGilPlugin(handle)
    , _wasOCIOVarFund(wasOCIOVarFund)
{
    _paramFilename = fetchStringParam(kTuttlePluginFilename);
    _paramInputSpace = fetchChoiceParam(kParamInputSpace);
    _paramOutputSpace = fetchChoiceParam(kParamOutputSpace);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OCIOColorSpacePlugin::render(const OFX::RenderArguments& args)
{
    if(!_wasOCIOVarFund)
    {
        BOOST_THROW_EXCEPTION(exception::Unsupported()
                              << exception::user() + "Tuttle.ocio.colorspace : an OCIO environnement variable must be set "
                                                     "with a path to an OpenColorIO configuration file.\n ");
    }

    using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent components = _clipDst->getPixelComponents();

    // do the rendering
    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            switch(bitDepth)
            {
                case OFX::eBitDepthFloat:
                {
                    doGilRender<OCIOColorSpaceProcess, false, rgba_layout_t, bits32f>(*this, args);
                    return;
                }
                case OFX::eBitDepthUByte:
                case OFX::eBitDepthUShort:
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                {
                    BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Bit depth (" +
                                                                          mapBitDepthEnumToString(bitDepth) +
                                                                          ") not recognized by the plugin.");
                }
            }
            break;
        }
        case OFX::ePixelComponentRGB:
        {
            switch(bitDepth)
            {
                case OFX::eBitDepthFloat:
                {
                    doGilRender<OCIOColorSpaceProcess, false, rgb_layout_t, bits32f>(*this, args);
                    return;
                }
                case OFX::eBitDepthUByte:
                case OFX::eBitDepthUShort:
                case OFX::eBitDepthCustom:
                case OFX::eBitDepthNone:
                {
                    BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Bit depth (" +
                                                                          mapBitDepthEnumToString(bitDepth) +
                                                                          ") not recognized by the plugin.");
                }
            }
            break;
        }
        case OFX::ePixelComponentAlpha: //@todo support Alpha (mfe)
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

OCIOColorSpaceProcessParams OCIOColorSpacePlugin::getProcessParams(const OfxPointD& renderScale) const
{
    using namespace boost::filesystem;

    OCIOColorSpaceProcessParams params;

    /// Here change config Path if needed
    std::string str;
    _paramFilename->getValue(str);
    if(!bfs::exists(str))
    {
        BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::filename(str));
    }

    // Get the OCIO configuration processor.
    params._config = OCIO::Config::CreateFromFile(str.c_str());

    int index;
    _paramInputSpace->getValue(index);
    params._inputSpace = params._config->getColorSpaceNameByIndex(index);
    _paramOutputSpace->getValue(index);
    params._outputSpace = params._config->getColorSpaceNameByIndex(index);

    return params;
}
}
}
}
}
