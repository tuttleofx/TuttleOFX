#ifndef _TUTTLE_PLUGIN_IDKEYER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IDKEYER_PLUGIN_HPP_

#include "IdKeyerDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace idKeyer
{

template <class View>
struct IdKeyerProcessParams
{
    typedef typename boost::gil::rgba32f_pixel_t Pixel;
    std::vector<Pixel> _colors;
    double _tolerance;
    bool _useAlpha;
};

/**
 * @brief IdKeyer plugin
 */
class IdKeyerPlugin : public ImageEffectGilPlugin
{
public:
    IdKeyerPlugin(OfxImageEffectHandle handle);

public:
    void render(const OFX::RenderArguments& args);
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    template <class View>
    IdKeyerProcessParams<View> getProcessParams() const;

public:
    typedef std::vector<OFX::RGBAParam*> RGBAParamVector;

    RGBAParamVector _paramColors;
    OFX::IntParam* _paramNbPoints;
    OFX::BooleanParam* _paramUseAlpha;
    OFX::DoubleParam* _paramTolerance;

private:
    OFX::InstanceChangedArgs _instanceChangedArgs;
};
}
}
}

#endif
