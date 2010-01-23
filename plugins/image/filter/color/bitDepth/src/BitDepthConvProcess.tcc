/**
 * @file BitDepthConvProcess.tcc
 * @brief
 * @author
 * @date    08/01/10 17:46
 *
 */


#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

#include "BitDepthConvDefinitions.hpp"


namespace tuttle {
namespace plugin {
namespace bitDepthConvert {

using namespace boost::gil;

template<class SView, class DView>
BitDepthConvProcess<SView, DView>::BitDepthConvProcess( BitDepthConvPlugin &instance )
: tuttle::plugin::ImageGilProcessor<DView>( instance )
, tuttle::plugin::Progress( instance )
, _plugin( instance )
{
}

template<class SView, class DView>
void BitDepthConvProcess<SView, DView>::setupAndProcess( const OFX::RenderArguments &args )
{
    try
    {
        boost::scoped_ptr<OFX::Image> src( _plugin.getSrcClip( )->fetchImage( args.time ) );
        if( !src.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI sBounds = src->getBounds( );

        // Build source view
        this->_srcView = interleaved_view( std::abs( sBounds.x2 - sBounds.x1 ), std::abs( sBounds.y2 - sBounds.y1 ),
                                           static_cast < svalue_t* > ( src->getPixelData( ) ),
                                           src->getRowBytes( ) );

        boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time ) );
        if( !dst.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI dBounds = dst->getBounds( );

        // Build destination view
        this->_dstView = interleaved_view( std::abs( dBounds.x2 - dBounds.x1 ), std::abs( dBounds.y2 - dBounds.y1 ),
                                           static_cast < dvalue_t* > ( dst->getPixelData( ) ),
                                           dst->getRowBytes( ) );

        // Set the render window
        this->setRenderWindow( args.renderWindow );
        // Call the base class process member
        this->process( );
    }
    catch( PluginException& e )
    {
        COUT_EXCEPTION( e );
    }
}

/**
 * @brief Function called by rendering thread each time 
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class SView, class DView>
void BitDepthConvProcess<SView, DView>::multiThreadProcessImages( OfxRectI procWindow )
{
    try
    {
        SView src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
								   procWindow.x2 - procWindow.x1,
                                   procWindow.y2 - procWindow.y1 );
        DView dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
                                   procWindow.x2 - procWindow.x1,
                                   procWindow.y2 - procWindow.y1 );
        // Put your computations here
        copy_and_convert_pixels( src, dst );
    }
    catch( PluginException& e )
    {
        COUT_EXCEPTION( e );
    }
}

}
}
}
