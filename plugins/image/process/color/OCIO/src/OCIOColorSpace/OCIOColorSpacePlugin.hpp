#ifndef _TUTTLE_PLUGIN_OCIOColorSpacePlugin_HPP_
#define _TUTTLE_PLUGIN_OCIOColorSpacePlugin_HPP_

#include "OCIOColorSpaceDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <OpenColorIO/OpenColorIO.h>

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace colorspace
{

struct OCIOColorSpaceProcessParams
{
    OCIO_NAMESPACE::ConstConfigRcPtr _config;
    std::string _inputSpace;
    std::string _outputSpace;
};

/**
 * @brief OCIO LUT plugin
 *
 */
class OCIOColorSpacePlugin : public ImageEffectGilPlugin
{

public:
    OCIOColorSpacePlugin(OfxImageEffectHandle handle, bool wasOCIOVarFund);

public:
    void render(const OFX::RenderArguments& args);

public:
    OFX::StringParam* _paramFilename;
    OFX::ChoiceParam* _paramInputSpace;
    OFX::ChoiceParam* _paramOutputSpace;

    OCIOColorSpaceProcessParams getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

private:
    const bool _wasOCIOVarFund;
};
}
}
}
}
#endif
