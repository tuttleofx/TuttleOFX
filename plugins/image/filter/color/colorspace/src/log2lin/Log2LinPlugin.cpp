/**
 * @file Log2LinPlugin.cpp
 * @brief
 * @author
 * @date    08/01/10 15:39
 *
 */

#include "Log2LinPlugin.hpp"
#include "Log2LinProcess.hpp"
#include "Log2LinDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

using namespace boost::gil;

Log2LinPlugin::Log2LinPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
}

OFX::Clip * Log2LinPlugin::getSrcClip( ) const
{
    return _srcClip;
}

OFX::Clip * Log2LinPlugin::getDstClip( ) const
{
    return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Log2LinPlugin::render( const OFX::RenderArguments &args )
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
                Log2LinProcess<rgba8_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                Log2LinProcess<rgba16_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                Log2LinProcess<rgba32f_view_t> fred( *this );
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
                Log2LinProcess<gray8_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                Log2LinProcess<gray16_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                Log2LinProcess<gray32f_view_t> fred( *this );
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

void Log2LinPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kLog2LinHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kLog2LinHelpString );
    }
}

}
}
}
}
