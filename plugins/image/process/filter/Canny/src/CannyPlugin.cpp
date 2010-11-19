#include "CannyPlugin.hpp"
#include "CannyProcess.hpp"
#include "CannyDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace canny {


CannyPlugin::CannyPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramHysteresis = fetchBooleanParam( kParamHysteresis );
	_paramUpperThres = fetchDoubleParam( kParamUpperThres );
	_paramLowerThres = fetchDoubleParam( kParamLowerThres );
	_paramFillAllChannels = fetchBooleanParam( kParamFillAllChannels );
	_paramBorder = fetchChoiceParam( kParamBorder );
}

CannyProcessParams<CannyPlugin::Scalar> CannyPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	CannyProcessParams<Scalar> params;

	params._hysteresis = _paramHysteresis->getValue();
	params._upperThres = _paramUpperThres->getValue();
	params._lowerThres = std::min( _paramLowerThres->getValue(), params._upperThres );

	params._fillAllChannels = _paramFillAllChannels->getValue();

	params._border     = static_cast<EParamBorder>( _paramBorder->getValue() );

	return params;
}

void CannyPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
//    if( paramName == kParamHelpButton )
//    {
//        sendMessage( OFX::Message::eMessageMessage,
//                     "", // No XML resources
//                     kParamHelpString );
//    }
}

bool CannyPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	CannyProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	switch( params._border )
	{
		case eParamBorderPadded:
			rod.x1 = srcRod.x1 + 1;
			rod.y1 = srcRod.y1 + 1;
			rod.x2 = srcRod.x2 - 1;
			rod.y2 = srcRod.y2 - 1;
			return true;
		default:
			break;
	}
	return false;
}

void CannyPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	CannyProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
	srcRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}


bool CannyPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	CannyProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void CannyPlugin::render( const OFX::RenderArguments &args )
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
                CannyProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                CannyProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                CannyProcess<rgba32f_view_t> p( *this );
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
//                CannyProcess<gray8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                CannyProcess<gray16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
//                CannyProcess<gray32f_view_t> p( *this );
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
