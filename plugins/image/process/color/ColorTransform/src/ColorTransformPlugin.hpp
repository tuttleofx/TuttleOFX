#ifndef _TUTTLE_PLUGIN_COLORTRANSFORM_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFORM_PLUGIN_HPP_

#include "ColorTransformDefinitions.hpp"
#include "ColorTransformAlgorithm.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <terry/math/Matrix.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransform
{

/**
 * @brief ColorTransform plugin
 */
class ColorTransformPlugin : public ImageEffectGilPlugin
{
public:
    typedef float Scalar;

public:
    ColorTransformPlugin(OfxImageEffectHandle handle);

public:
    ColorTransformParams getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    //	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
    //	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::DoubleParam* _paramHueShift;
    OFX::DoubleParam* _paramSaturation;

    OFX::DoubleParam* _paramContrastRGB;
    OFX::RGBAParam* _paramContrast;
    OFX::DoubleParam* _paramBrightnessRGB;
    OFX::RGBAParam* _paramBrightness;
    OFX::DoubleParam* _paramOffsetRGB;
    OFX::RGBAParam* _paramOffset;
    OFX::DoubleParam* _paramPivotRGB;
    OFX::RGBAParam* _paramPivot;
};
}
}
}

#endif
