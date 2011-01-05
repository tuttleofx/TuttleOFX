#include "SobelPlugin.hpp"
#include "SobelProcess.hpp"
#include "SobelDefinitions.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/gaussianKernel.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/lambda/lambda.hpp>


namespace {
namespace sobel {

namespace exception = tuttle::plugin::exception;

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
		  bool dPlanar, class DLayout, class DBits,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args )
{
	typedef boost::gil::pixel<SBits, SLayout> SPixel;
	typedef boost::gil::image<SPixel, sPlanar> SImage;
	typedef typename SImage::view_t SView;

	typedef boost::gil::pixel<DBits, DLayout> DPixel;
	typedef boost::gil::image<DPixel, dPlanar> DImage;
	typedef typename DImage::view_t DView;

	Process<SView, DView> procObj( plugin );

	procObj.setupAndProcess( args );
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
          bool dPlanar, class DLayout,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth dBitDepth )
{
    switch( dBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			localDoGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, DLayout, boost::gil::bits8>( plugin, args );
			return;
		}
		case OFX::eBitDepthUShort:
		{
			localDoGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, DLayout, boost::gil::bits16>( plugin, args );
			return;
		}
		case OFX::eBitDepthFloat:
		{
			localDoGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, DLayout, boost::gil::bits32f>( plugin, args );
			return;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(dBitDepth) + ") not recognized by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
          bool dPlanar,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
    switch( dComponent )
	{
		case OFX::ePixelComponentRGBA:
		{
			localDoGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, boost::gil::rgba_layout_t>( plugin, args, dBitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			localDoGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, boost::gil::rgb_layout_t>( plugin, args, dBitDepth );
			return;
		}
//		{
//			localDoGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, boost::gil::gray_layout_t>( plugin, args, dBitDepth );
//			return;
//		}
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel component (" + mapPixelComponentEnumToString(dComponent) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
	if( dPlanar )
	{
//		localDoGilRender2<Process, sPlanar, SLayout, SBits, true>( plugin, args, dComponent, dBitDepth );
	}
	else
	{
		localDoGilRender2<Process, sPlanar, SLayout, SBits, false>( plugin, args, dComponent, dBitDepth );
	}
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth sBitDepth, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
    switch( sBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			localDoGilRender2<Process, sPlanar, SLayout, boost::gil::bits8>( plugin, args, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::eBitDepthUShort:
		{
			localDoGilRender2<Process, sPlanar, SLayout, boost::gil::bits16>( plugin, args, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::eBitDepthFloat:
		{
			localDoGilRender2<Process, sPlanar, SLayout, boost::gil::bits32f>( plugin, args, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(sBitDepth) + ") not recognized by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class,class> class Process,
          bool sPlanar,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent sComponent, const OFX::EBitDepth sBitDepth, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
    switch( sComponent )
	{
		case OFX::ePixelComponentRGBA:
		{
			localDoGilRender2<Process, sPlanar, boost::gil::rgba_layout_t>( plugin, args, sBitDepth, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			localDoGilRender2<Process, sPlanar, boost::gil::rgb_layout_t>( plugin, args, sBitDepth, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		{
			localDoGilRender2<Process, sPlanar, boost::gil::gray_layout_t>( plugin, args, sBitDepth, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel component (" + mapPixelComponentEnumToString(sComponent) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}


template< template<class,class> class Process,
          class Plugin >
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const bool sPlanar, const OFX::EPixelComponent sComponent, const OFX::EBitDepth sBitDepth, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
	if( sPlanar )
	{
//		localDoGilRender2<Process, true>( plugin, args, sComponent, sBitDepth, dPlanar, dComponent, dBitDepth );
	}
	else
	{
		localDoGilRender2<Process, false>( plugin, args, sComponent, sBitDepth, dPlanar, dComponent, dBitDepth );
	}
}

template< template<class,class> class Process,
          class Plugin>
void localDoGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::Clip& sClip, const OFX::Clip& dClip )
{
	localDoGilRender2<Process, Plugin>( plugin, args, false, sClip.getPixelComponents(), sClip.getPixelDepth(), false, dClip.getPixelComponents(), dClip.getPixelDepth() );
}

}
}

namespace tuttle {
namespace plugin {
namespace sobel {


SobelPlugin::SobelPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramSize = fetchDouble2DParam( kParamSize );
	_paramNormalizedKernel = fetchBooleanParam( kParamNormalizedKernel );
	_paramReverseKernel = fetchBooleanParam( kParamReverseKernel );
	_paramPass = fetchChoiceParam( kParamPass );
	_paramKernelEpsilon = fetchDoubleParam( kParamKernelEpsilon );
	_paramUnidimensional = fetchBooleanParam( kParamUnidimensional );
	_paramBorder = fetchChoiceParam( kParamBorder );
	_paramComputeGradientNorm = fetchBooleanParam( kParamComputeGradientNorm );
	_paramGradientNormManhattan = fetchBooleanParam( kParamGradientNormManhattan );
	_paramComputeGradientDirection = fetchBooleanParam( kParamComputeGradientDirection );
	_paramGradientDirectionAbs = fetchBooleanParam( kParamGradientDirectionAbs );
	_paramOutputComponent = fetchChoiceParam( kParamOutputComponent );
}

SobelProcessParams<SobelPlugin::Scalar> SobelPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	using namespace boost::gil;
	SobelProcessParams<Scalar> params;

	params._size   = ofxToGil( _paramSize->getValue() ) * ofxToGil( renderScale  );
	params._unidimensional = _paramUnidimensional->getValue();
	params._pass = static_cast<EParamPass>( _paramPass->getValue() );

	params._computeGradientNorm = _paramComputeGradientNorm->getValue();
	params._gradientNormManhattan = _paramGradientNormManhattan->getValue();
	params._computeGradientDirection = _paramComputeGradientDirection->getValue();
	params._gradientDirectionAbs = _paramGradientDirectionAbs->getValue();


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
	double kernelEpsilon = _paramKernelEpsilon->getValue();

	params._xKernelGaussianDerivative = buildGaussianDerivative1DKernel<Scalar>( params._size.x, normalizedKernel, kernelEpsilon );
	if( ! params._unidimensional )
		params._xKernelGaussian = buildGaussian1DKernel<Scalar>( params._size.x, normalizedKernel, kernelEpsilon );

	if( params._size.x == params._size.y )
	{
		params._yKernelGaussianDerivative = params._xKernelGaussianDerivative;
		params._yKernelGaussian = params._xKernelGaussian;
	}
	else
	{
		params._yKernelGaussianDerivative = buildGaussianDerivative1DKernel<Scalar>( params._size.y, normalizedKernel, kernelEpsilon );
		if( ! params._unidimensional )
			params._yKernelGaussian = buildGaussian1DKernel<Scalar>( params._size.y, normalizedKernel, kernelEpsilon );
	}

	if( _paramReverseKernel->getValue() )
	{
		params._xKernelGaussianDerivative = boost::gil::reverse_kernel( params._xKernelGaussianDerivative );
		params._yKernelGaussianDerivative = boost::gil::reverse_kernel( params._yKernelGaussianDerivative );
	}
	return params;
}

template< typename Scalar >
std::ostream& operator<<( std::ostream& os, boost::gil::kernel_1d<Scalar>& kernel )
{
	using namespace boost;
	os << "[";
	std::for_each( kernel.begin(), kernel.end(), os << lambda::_1 << "," );
	os << "]";
	return os;
}

void SobelPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kParamInfos )
    {
		SobelProcessParams<Scalar> params = getProcessParams(args.renderScale);

		std::ostringstream infos;
		infos << "Kernel size for values (" << params._size.x << "x" << params._size.y << ") is:" << std::endl;
		infos << "  for X (" << params._xKernelGaussian.size() << "x" << params._xKernelGaussianDerivative.size() << ")" << std::endl;
		infos << "  for Y (" << params._yKernelGaussian.size() << "x" << params._yKernelGaussianDerivative.size() << ")" << std::endl;
		infos << std::endl;
		infos << "X :" << std::endl;
		infos << params._xKernelGaussian << std::endl;
		infos << params._xKernelGaussianDerivative << std::endl;
		infos << "Y :" << std::endl;
		infos << params._yKernelGaussian << std::endl;
		infos << params._yKernelGaussianDerivative << std::endl;
		infos << std::endl;
        
		sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     infos.str() );
    }
}

void SobelPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	EParamOutputComponent comp = static_cast<EParamOutputComponent>(_paramOutputComponent->getValue());
	switch( comp )
	{
		case eParamOutputComponentRGBA:
		{
			clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
			break;
		}
		case eParamOutputComponentRGB:
		{
			clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGB );
			break;
		}
	}
}

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

	OfxRectD marge;
	marge.x1 = std::max( params._xKernelGaussianDerivative.left_size(), params._yKernelGaussian.left_size() );
	marge.y1 = std::max( params._xKernelGaussian.left_size(), params._yKernelGaussianDerivative.left_size() );
	marge.x2 = std::max( params._xKernelGaussianDerivative.right_size(), params._yKernelGaussian.right_size() );
	marge.y2 = std::max( params._xKernelGaussian.right_size(), params._yKernelGaussianDerivative.right_size() );
	switch( params._pass )
	{
		case eParamPass1:
		{
			marge.y1 = 0;
			marge.y2 = 0;
			break;
		}
		case eParamPass2:
		{
			marge.x1 = 0;
			marge.x2 = 0;
			break;
		}
		case eParamPassFull:
		{
			break;
		}
	}
	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - marge.x1;
	srcRoi.y1 = srcRod.y1 - marge.y1;
	srcRoi.x2 = srcRod.x2 + marge.x2;
	srcRoi.y2 = srcRod.y2 + marge.y2;
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
	TUTTLE_COUT_X( 20, "-" );
	TUTTLE_COUT_VAR( mapPixelComponentEnumToString( _clipSrc->getPixelComponents() ) );
	TUTTLE_COUT_VAR( mapBitDepthEnumToString( _clipSrc->getPixelDepth() ) );

	TUTTLE_COUT_VAR( mapPixelComponentEnumToString( _clipDst->getPixelComponents() ) );
	TUTTLE_COUT_VAR( mapBitDepthEnumToString( _clipDst->getPixelDepth() ) );

	::sobel::localDoGilRender2<SobelProcess>( *this, args, *_clipSrc, *_clipDst );
}


}
}
}
