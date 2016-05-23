#ifndef _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_

#include "InvertDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace invert
{

struct InvertProcessParams
{
    InvertProcessParams()
        : _red(false)
        , _green(false)
        , _blue(false)
        , _alpha(false)
        , _gray(false)
    {
    }

    bool _red;
    bool _green;
    bool _blue;
    bool _alpha;
    bool _gray;
};

/**
 * @brief
 *
 */
class InvertPlugin : public ImageEffectGilPlugin
{
public:
    InvertPlugin(OfxImageEffectHandle handle);

public:
    InvertProcessParams getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void render(const OFX::RenderArguments& args);

protected:
    OFX::GroupParam* _paramProcessGroup;
    OFX::BooleanParam* _paramProcessR;
    OFX::BooleanParam* _paramProcessG;
    OFX::BooleanParam* _paramProcessB;
    OFX::BooleanParam* _paramProcessA;
    OFX::BooleanParam* _paramProcessGray;
};
}
}
}

#endif
