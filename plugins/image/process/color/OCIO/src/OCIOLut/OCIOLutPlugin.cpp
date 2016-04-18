#include "OCIOLutPlugin.hpp"
#include "OCIOLutProcess.hpp"

#include <tuttle/common/utils/color.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace lut
{

using namespace boost::gil;

OCIOLutPlugin::OCIOLutPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramFilename = fetchStringParam(kTuttlePluginFilename);
    _paramInterpolationType = fetchChoiceParam(kParamInterpolationType);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OCIOLutPlugin::render(const OFX::RenderArguments& args)
{
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
                    doGilRender<OCIOLutProcess, false, rgba_layout_t, bits32f>(*this, args);
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
                    doGilRender<OCIOLutProcess, false, rgb_layout_t, bits32f>(*this, args);
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

OCIOLutProcessParams OCIOLutPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    using namespace boost::filesystem;

    OCIOLutProcessParams params;
    params._interpolationType =
        getOCIOInterpolationType(static_cast<EInterpolationType>(_paramInterpolationType->getValue()));

    params._filename = _paramFilename->getValue();
    if(!bfs::exists(params._filename))
    {
        BOOST_THROW_EXCEPTION(exception::FileNotExist() << exception::filename(params._filename));
    }

    return params;
}
}
}
}
}
