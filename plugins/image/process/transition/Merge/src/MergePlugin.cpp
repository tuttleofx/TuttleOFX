#include <tuttle/common/utils/global.hpp>

#include "MergePlugin.hpp"
#include "MergeProcess.hpp"
#include "MergeDefinitions.hpp"

#include <tuttle/common/math/rectOp.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

using namespace boost::gil;

MergePlugin::MergePlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_mergeFunction = fetchChoiceParam( kMergeFunction );
	_clipSrcA      = fetchClip( kMergeSourceA );
	_clipSrcB      = fetchClip( kMergeSourceB );
	_clipDst       = fetchClip( kOfxImageEffectOutputClipName );
	assert( _mergeFunction && _clipSrcA && _clipSrcB && _clipDst );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MergePlugin::render( const OFX::RenderArguments& args )
{
	bool isGray = _clipDst->getPixelComponents() == OFX::ePixelComponentAlpha;

	if( isGray )
	{
		switch( (EMergeFunction)_mergeFunction->getValue() )
		{
			// Functions that doesn't need alpha
			case eMergeFunctionAverage:
			{
				renderGray<FunctorAverage>( args );
				break;
			}
			case eMergeFunctionCopy:
			{
				renderGray<FunctorCopy>( args );
				break;
			}
			case eMergeFunctionDifference:
			{
				renderGray<FunctorDifference>( args );
				break;
			}
			case eMergeFunctionDivide:
			{
				renderGray<FunctorDivide>( args );
				break;
			}
			case eMergeFunctionExclusion:
			{
				renderGray<FunctorExclusion>( args );
				break;
			}
			case eMergeFunctionFrom:
			{
				renderGray<FunctorFrom>( args );
				break;
			}
			case eMergeFunctionGeometric:
			{
				renderGray<FunctorGeometric>( args );
				break;
			}
			case eMergeFunctionHardLight:
			{
				renderGray<FunctorHardLight>( args );
				break;
			}
			case eMergeFunctionHypot:
			{
				renderGray<FunctorHypot>( args );
				break;
			}
			case eMergeFunctionLighten:
			{
				renderGray<FunctorLighten>( args );
				break;
			}
			case eMergeFunctionDarken:
			{
				renderGray<FunctorDarken>( args );
				break;
			}
			case eMergeFunctionMinus:
			{
				renderGray<FunctorMinus>( args );
				break;
			}
			case eMergeFunctionMultiply:
			{
				renderGray<FunctorMultiply>( args );
				break;
			}
			case eMergeFunctionOverlay:
			{
				renderGray<FunctorOverlay>( args );
				break;
			}
			case eMergeFunctionPlus:
			{
				renderGray<FunctorPlus>( args );
				break;
			}
			case eMergeFunctionScreen:
			{
				renderGray<FunctorScreen>( args );
				break;
			}
			case eMergeFunctionPinLight:
			{
				renderGray<FunctorPinLight>( args );
				break;
			}
			case eMergeFunctionReflect:
			{
				// Quadratic mode: reflect
				renderGray<FunctorReflect>( args );
				break;
			}
			case eMergeFunctionFreeze:
			{
				// Quadratic mode: freeze
				renderGray<FunctorFreeze>( args );
				break;
			}
			case eMergeFunctionInterpolated:
			{
				// Similar to average, but smoother (and a lot slower)...
				renderGray<FunctorInterpolated>( args );
				break;
			}
			default:
				COUT_ERROR( "Unsupported operation !" );
				break;
		}
	}
	else
	{
		switch( (EMergeFunction)_mergeFunction->getValue() )
		{
			// Functions that need alpha
			case eMergeFunctionATop:
			{
				renderRGBA< FunctorATop >( args );
				break;
			}
			case eMergeFunctionColor:
			{
				renderRGBA<FunctorColor>( args );
				break;
			}
			case eMergeFunctionConjointOver:
			{
				renderRGBA<FunctorConjointOver>( args );
				break;
			}
			case eMergeFunctionColorBurn:
			{
				renderRGBA<FunctorColorBurn>( args );
				break;
			}
			case eMergeFunctionColorDodge:
			{
				renderRGBA<FunctorColorDodge>( args );
				break;
			}
			case eMergeFunctionDisjointOver:
			{
				renderRGBA<FunctorDisjointOver>( args );
				break;
			}
			case eMergeFunctionIn:
			{
				renderRGBA<FunctorIn>( args );
				break;
			}
			case eMergeFunctionMask:
			{
				renderRGBA<FunctorMask>( args );
				break;
			}
			case eMergeFunctionMatte:
			{
				renderRGBA<FunctorMatte>( args );
				break;
			}
			case eMergeFunctionOut:
			{
				renderRGBA<FunctorOut>( args );
				break;
			}
			case eMergeFunctionOver:
			{
				renderRGBA<FunctorOver>( args );
				break;
			}
			case eMergeFunctionStencil:
			{
				renderRGBA<FunctorStencil>( args );
				break;
			}
			case eMergeFunctionUnder:
			{
				renderRGBA<FunctorUnder>( args );
				break;
			}
			case eMergeFunctionXOR:
			{
				renderRGBA<FunctorXOR>( args );
				break;
			}
			// Functions that doesn't need alpha
			case eMergeFunctionAverage:
			{
				renderRGBA<FunctorAverage>( args );
				break;
			}
			case eMergeFunctionCopy:
			{
				renderRGBA<FunctorCopy>( args );
				break;
			}
			case eMergeFunctionDifference:
			{
				renderRGBA<FunctorDifference>( args );
				break;
			}
			case eMergeFunctionDivide:
			{
				renderRGBA<FunctorDivide>( args );
				break;
			}
			case eMergeFunctionExclusion:
			{
				renderRGBA<FunctorExclusion>( args );
				break;
			}
			case eMergeFunctionFrom:
			{
				renderRGBA<FunctorFrom>( args );
				break;
			}
			case eMergeFunctionGeometric:
			{
				renderRGBA<FunctorGeometric>( args );
				break;
			}
			case eMergeFunctionHardLight:
			{
				renderRGBA<FunctorHardLight>( args );
				break;
			}
			case eMergeFunctionHypot:
			{
				renderRGBA<FunctorHypot>( args );
				break;
			}
			case eMergeFunctionLighten:
			{
				renderRGBA<FunctorLighten>( args );
				break;
			}
			case eMergeFunctionDarken:
			{
				renderRGBA<FunctorDarken>( args );
				break;
			}
			case eMergeFunctionMinus:
			{
				renderRGBA<FunctorMinus>( args );
				break;
			}
			case eMergeFunctionMultiply:
			{
				renderRGBA<FunctorMultiply>( args );
				break;
			}
			case eMergeFunctionOverlay:
			{
				renderRGBA<FunctorOverlay>( args );
				break;
			}
			case eMergeFunctionPlus:
			{
				renderRGBA<FunctorPlus>( args );
				break;
			}
			case eMergeFunctionScreen:
			{
				renderRGBA<FunctorScreen>( args );
				break;
			}
			case eMergeFunctionPinLight:
			{
				renderRGBA<FunctorPinLight>( args );
				break;
			}
			case eMergeFunctionReflect:
			{
				// Quadratic mode: reflect
				renderRGBA<FunctorReflect>( args );
				break;
			}
			case eMergeFunctionFreeze:
			{
				// Quadratic mode: freeze
				renderRGBA<FunctorFreeze>( args );
				break;
			}
			case eMergeFunctionInterpolated:
			{
				// Similar to average, but smoother (and a lot slower)...
				renderRGBA<FunctorInterpolated>( args );
				break;
			}
			default:
				COUT_ERROR( "Unsupported operation !" );
				break;

		}

	}
}

