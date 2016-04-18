#ifndef _TUTTLE_PLUGIN_CTL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_CTL_PLUGIN_HPP_

#include "CTLDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace ctl
{

template <typename Scalar>
struct CTLProcessParams
{
    EParamChooseInput _inputType;
    std::vector<std::string> _paths;
    std::string _filename;
    std::string _module;
    std::string _code;
};

/**
 * @brief CTL plugin
 */
class CTLPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    CTLPlugin(OfxImageEffectHandle handle);

public:
    CTLProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _paramInput;
    OFX::StringParam* _paramCode;
    OFX::StringParam* _paramFile;
    OFX::PushButtonParam* _paramUpdateRender;

private:
    OFX::InstanceChangedArgs _instanceChangedArgs;
};
}
}
}

#endif
