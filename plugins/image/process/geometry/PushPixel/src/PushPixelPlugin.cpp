#include "PushPixelPlugin.hpp"
#include "PushPixelProcess.hpp"
#include "PushPixelDefinitions.hpp"

#include <terry/filter/gaussianKernel.hpp>

#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

#include <boost/math/constants/constants.hpp>

namespace tuttle
{
namespace plugin
{
namespace pushPixel
{

PushPixelPlugin::PushPixelPlugin(OfxImageEffectHandle handle)
    : ImageEffect(handle)
{
    _clipSrc = fetchClip(kOfxImageEffectSimpleSourceClipName);
    _clipMask = fetchClip(kClipMask);
    _clipDst = fetchClip(kOfxImageEffectOutputClipName);

    _paramOutput = fetchChoiceParam(kParamOutput);
    _paramSize = fetchDoubleParam(kParamSize);
    _paramNormalizedKernel = fetchBooleanParam(kParamNormalizedKernel);
    _paramIntensity = fetchDoubleParam(kParamIntensity);
    _paramAngle = fetchDoubleParam(kParamAngle);
    _paramInterpolation = fetchChoiceParam(kParamInterpolation);
    _paramBorder = fetchChoiceParam(kParamBorder);
}

PushPixelProcessParams<PushPixelPlugin::Scalar> PushPixelPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    using namespace boost::gil;
    using namespace terry;
    using namespace terry::filter;

    PushPixelProcessParams<Scalar> params;
    params._output = static_cast<EParamOutput>(_paramOutput->getValue());
    params._size = _paramSize->getValue();
    params._intensity = _paramIntensity->getValue() * params._size;
    params._size *= renderScale.x;
    params._intensity *= renderScale.x;

    static const double degreeToRadian = boost::math::constants::pi<double>() / 180.0;
    params._angle = _paramAngle->getValue() * degreeToRadian;

    params._interpolation = static_cast<EParamInterpolation>(_paramInterpolation->getValue());

    params._border = static_cast<EParamBorder>(_paramBorder->getValue());
    params._boundary_option = convolve_option_extend_mirror;
    switch(params._border)
    {
        case eParamBorderMirror:
            params._boundary_option = convolve_option_extend_mirror;
            break;
        case eParamBorderConstant:
            params._boundary_option = convolve_option_extend_constant;
            break;
        case eParamBorderBlack:
            params._boundary_option = convolve_option_extend_zero;
            break;
        case eParamBorderPadded:
            params._boundary_option = convolve_option_extend_padded;
            break;
    }

    bool normalizedKernel = _paramNormalizedKernel->getValue();
    params._kernelGaussianDerivative = buildGaussianDerivative1DKernel<Scalar>(params._size, normalizedKernel);
    params._kernelGaussian = buildGaussian1DKernel<Scalar>(params._size, normalizedKernel);

    return params;
}

bool PushPixelPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    PushPixelProcessParams<Scalar> params = getProcessParams();
    if(params._size != 0 && params._intensity != 0)
        return false;

    identityClip = _clipSrc;
    identityTime = args.time;
    return true;
}

void PushPixelPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois)
{

    OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);
    // OfxRectD dstRod = _clipDst->getCanonicalRod( args.time );

    if(_clipMask->isConnected())
    {
        PushPixelProcessParams<Scalar> params(getProcessParams());
        // OfxRectD maskRod = _clipMask->getCanonicalRod( args.time );
        double maxKernelTL = std::max(params._kernelGaussian.left_size(), params._kernelGaussianDerivative.left_size());
        double maxKernelBR = std::max(params._kernelGaussian.right_size(), params._kernelGaussianDerivative.right_size());
        OfxRectD maskRoi;
        maskRoi.x1 = args.regionOfInterest.x1 - (maxKernelTL * _clipMask->getPixelAspectRatio());
        maskRoi.y1 = args.regionOfInterest.y1 - maxKernelTL;
        maskRoi.x2 = args.regionOfInterest.x2 + (maxKernelBR * _clipMask->getPixelAspectRatio());
        maskRoi.y2 = args.regionOfInterest.y2 + maxKernelBR;
        rois.setRegionOfInterest(*_clipMask, maskRoi);
    }

    // no tiles on src clip, because it depends on the mask content so we can't
    // define the maximal bounding box needed...
    rois.setRegionOfInterest(*_clipSrc, srcRod);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PushPixelPlugin::render(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    BOOST_ASSERT(_clipDst->getPixelDepth() == _clipSrc->getPixelDepth());
    BOOST_ASSERT(_clipDst->getPixelComponents() == _clipSrc->getPixelComponents());

    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                PushPixelProcess<rgba8_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                PushPixelProcess<rgba16_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                PushPixelProcess<rgba32f_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthNone:
                TUTTLE_LOG_FATAL("BitDepthNone not recognize.");
                return;
            case OFX::eBitDepthCustom:
                TUTTLE_LOG_FATAL("BitDepthCustom not recognize.");
                return;
        }
    } /*
     else if( dstComponents == OFX::ePixelComponentAlpha )
     {
             switch( dstBitDepth )
             {
                     case OFX::eBitDepthUByte :
                     {
                             PushPixelProcess<gray8_view_t> p( *this );
                             p.setupAndProcess( args );
                             break;
                     }
                     case OFX::eBitDepthUShort :
                     {
                             PushPixelProcess<gray16_view_t> p( *this );
                             p.setupAndProcess( args );
                             break;
                     }
                     case OFX::eBitDepthFloat :
                     {
                             PushPixelProcess<gray32f_view_t> p( *this );
                             p.setupAndProcess( args );
                             break;
                     }
                     case OFX::eBitDepthNone :
                             TUTTLE_LOG_FATAL( "BitDepthNone not recognize." );
                             return;
                     case OFX::eBitDepthCustom :
                             TUTTLE_LOG_FATAL( "BitDepthCustom not recognize." );
                             return;
             }
     }*/
    else
    {
        TUTTLE_LOG_FATAL("Components not recognize.");
    }
}
}
}
}