template< template <typename> class Functor >
void MergePlugin::renderGray( const OFX::RenderArguments& args )
{
	assert( _clipDst );
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth();
	switch( dstBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			MergeProcess<gray8_view_t, Functor<typename gray8_view_t::value_type> > fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthUShort:
		{
			MergeProcess<gray16_view_t, Functor<typename gray16_view_t::value_type> > fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthFloat:
		{
			MergeProcess<gray32f_view_t, Functor<typename gray32f_view_t::value_type> > fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			COUT_FATALERROR( "BitDepthNone not recognize." );
			return;
	}
}

template< template <typename> class Functor >
void MergePlugin::renderRGBA( const OFX::RenderArguments& args )
{
	assert( _clipDst );
	OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth();

	// do the rendering
	switch( dstBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			MergeProcess<rgba8_view_t, Functor<rgba8_view_t::value_type> > fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthUShort:
		{
			MergeProcess<rgba16_view_t, Functor<rgba16_view_t::value_type> > fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthFloat:
		{
			MergeProcess<rgba32f_view_t, Functor<rgba32f_view_t::value_type> > fred( *this );
			fred.setupAndProcess( args );
			break;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
			COUT_FATALERROR( "BitDepthNone not recognize." );
			return;
	}
}

void MergePlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kMergeHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kMergeHelpString );
	}
}

bool MergePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	const OfxRectD irod = rectanglesIntersection( _clipSrcA->getCanonicalRod( args.time ),
	                                              _clipSrcB->getCanonicalRod( args.time ) );

	// Intersection of A & B
	rod.x1 = irod.x1;
	rod.x2 = irod.x2;
	rod.y1 = irod.y1;
	rod.y2 = irod.y2;
	return true;
}

}
}
}
