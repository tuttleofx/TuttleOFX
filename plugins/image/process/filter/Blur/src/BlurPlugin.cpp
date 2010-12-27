#include "BlurPlugin.hpp"
#include "BlurProcess.hpp"
#include "BlurDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

BlurPlugin::BlurPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramSize   = fetchDouble2DParam( kParamSize );
	_paramBorder = fetchChoiceParam( kParamBorder );
	_paramNormalizedKernel = fetchBooleanParam( kParamNormalizedKernel );
	_paramKernelEpsilon = fetchDoubleParam( kParamKernelEpsilon );
}

BlurProcessParams<BlurPlugin::Scalar> BlurPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	namespace bgil = boost::gil;

	BlurProcessParams<Scalar> params;
	params._size   = ofxToGil( _paramSize->getValue() ) * ofxToGil( renderScale  );
	params._border = static_cast<EParamBorder>( _paramBorder->getValue() );

	bool normalizedKernel = _paramNormalizedKernel->getValue();
	double kernelEpsilon = _paramKernelEpsilon->getValue();

	params._gilKernelX = buildGaussian1DKernel<Scalar>( params._size.x, normalizedKernel, kernelEpsilon );
	params._gilKernelY = buildGaussian1DKernel<Scalar>( params._size.y, normalizedKernel, kernelEpsilon );
	
	params._boundary_option = bgil::convolve_option_extend_mirror;
	switch( params._border )
	{
		case eParamBorderMirror:
			params._boundary_option = bgil::convolve_option_extend_mirror;
			break;
		case eParamBorderConstant:
			params._boundary_option = bgil::convolve_option_extend_constant;
			break;
		case eParamBorderBlack:
			params._boundary_option = bgil::convolve_option_extend_zero;
			break;
		case eParamBorderPadded:
			params._boundary_option = bgil::convolve_option_extend_padded;
			break;
	}
	return params;
}

/*
   void BlurPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
   {
    if( paramName == kHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kHelpString );
    }
   }
 */

bool BlurPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	BlurProcessParams<Scalar> params = getProcessParams();

	switch( params._border )
	{
		case eParamBorderPadded:
			rod.x1 = srcRod.x1 + params._gilKernelX.left_size();
			rod.y1 = srcRod.y1 + params._gilKernelY.left_size();
			rod.x2 = srcRod.x2 - params._gilKernelX.right_size();
			rod.y2 = srcRod.y2 - params._gilKernelY.right_size();
			return true;
		default:
			break;
	}
	return false;
}

void BlurPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	BlurProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod                  = _clipSrc->getCanonicalRod( args.time );

	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - params._gilKernelX.left_size();
	srcRoi.y1 = srcRod.y1 - params._gilKernelY.left_size();
	srcRoi.x2 = srcRod.x2 + params._gilKernelX.right_size();
	srcRoi.y2 = srcRod.y2 + params._gilKernelY.right_size();
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}

bool BlurPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	BlurProcessParams<Scalar> params = getProcessParams();
	if( params._size.x != 0 || params._size.y != 0 )
		return false;

	identityClip = _clipSrc;
	identityTime = args.time;
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void BlurPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<BlurProcess>( *this, args );
}

}
}
}
