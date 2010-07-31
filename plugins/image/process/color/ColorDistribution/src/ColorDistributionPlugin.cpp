#include "ColorDistributionPlugin.hpp"
#include "ColorDistributionProcess.hpp"
#include "ColorDistributionDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {


ColorDistributionPlugin::ColorDistributionPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramIn = fetchChoiceParam( kParamIn );
	_paramOut = fetchChoiceParam( kParamOut );
}

ColorDistributionProcessParams<ColorDistributionPlugin::Scalar> ColorDistributionPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ColorDistributionProcessParams<Scalar> params;
	params._in = static_cast<EParamDistribution>( _paramIn->getValue() );
	params._out = static_cast<EParamDistribution>( _paramOut->getValue() );
	return params;
}

void ColorDistributionPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamInvert )
	{
		int in = _paramIn->getValue();
		_paramIn->setValue( _paramOut->getValue() );
		_paramOut->setValue( in );
	}
}

bool ColorDistributionPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	ColorDistributionProcessParams<Scalar> params = getProcessParams();
	if( params._in == params._out )
	{
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
	return false; // by default, we are not an identity operation
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorDistributionPlugin::render( const OFX::RenderArguments &args )
{
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum dstBitDepth = _clipDst->getPixelDepth( );
    OFX::PixelComponentEnum dstComponents = _clipDst->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                ColorDistributionProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                ColorDistributionProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                ColorDistributionProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToStr(dstBitDepth) << ") not recognized by the plugin." );
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
                ColorDistributionProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                ColorDistributionProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                ColorDistributionProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToStr(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToStr(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
