#ifndef _TUTTLE_PLUGIN_SOBEL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_SOBEL_PLUGIN_HPP_

#include "SobelDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>

namespace tuttle {
namespace plugin {
namespace sobel {

template<typename Scalar>
struct SobelProcessParams
{
	boost::gil::point2<double> _size;
	EParamBorder _border;
	boost::gil::convolve_boundary_option _boundary_option;

	boost::gil::kernel_1d<Scalar> _xKernelGaussianDerivative;
	boost::gil::kernel_1d<Scalar> _xKernelGaussian;
	boost::gil::kernel_1d<Scalar> _yKernelGaussianDerivative;
	boost::gil::kernel_1d<Scalar> _yKernelGaussian;

	bool _unidimensional;

	bool _computeGradientNorm;
	bool _gradientNormManhattan;
	bool _computeGradientDirection;
	bool _gradientDirectionAbs;
};

/**
 * @brief Sobel plugin
 */
class SobelPlugin : public OFX::ImageEffect
{
public:
	typedef float Scalar;
public:
    SobelPlugin( OfxImageEffectHandle handle );

public:
	SobelProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

//    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _clipSrc; ///< Source image clip
    OFX::Clip* _clipDst; ///< Destination image clip

	OFX::Double2DParam* _paramSize;
	OFX::BooleanParam* _paramNormalizedKernel;
	OFX::BooleanParam* _paramUnidimensional;
    OFX::ChoiceParam* _paramBorder;
	OFX::BooleanParam* _paramComputeGradientNorm;
	OFX::BooleanParam* _paramGradientNormManhattan;
	OFX::BooleanParam* _paramComputeGradientDirection;
	OFX::BooleanParam* _paramGradientDirectionAbs;
};

}
}
}

#endif
