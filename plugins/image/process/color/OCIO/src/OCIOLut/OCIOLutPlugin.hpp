#ifndef _TUTTLE_PLUGIN_OCIOLutPlugin_HPP_
#define _TUTTLE_PLUGIN_OCIOLutPlugin_HPP_

#include "OCIOLutDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <OpenColorIO/OpenColorIO.h>

namespace OCIO = OCIO_NAMESPACE;

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace lut
{

struct OCIOLutProcessParams
{
    std::string _filename;
    OCIO::Interpolation _interpolationType;
};

/**
 * @brief OCIO LUT plugin
 *
 */
class OCIOLutPlugin : public ImageEffectGilPlugin
{

public:
    OCIOLutPlugin(OfxImageEffectHandle handle);

public:
    void render(const OFX::RenderArguments& args);

public:
    OFX::StringParam* _paramFilename;
    OFX::ChoiceParam* _paramInterpolationType;

    OCIOLutProcessParams getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    EInterpolationType getInterpolationType() const
    {
        return static_cast<EInterpolationType>(_paramInterpolationType->getValue());
    }

    OCIO_NAMESPACE::Interpolation getOCIOInterpolationType(EInterpolationType tuttleInterpolationType) const
    {
        switch(tuttleInterpolationType)
        {
            case eInterpolationTypeNearest:
                return OCIO_NAMESPACE::INTERP_NEAREST;
            case eInterpolationTypeTetrahedral:
                return OCIO_NAMESPACE::INTERP_TETRAHEDRAL;
            case eInterpolationTypeLinear:
                return OCIO_NAMESPACE::INTERP_LINEAR;
        }
        BOOST_ASSERT(false);
        return OCIO_NAMESPACE::INTERP_LINEAR;
    }
};
}
}
}
}
#endif
