#ifndef _TUTTLE_PLUGIN_SEEXPR_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_SEEXPR_PLUGIN_HPP_

#include "SeExprDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPlugin.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace seExpr
{

template <typename Scalar>
struct SeExprProcessParams
{
    typedef boost::gil::point2<Scalar> Point;
    EParamChooseInput _inputType;
    std::string _code;
    Point _paramTextureOffset;
};

/**
 * @brief SeExpr plugin
 */
class SeExprPlugin : public GeneratorPlugin
{
public:
    typedef float Scalar;

public:
    SeExprPlugin(OfxImageEffectHandle handle);

public:
    SeExprProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);
    void render(const OFX::RenderArguments& args);

public:
    OFX::ChoiceParam* _paramInput;
    OFX::StringParam* _paramCode;
    OFX::StringParam* _paramFile;
    OFX::Double2DParam* _paramTextureOffset;

private:
    OFX::InstanceChangedArgs _instanceChangedArgs;
};
}
}
}

#endif
