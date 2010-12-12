#include <tuttle/common/utils/global.hpp>

#include "MergePlugin.hpp"
#include "MergeProcess.hpp"
#include "MergeDefinitions.hpp"

#include <tuttle/common/math/rectOp.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

using namespace boost::gil;

MergePlugin::MergePlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipSrcA      = fetchClip( kMergeSourceA );
	_clipSrcB      = fetchClip( kMergeSourceB );
	_clipDst       = fetchClip( kOfxImageEffectOutputClipName );

	_paramMerge = fetchChoiceParam( kMergeFunction );
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
	// Intersection of A & B
	rod = rectanglesIntersection( _clipSrcA->getCanonicalRod( args.time ),
	                              _clipSrcB->getCanonicalRod( args.time ) );
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MergePlugin::render( const OFX::RenderArguments &args )
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
            case OFX::eBitDepthUByte :
            {
                render<rgba8_view_t>(args);
                break;
            }
            case OFX::eBitDepthUShort :
            {
                render<rgba16_view_t>(args);
                break;
            }
            case OFX::eBitDepthFloat :
            {
                render<rgba32f_view_t>(args);
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                render<gray8_view_t>(args);
                break;
            }
            case OFX::eBitDepthUShort :
            {
                render<gray16_view_t>(args);
                break;
            }
            case OFX::eBitDepthFloat :
            {
                render<gray32f_view_t>(args);
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


template< class View >
void MergePlugin::render( const OFX::RenderArguments& args )
{
	typedef typename View::value_type Pixel;
	EParamMerge merge = static_cast<EParamMerge>(_paramMerge->getValue());

//	if( ! boost::gil::contains_color<Pixel, boost::gil::alpha_t>::value )
//	{
//		// Functions that need alpha
//		switch( merge )
//		{
//			case eParamMergeATop:
//			case eParamMergeColor:
//			case eParamMergeConjointOver:
//			case eParamMergeColorBurn:
//			case eParamMergeColorDodge:
//			case eParamMergeDisjointOver:
//			case eParamMergeIn:
//			case eParamMergeMask:
//			case eParamMergeMatte:
//			case eParamMergeOut:
//			case eParamMergeOver:
//			case eParamMergeStencil:
//			case eParamMergeUnder:
//			case eParamMergeXOR:
//				COUT_FATALERROR( "Need an alpha channel for this Merge operation." );
//				return;
//			default:
//				break;
//		}
//	}
	switch( merge )
	{
		// Functions that need alpha
		case eParamMergeATop:
		{
			render< View, FunctorATop >( args );
			break;
		}
		case eParamMergeColor:
		{
			render< View, FunctorColor >( args );
			break;
		}
		case eParamMergeConjointOver:
		{
			render< View, FunctorConjointOver >( args );
			break;
		}
		case eParamMergeColorBurn:
		{
			render< View, FunctorColorBurn >( args );
			break;
		}
		case eParamMergeColorDodge:
		{
			render< View, FunctorColorDodge >( args );
			break;
		}
		case eParamMergeDisjointOver:
		{
			render< View, FunctorDisjointOver >( args );
			break;
		}
		case eParamMergeIn:
		{
			render< View, FunctorIn >( args );
			break;
		}
		case eParamMergeMask:
		{
			render< View, FunctorMask >( args );
			break;
		}
		case eParamMergeMatte:
		{
			render< View, FunctorMatte >( args );
			break;
		}
		case eParamMergeOut:
		{
			render< View, FunctorOut >( args );
			break;
		}
		case eParamMergeOver:
		{
			render< View, FunctorOver >( args );
			break;
		}
		case eParamMergeStencil:
		{
			render< View, FunctorStencil >( args );
			break;
		}
		case eParamMergeUnder:
		{
			render< View, FunctorUnder >( args );
			break;
		}
		case eParamMergeXOR:
		{
			render< View, FunctorXOR >( args );
			break;
		}
		// Functions that doesn't need alpha
		case eParamMergeAverage:
		{
			render< View, FunctorAverage >( args );
			break;
		}
		case eParamMergeCopy:
		{
			render< View, FunctorCopy >( args );
			break;
		}
		case eParamMergeDifference:
		{
			render< View, FunctorDifference >( args );
			break;
		}
		case eParamMergeDivide:
		{
			render< View, FunctorDivide >( args );
			break;
		}
		case eParamMergeExclusion:
		{
			render< View, FunctorExclusion >( args );
			break;
		}
		case eParamMergeFrom:
		{
			render< View, FunctorFrom >( args );
			break;
		}
		case eParamMergeGeometric:
		{
			render< View, FunctorGeometric >( args );
			break;
		}
		case eParamMergeHardLight:
		{
			render< View, FunctorHardLight >( args );
			break;
		}
		case eParamMergeHypot:
		{
			render< View, FunctorHypot >( args );
			break;
		}
		case eParamMergeLighten:
		{
			render< View, FunctorLighten >( args );
			break;
		}
		case eParamMergeDarken:
		{
			render< View, FunctorDarken >( args );
			break;
		}
		case eParamMergeMinus:
		{
			render< View, FunctorMinus >( args );
			break;
		}
		case eParamMergeMultiply:
		{
			render< View, FunctorMultiply >( args );
			break;
		}
		case eParamMergeOverlay:
		{
			render< View, FunctorOverlay >( args );
			break;
		}
		case eParamMergePlus:
		{
			render< View, FunctorPlus >( args );
			break;
		}
		case eParamMergeScreen:
		{
			render< View, FunctorScreen >( args );
			break;
		}
		case eParamMergePinLight:
		{
			render< View, FunctorPinLight >( args );
			break;
		}
		case eParamMergeReflect:
		{
			// Quadratic mode: reflect
			render< View, FunctorReflect >( args );
			break;
		}
		case eParamMergeFreeze:
		{
			// Quadratic mode: freeze
			render< View, FunctorFreeze >( args );
			break;
		}
		case eParamMergeInterpolated:
		{
			// Similar to average, but smoother (and a lot slower)...
			render< View, FunctorInterpolated >( args );
			break;
		}
		default:
		{
			COUT_ERROR( "Unrecognized merge operation (" << merge << ")." );
			break;
		}
	}
}

template< class View, template <typename> class Functor >
void MergePlugin::render_if( const OFX::RenderArguments& args, boost::mpl::true_ )
{
	typedef typename View::value_type Pixel;
	MergeProcess<View, Functor<Pixel> > p( *this );
	p.setupAndProcess( args );
}

template< class View, template <typename> class Functor >
void MergePlugin::render_if( const OFX::RenderArguments& args, boost::mpl::false_ )
{
	COUT_FATALERROR( "Need an alpha channel for this Merge operation." );
}

template< class View, template <typename> class Functor >
void MergePlugin::render( const OFX::RenderArguments& args )
{
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::contains_color< typename View::value_type, boost::gil::alpha_t>::type has_alpha_t;
	typedef typename boost::is_same<typename Functor<Pixel>::operating_mode_t, boost::gil::merge_per_channel_with_alpha>::type merge_need_alpha_t;
	typedef typename boost::mpl::if_<merge_need_alpha_t, has_alpha_t, boost::mpl::true_>::type render_condition_t;

	render_if<View, Functor>( args, render_condition_t() );
}

}
}
}
