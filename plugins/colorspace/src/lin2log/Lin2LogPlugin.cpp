/**
 * @file Lin2LogPlugin.cpp
 * @brief
 * @author
 * @date    08/01/10 15:19
 *
 */

#include "Lin2LogPlugin.hpp"
#include "Lin2LogProcess.hpp"
#include "Lin2LogDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

using namespace boost::gil;

Lin2LogPlugin::Lin2LogPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
}

OFX::Clip * Lin2LogPlugin::getSrcClip( ) const
{
    return _srcClip;
}

OFX::Clip * Lin2LogPlugin::getDstClip( ) const
{
    return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Lin2LogPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth( );
    OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                Lin2LogProcess<rgba8_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                Lin2LogProcess<rgba16_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                Lin2LogProcess<rgba32f_view_t> fred( *this );
                fred.setupAndProcess( args );
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
                Lin2LogProcess<gray8_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                Lin2LogProcess<gray16_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                Lin2LogProcess<gray32f_view_t> fred( *this );
                fred.setupAndProcess( args );
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

void Lin2LogPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kLin2LogHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kLin2LogHelpString );
    }
}

}
}
}
}
