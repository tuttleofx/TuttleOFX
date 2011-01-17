#include "MergePlugin.hpp"
#include "MergeProcess.hpp"
#include "MergeDefinitions.hpp"

#include "gil/toolbox/MergeFunctors.hpp"

#include <tuttle/common/math/rectOp.hpp>

#include <boost/gil/gil_all.hpp>

#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

MergePlugin::MergePlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
{
	_clipSrcA      = fetchClip( kParamSourceA );
	_clipSrcB      = fetchClip( kParamSourceB );
	_clipDst       = fetchClip( kOfxImageEffectOutputClipName );

	_paramMerge = fetchChoiceParam( kParamFunction );
	_paramOffsetA = fetchInt2DParam( kParamOffsetA );
	_paramOffsetB = fetchInt2DParam( kParamOffsetB );
	_paramRod = fetchChoiceParam( kParamRod );
}

MergeProcessParams<MergePlugin::Scalar> MergePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	MergeProcessParams<Scalar> params;

	params._rod = static_cast<EParamRod>( _paramRod->getValue() );

	OfxPointI offsetA = _paramOffsetA->getValue();
	params._offsetA.x = offsetA.x / renderScale.x;
	params._offsetA.y = offsetA.y / renderScale.y;

	OfxPointI offsetB = _paramOffsetB->getValue();
	params._offsetB.x = offsetB.x / renderScale.x;
	params._offsetB.y = offsetB.y / renderScale.y;

	return params;
}

//void MergePlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
//{
//}

bool MergePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	MergeProcessParams<Scalar> params = getProcessParams( args.renderScale );

	OfxRectD srcRodA = translateRegion( _clipSrcA->getCanonicalRod( args.time ), params._offsetA );
	OfxRectD srcRodB = translateRegion( _clipSrcB->getCanonicalRod( args.time ), params._offsetB );
	
	switch( params._rod )
	{
		case eParamRodIntersect:
		{
			rod = rectanglesIntersection( srcRodA, srcRodB );
			return true;
		}
		case eParamRodUnion:
		{
			rod = rectanglesBoundingBox( srcRodA, srcRodB );
			return true;
		}
		case eParamRodA:
		{
			rod = srcRodA;
			return true;
		}
		case eParamRodB:
		{
			rod = srcRodB;
			return true;
		}
	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MergePlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent components = _clipDst->getPixelComponents( );

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			switch( bitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					render<rgba8_view_t>(args);
					return;
				}
				case OFX::eBitDepthUShort:
				{
					render<rgba16_view_t>(args);
					return;
				}
				case OFX::eBitDepthFloat:
				{
					render<rgba32f_view_t>(args);
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
				}
			}
		}
		case OFX::ePixelComponentRGB:
		{
			switch( bitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					render<rgb8_view_t>(args);
					return;
				}
				case OFX::eBitDepthUShort:
				{
					render<rgb16_view_t>(args);
					return;
				}
				case OFX::eBitDepthFloat:
				{
					render<rgb32f_view_t>(args);
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
				}
			}
		}
		case OFX::ePixelComponentAlpha:
		{
			switch( bitDepth )
			{
				case OFX::eBitDepthUByte :
				{
					render<gray8_view_t>(args);
					return;
				}
				case OFX::eBitDepthUShort :
				{
					render<gray16_view_t>(args);
					return;
				}
				case OFX::eBitDepthFloat :
				{
					render<gray32f_view_t>(args);
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
				}
			}
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< class View >
void MergePlugin::render( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
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
//				TUTTLE_COUT_FATALERROR( "Need an alpha channel for this Merge operation." );
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
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::user() + "Unrecognized merge operation (" + merge + ")." );
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
	BOOST_THROW_EXCEPTION( exception::Unsupported()
		<< exception::user() + "Need an alpha channel for this Merge operation." );
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
