#ifndef _TUTTLE_PLUGIN_SOBEL_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_SOBEL_PLUGIN_HPP_

#include "SobelDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

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

	EParamPass _pass;
	bool _unidimensional;

	bool _computeGradientNorm;
	bool _gradientNormManhattan;
	bool _computeGradientDirection;
	bool _gradientDirectionAbs;

	EParamBorder _border;
	boost::gil::convolve_boundary_option _boundary_option;

	boost::gil::kernel_1d<Scalar> _xKernelGaussianDerivative;
	boost::gil::kernel_1d<Scalar> _xKernelGaussian;
	boost::gil::kernel_1d<Scalar> _yKernelGaussianDerivative;
	boost::gil::kernel_1d<Scalar> _yKernelGaussian;
};

/**
 * @brief Sobel plugin
 */
class SobelPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
    SobelPlugin( OfxImageEffectHandle handle );

public:
	SobelProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

    void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

    void render( const OFX::RenderArguments &args );
	
public:
	OFX::Double2DParam* _paramSize;
	OFX::BooleanParam* _paramNormalizedKernel;
	OFX::BooleanParam* _paramReverseKernel;
	OFX::ChoiceParam* _paramPass;
	OFX::DoubleParam* _paramKernelEpsilon;
	OFX::BooleanParam* _paramUnidimensional;
    OFX::ChoiceParam* _paramBorder;
	OFX::BooleanParam* _paramComputeGradientNorm;
	OFX::BooleanParam* _paramGradientNormManhattan;
	OFX::BooleanParam* _paramComputeGradientDirection;
	OFX::BooleanParam* _paramGradientDirectionAbs;
    OFX::ChoiceParam* _paramOutputComponent;
};

}
}
}

#endif
