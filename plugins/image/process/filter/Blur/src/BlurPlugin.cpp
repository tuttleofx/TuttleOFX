#include "BlurPlugin.hpp"
#include "BlurProcess.hpp"
#include "BlurDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

BlurPlugin::BlurPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramSize   = fetchDouble2DParam( kParamSize );
	_paramBorder = fetchChoiceParam( kParamBorder );
}

BlurProcessParams<BlurPlugin::Scalar> BlurPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	BlurProcessParams<Scalar> params;
	params._size   = ofxToGil( _paramSize->getValue() ) * ofxToGil( renderScale  );
	params._border = static_cast<EParamBorder>( _paramBorder->getValue() );

	//	COUT_X(80, "X");
	params._gilKernelX = buildGaussian1DKernel<Scalar>( params._size.x );
	//	COUT_X(80, "Y");
	params._gilKernelY = buildGaussian1DKernel<Scalar>( params._size.y );
	
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
	using namespace boost::gil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				BlurProcess<rgba8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				BlurProcess<rgba16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				BlurProcess<rgba32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString( dstBitDepth ) << ") not recognized by the plugin." );
				break;
			}
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				BlurProcess<gray8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				BlurProcess<gray16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				BlurProcess<gray32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString( dstBitDepth ) << ") not recognized by the plugin." );
				break;
			}
		}
	}
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString( dstComponents ) << ") not supported by the plugin." );
	}
}

}
}
}
