#ifndef _TUTTLE_PLUGIN_SWSCALE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_SWSCALE_PLUGIN_HPP_

#include "SwscaleDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace swscale
{

struct SwscaleProcessParams
{
    EParamFilter _filter;
    int _sws_filter;
};

/**
 * @brief Swscale plugin
 */
class SwscalePlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;
    typedef boost::gil::point2<double> Point2;

public:
    SwscalePlugin(OfxImageEffectHandle handle);

public:
    SwscaleProcessParams getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void updateVisibleTools();

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);
    //	void getRegionsOfInterest  ( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

private:
    bool isIdentity() const;

public:
    OFX::ChoiceParam* _paramMode;

    OFX::ChoiceParam* _paramFormat;

    OFX::Int2DParam* _paramSize;
    OFX::IntParam* _paramSizeWidth;
    OFX::IntParam* _paramSizeHeight;
    OFX::BooleanParam* _paramSizeKeepRatio;
    OFX::ChoiceParam* _paramSizeOrientation;

    OFX::Double2DParam* _paramScale;

    OFX::ChoiceParam* _paramFilter;
};
}
}
}

#endif
