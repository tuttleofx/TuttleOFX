#include "ThinningPlugin.hpp"
#include "ThinningProcess.hpp"
#include "ThinningDefinitions.hpp"

#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {


ThinningPlugin::ThinningPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramBorder = fetchChoiceParam( kParamBorder );
}

ThinningProcessParams<ThinningPlugin::Scalar> ThinningPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ThinningProcessParams<Scalar> params;
	params._border = static_cast<EParamBorder>( _paramBorder->getValue() );
	return params;
}

bool ThinningPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ThinningProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	switch( params._border )
	{
		case eParamBorderPadded:
			rod = rectangleReduce( srcRod, 2 );
			return true;
		default:
			break;
	}
	return false;
}

void ThinningPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	ThinningProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	
	OfxRectD srcRoi = rectangleGrow( srcRod, 2 );

	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}


/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ThinningPlugin::render( const OFX::RenderArguments &args )
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
                ThinningProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                ThinningProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                ThinningProcess<rgba32f_view_t> p( *this );
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
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                ThinningProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                ThinningProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                ThinningProcess<gray32f_view_t> p( *this );
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
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
