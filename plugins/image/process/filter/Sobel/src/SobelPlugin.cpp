#include "SobelPlugin.hpp"
#include "SobelProcess.hpp"
#include "SobelDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/gaussianKernel.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace sobel {


SobelPlugin::SobelPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramSize = fetchDouble2DParam( kParamSize );
	_paramNormalizedKernel = fetchBooleanParam( kParamNormalizedKernel );
	_paramUnidimensional = fetchBooleanParam( kParamUnidimensional );
	_paramBorder = fetchChoiceParam( kParamBorder );
	_paramComputeGradientNorm = fetchBooleanParam( kParamComputeGradientNorm );
	_paramNormalizedNorm = fetchBooleanParam( kParamNormalizedNorm );
	_paramGradientNormManhattan = fetchBooleanParam( kParamGradientNormManhattan );
	_paramComputeGradientDirection = fetchBooleanParam( kParamComputeGradientDirection );
	_paramGradientDirectionAbs = fetchBooleanParam( kParamGradientDirectionAbs );
}

SobelProcessParams<SobelPlugin::Scalar> SobelPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	using namespace boost::gil;
	SobelProcessParams<Scalar> params;

	params._size   = ofxToGil( _paramSize->getValue() ) * ofxToGil( renderScale  );
	params._border = static_cast<EParamBorder>( _paramBorder->getValue() );
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

	bool normalizedKernel = _paramNormalizedKernel->getValue();

	params._unidimensional = _paramUnidimensional->getValue();
	params._xKernelGaussianDerivative = buildGaussianDerivative1DKernel<Scalar>( params._size.x, normalizedKernel );
	if( ! params._unidimensional )
		params._xKernelGaussian = buildGaussian1DKernel<Scalar>( params._size.x, normalizedKernel );

	if( params._size.x == params._size.y )
	{
		params._yKernelGaussianDerivative = params._xKernelGaussianDerivative;
		params._yKernelGaussian = params._xKernelGaussian;
	}
	else
	{
		params._yKernelGaussianDerivative = buildGaussianDerivative1DKernel<Scalar>( params._size.y, normalizedKernel );
		if( ! params._unidimensional )
			params._yKernelGaussian = buildGaussian1DKernel<Scalar>( params._size.y, normalizedKernel );
	}

	params._computeGradientNorm = _paramComputeGradientNorm->getValue();
	params._normalizedNorm = _paramNormalizedNorm->getValue();
	params._gradientNormManhattan = _paramGradientNormManhattan->getValue();
	params._computeGradientDirection = _paramComputeGradientDirection->getValue();
	params._gradientDirectionAbs = _paramGradientDirectionAbs->getValue();

	return params;
}

//void SobelPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
//{
//}

bool SobelPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	SobelProcessParams<Scalar> params = getProcessParams();

	switch( params._border )
	{
		case eParamBorderPadded:
			rod.x1 = srcRod.x1 + std::max( params._xKernelGaussianDerivative.left_size(), params._yKernelGaussian.left_size() );
			rod.y1 = srcRod.y1 + std::max( params._xKernelGaussian.left_size(), params._yKernelGaussianDerivative.left_size() );
			rod.x2 = srcRod.x2 - std::max( params._xKernelGaussianDerivative.right_size(), params._yKernelGaussian.right_size() );
			rod.y2 = srcRod.y2 - std::max( params._xKernelGaussian.right_size(), params._yKernelGaussianDerivative.right_size() );
			return true;
		default:
			break;
	}
	return false;
}

void SobelPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	SobelProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod                  = _clipSrc->getCanonicalRod( args.time );

	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - std::max( params._xKernelGaussianDerivative.left_size(), params._yKernelGaussian.left_size() );
	srcRoi.y1 = srcRod.y1 - std::max( params._xKernelGaussian.left_size(), params._yKernelGaussianDerivative.left_size() );
	srcRoi.x2 = srcRod.x2 + std::max( params._xKernelGaussianDerivative.right_size(), params._yKernelGaussian.right_size() );
	srcRoi.y2 = srcRod.y2 + std::max( params._xKernelGaussian.right_size(), params._yKernelGaussianDerivative.right_size() );
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}


bool SobelPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	SobelProcessParams<Scalar> params = getProcessParams();
	if( ( params._xKernelGaussianDerivative.size() == 0 || ( !params._unidimensional && params._xKernelGaussian.size() == 0 ) ) &&
	    ( params._yKernelGaussianDerivative.size() == 0 || ( !params._unidimensional && params._yKernelGaussian.size() == 0 ) ) )
	{
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void SobelPlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
//            case OFX::eBitDepthUByte :
//            {
//                SobelProcess<rgba8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                SobelProcess<rgba16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
            case OFX::eBitDepthFloat :
            {
                SobelProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
//    else if( dstComponents == OFX::ePixelComponentAlpha )
//    {
//        switch( dstBitDepth )
//        {
//            case OFX::eBitDepthUByte :
//            {
//                SobelProcess<gray8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                SobelProcess<gray16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
//                SobelProcess<gray32f_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//			default:
//			{
//				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
//				break;
//			}
//        }
//    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
