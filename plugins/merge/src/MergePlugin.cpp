/**
 * @file MergePlugin.cpp
 * @brief Merging ofx plugin
 * @author Eloi Du Bois
 * @date    27/11/09 13:42
 *
 */

#include "MergePlugin.hpp"
#include "MergeProcess.hpp"
#include "MergeDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
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
    _srcClipA = fetchClip( kMergeSourceA );
    _srcClipB = fetchClip( kMergeSourceB );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
    assert(_srcClipA && _srcClipB && _dstClip);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void MergePlugin::render( const OFX::RenderArguments &args )
{
    assert(_dstClip);
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
                MergeProcess<rgba8_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                MergeProcess<rgba16_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                MergeProcess<rgba32f_view_t> fred( *this );
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
                MergeProcess<gray8_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                MergeProcess<gray16_view_t> fred( *this );
                fred.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                MergeProcess<gray32f_view_t> fred( *this );
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

void MergePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kMergeHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kMergeHelpString );
    }
}

}
}
}
