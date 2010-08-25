/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "FftPlugin.hpp"
#include "FftProcess.hpp"
#include "FftDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

using namespace boost::gil;

FftPlugin::FftPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_fftType = fetchChoiceParam( kFftType );
	_fftTypeLong = fetchChoiceParam( kFftTypeLong );
}

OFX::Clip* FftPlugin::getSrcClip( ) const
{
    return _clipSrc;
}

OFX::Clip* FftPlugin::getDstClip( ) const
{
    return _clipDst;
}

FftTransformProcessParams FftPlugin::getProcessParams() const
{
	FftTransformProcessParams params;
	params._type = (EfftType)_fftType->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FftPlugin::render( const OFX::RenderArguments &args )
{
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
                FftProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FftProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FftProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthNone :
                COUT_FATALERROR( "BitDepthNone not recognize." );
                return;
            case OFX::eBitDepthCustom :
                COUT_FATALERROR( "BitDepthCustom not recognize." );
                return;
        }
    }
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                FftProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FftProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FftProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthNone :
                COUT_FATALERROR( "BitDepthNone not recognize." );
                return;
            case OFX::eBitDepthCustom :
                COUT_FATALERROR( "BitDepthCustom not recognize." );
                return;
        }
    }
}

void FftPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kHelpString );
    }
	else if( paramName == kFftTypeLong && args.reason == OFX::eChangeUserEdit )
	{
		_fftType->setValue( _fftTypeLong->getValue() );
	}
	else if( paramName == kFftType && args.reason == OFX::eChangeUserEdit )
	{
		_fftTypeLong->setValue( _fftType->getValue() );
	}
}

}
}
}
}
