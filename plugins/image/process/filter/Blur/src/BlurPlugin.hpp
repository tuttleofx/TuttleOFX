#ifndef _TUTTLE_PLUGIN_BLUR_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_BLUR_PLUGIN_HPP_

#include "BlurDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <terry/filter/convolve.hpp>

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace blur
{

template <typename Scalar>
struct BlurProcessParams
{
    typedef typename terry::filter::kernel_1d<Scalar> Kernel;
    terry::point2<double> _size;
    EParamBorder _border;
    terry::filter::convolve_boundary_option _boundary_option;

    Kernel _gilKernelX;
    Kernel _gilKernelY;
};

/**
 * @brief Blur plugin
 */
class BlurPlugin : public ImageEffectGilPlugin
{
    typedef float Scalar;

public:
    BlurPlugin(OfxImageEffectHandle handle);

public:
    BlurProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    //    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);
    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);
    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void render(const OFX::RenderArguments& args);

public:
    OFX::Double2DParam* _paramSize;
    OFX::ChoiceParam* _paramBorder;
    OFX::BooleanParam* _paramNormalizedKernel;
    OFX::DoubleParam* _paramKernelEpsilon;
};
}
}
}

#endif
