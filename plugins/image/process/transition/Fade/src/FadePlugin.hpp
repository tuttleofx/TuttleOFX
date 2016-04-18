#ifndef _TUTTLE_PLUGIN_FADE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FADE_PLUGIN_HPP_

#include "FadeDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace fade
{

struct FadeProcessParams
{
    double _transition;
    EParamRod _rod;
    boost::gil::rgba32f_pixel_t _color;
};

/**
 * @brief Fade plugin
 */
class FadePlugin : public OFX::ImageEffect
{
public:
    typedef float Scalar;

public:
    FadePlugin(OfxImageEffectHandle handle);

public:
    FadeProcessParams getProcessParams() const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::Clip* _clipDst;     ///< Destination image clip
    OFX::Clip* _clipSrcFrom; ///< Source image clip
    OFX::Clip* _clipSrcTo;   ///< Source image clip

    OFX::DoubleParam* _paramTransition; ///< Source image clip
    OFX::ChoiceParam* _paramRod;
    OFX::RGBAParam* _paramColor;
};
}
}
}

#endif
