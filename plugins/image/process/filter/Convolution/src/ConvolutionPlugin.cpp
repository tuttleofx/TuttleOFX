#include "ConvolutionPlugin.hpp"
#include "ConvolutionProcess.hpp"
#include "ConvolutionDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace convolution
{

ConvolutionPlugin::ConvolutionPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramSize = fetchInt2DParam(kParamSize);
    _paramBorder = fetchChoiceParam(kParamBorder);

    _paramCoef.resize(kParamSizeMax);
    for(unsigned int y = 0; y < kParamSizeMax; ++y)
    {
        _paramCoef[y].resize(kParamSizeMax);
        for(unsigned int x = 0; x < kParamSizeMax; ++x)
        {
            _paramCoef[y][x] = fetchDoubleParam(getCoefName(y, x));
        }
    }
}

ConvolutionProcessParams ConvolutionPlugin::getProcessParams() const
{
    using namespace terry::filter;

    ConvolutionProcessParams params;

    const OfxPointI size = _paramSize->getValue();

    params._size.x = boost::numeric_cast<unsigned int>(size.x);
    params._size.y = boost::numeric_cast<unsigned int>(size.y);

    params._boundary_option = convolve_option_extend_mirror;
    params._border = static_cast<EParamBorder>(_paramBorder->getValue());

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
    params._convX.resize(params._size.x);
    params._convY.resize(params._size.y);
    for(unsigned int i = 0; i < params._size.x; ++i)
    {
        params._convX[i] = _paramCoef[0][i]->getValue();
    }
    for(unsigned int i = 0; i < params._size.y; ++i)
    {
        params._convY[i] = _paramCoef[1][i]->getValue();
    }
    /*
    params._convMatrix.resize( params._size.x, params._size.y );
    for( unsigned int y = 0; y < params._size.y; ++y )
    {
//		unsigned int yy = y * params._size.x;
            for( unsigned int x = 0; x < params._size.x; ++x )
            {
                    params._convMatrix(x, y) = _paramCoef[y][x]->getValue();
                    TUTTLE_LOG_INFO( "coef[" << y << "][" << x << "] = " << params._convMatrix(x, y) );
            }
    }
    */
    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ConvolutionPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<ConvolutionProcess>(*this, args);
}

bool ConvolutionPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    const ConvolutionProcessParams params = getProcessParams();
    const OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);

    switch(params._border)
    {
        case eParamBorderPadded:
            rod.x1 = srcRod.x1 + params._convX.left_size();
            rod.y1 = srcRod.y1 + params._convY.left_size();
            rod.x2 = srcRod.x2 - params._convX.right_size();
            rod.y2 = srcRod.y2 - params._convY.right_size();
            return true;
        default:
            break;
    }
    return false;
}

void ConvolutionPlugin::getRegionsOfInterest(const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois)
{
    OfxRectD srcRoi = args.regionOfInterest;
    const OfxPointI size = _paramSize->getValue();
    OfxPointD halfSize;

    halfSize.x = size.x * 0.5;
    halfSize.y = size.y * 0.5;
    srcRoi.x1 -= halfSize.x;
    srcRoi.x2 += halfSize.x;
    srcRoi.y1 -= halfSize.y;
    srcRoi.y2 += halfSize.y;
    rois.setRegionOfInterest(*_clipSrc, srcRoi);
}

bool ConvolutionPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    const OfxPointI size = _paramSize->getValue();
    if(size.x != 0 || size.y != 0)
        return false;

    identityClip = _clipSrc;
    identityTime = args.time;
    return true;
}

void ConvolutionPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kParamSize && args.reason == OFX::eChangeUserEdit)
    {
        const OfxPointI v = _paramSize->getValue();
        OfxPointI oddNumber;
        oddNumber.x = v.x | 1; // odd number
        oddNumber.y = v.y | 1; // odd number
        if(oddNumber != v)
            _paramSize->setValue(oddNumber);

        // separable convolution
        for(unsigned int x = 0; x < kParamSizeMax; ++x)
        {
            _paramCoef[0][x]->setIsSecretAndDisabled(static_cast<int>(x) >= v.x);
            _paramCoef[1][x]->setIsSecretAndDisabled(static_cast<int>(x) >= v.y);
        }
        // disable other matrix values
        for(unsigned int y = 2; y < kParamSizeMax; ++y)
        {
            for(unsigned int x = 0; x < kParamSizeMax; ++x)
            {
                _paramCoef[y][x]->setIsSecretAndDisabled(true);
            }
        }
    }
}
}
}
}
