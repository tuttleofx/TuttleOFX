#include "HudPlugin.hpp"
#include "HudProcess.hpp"
#include "HudDefinitions.hpp"
#include <terry/merge/MergeFunctors.hpp>



namespace tuttle {
namespace plugin {
namespace hud {


HudPlugin::HudPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
//    _clipSrcMatte = fetchClip( kClipMatte );
	_color = fetchRGBAParam( kConstantColor );
	_paramMerge = fetchChoiceParam( kParamFunction );

}

void HudPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
//    if( paramName == kParamHelpButton )
//    {
//        sendMessage( OFX::Message::eMessageMessage,
//                     "", // No XML resources
//                     kParamHelpString );
//    }
}

//bool HudPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
//{
//	HudProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	switch( params._border )
//	{
//		case eParamBorderPadded:
//			rod.x1 = srcRod.x1 + 1;
//			rod.y1 = srcRod.y1 + 1;
//			rod.x2 = srcRod.x2 - 1;
//			rod.y2 = srcRod.y2 - 1;
//			return true;
//		default:
//			break;
//	}
//	return false;
//}
//
//void HudPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
//{
//	HudProcessParams<Scalar> params = getProcessParams();
//	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
//	OfxRectD srcRoi;
//	srcRoi.x1 = srcRod.x1 - 1;
//	srcRoi.y1 = srcRod.y1 - 1;
//	srcRoi.x2 = srcRod.x2 + 1;
//	srcRoi.y2 = srcRod.y2 + 1;
//	rois.setRegionOfInterest( *_clipSrc, srcRoi );
//}

bool HudPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	HudProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}


void HudPlugin::render( const OFX::RenderArguments &args )
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
void HudPlugin::render( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	using namespace terry;
	typedef typename View::value_type Pixel;
	EParamMerge merge = static_cast<EParamMerge>(_paramMerge->getValue());
	switch( merge )
	{
		// Functions that need alpha
		case eParamMergeATop:
		{
			render< View, FunctorATop >( args );
			break;
		}
		// @todo : Build pb here
		// case eParamMergeColor:
		// {
		// 	render< View, FunctorColor >( args );
		// 	break;
		// }
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
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Mode (" + merge + ") not supported by the plugin." );
	}
}


template< class View, template <typename> class Functor >
void HudPlugin::render_if( const OFX::RenderArguments& args, boost::mpl::true_ )
{
	typedef typename View::value_type Pixel;
	HudProcess<View, Functor<Pixel> > p( *this );
	p.setupAndProcess( args );
}

template< class View, template <typename> class Functor >
void HudPlugin::render_if( const OFX::RenderArguments& args, boost::mpl::false_ )
{
	BOOST_THROW_EXCEPTION( exception::Unsupported()
		<< exception::user() + "Need an alpha channel for this Merge operation." );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
template< class View, template <typename> class Functor >
void HudPlugin::render( const OFX::RenderArguments &args )
{
	typedef typename View::value_type Pixel;
	typedef typename boost::gil::contains_color< typename View::value_type, boost::gil::alpha_t>::type has_alpha_t;
	typedef typename boost::is_same<typename Functor<Pixel>::operating_mode_t, terry::merge_per_channel_with_alpha>::type merge_need_alpha_t;
	typedef typename boost::mpl::if_<merge_need_alpha_t, has_alpha_t, boost::mpl::true_>::type render_condition_t;

	render_if<View, Functor>( args, render_condition_t() );
}
}
}
}
