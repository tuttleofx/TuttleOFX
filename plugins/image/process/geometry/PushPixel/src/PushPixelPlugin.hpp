#ifndef _TUTTLE_PLUGIN_PUSHPIXELPLUGIN_HPP_
#define _TUTTLE_PLUGIN_PUSHPIXELPLUGIN_HPP_

#include "PushPixelDefinitions.hpp"
#include <ofxsImageEffect.h>
#include <tuttle/common/utils/global.hpp>

#include <boost/gil/gil_all.hpp>
#include <terry/filter/convolve.hpp>

namespace tuttle
{
namespace plugin
{
namespace pushPixel
{

template <typename Scalar>
struct PushPixelProcessParams
{
    EParamOutput _output;
    double _intensity;
    double _angle;
    double _size;
    EParamInterpolation _interpolation;

    EParamBorder _border;
    terry::filter::convolve_boundary_option _boundary_option;

    terry::filter::kernel_1d<Scalar> _kernelGaussianDerivative;
    terry::filter::kernel_1d<Scalar> _kernelGaussian;
};

/**
 * @brief PushPixel plugin
 */
class PushPixelPlugin : public OFX::ImageEffect
{
public:
    typedef float Scalar;

public:
    PushPixelPlugin(OfxImageEffectHandle handle);

public:
    PushPixelProcessParams<Scalar> getProcessParams(const OfxPointD& renderScale = OFX::kNoRenderScale) const;

    bool isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime);

    void getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois);

    void render(const OFX::RenderArguments& args);

public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc;  ///< Source image clip
    OFX::Clip* _clipMask; ///< Mask clip
    OFX::Clip* _clipDst;  ///< Destination image clip

    OFX::ChoiceParam* _paramOutput;
    OFX::DoubleParam* _paramSize;
    OFX::BooleanParam* _paramNormalizedKernel;
    OFX::DoubleParam* _paramIntensity;
    OFX::DoubleParam* _paramAngle;
    OFX::ChoiceParam* _paramInterpolation; ///< interpolation method
    OFX::ChoiceParam* _paramBorder;
};
}
}
}

#endif
