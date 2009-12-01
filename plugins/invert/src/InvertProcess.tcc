/**
 * @file InvertProcess.tcc
 * @brief
 * @author
 * @date    01/10/09 12:01
 *
 */

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

#include <cmath>
#include <vector>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

struct inverter
{
    template< class P>
    P operator( ) ( const P& p ) const
    {
        P p2;
        // @TODO_OFX Improve this: Don't consider alpha
        // http://stlab.adobe.com/gil/html/color__convert_8hpp-source.html
        // typedef typename channel_type<P2>::type T2;
        // channel_convert<T2>(alpha_or_max(src))
        // mpl::contains<typename color_space_type<Pixel>::type,alpha_t>() -> mpl::true_ ?
        const int nc = std::min((int)num_channels<P>::type::value, 3);
        for( int n = 0; n < nc; ++n ) {
            p2[n] = channel_traits< typename channel_type< P >::type >::max_value( ) - p[n] +
                    channel_traits< typename channel_type< P >::type >::min_value( );
        }
        if ( num_channels<P>::type::value > 3 )
            p2[3] = p[3];
        return p2;
    }
};

using namespace boost::gil;

template<class View>
InvertProcess<View>::InvertProcess( InvertPlugin &instance )
: tuttle::plugin::ImageGilProcessor<View>( instance )
, tuttle::plugin::Progress( instance )
, _plugin( instance )
{
}

template<class View>
void InvertProcess<View>::setupAndProcess( const OFX::RenderArguments &args )
{
    try
    {
        boost::scoped_ptr<OFX::Image> src( _plugin.getSrcClip( )->fetchImage( args.time ) );
        if( !src.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI sBounds = src->getBounds();

        OFX::BitDepthEnum srcBitDepth = src->getPixelDepth( );
        OFX::PixelComponentEnum srcComponents = src->getPixelComponents( );

        // Build destination view
        this->_srcView = interleaved_view( std::abs(sBounds.x2 - sBounds.x1), std::abs(sBounds.y2 - sBounds.y1),
                                           static_cast<value_t*>( src->getPixelData() ),
                                           src->getRowBytes() );
        boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time ) );
        if( !dst.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI dBounds = dst->getBounds();
        OFX::BitDepthEnum dstBitDepth = dst->getPixelDepth( );
        OFX::PixelComponentEnum dstComponents = dst->getPixelComponents( );

        // Make sure bit depths are same
        if( srcBitDepth != dstBitDepth || srcComponents != dstComponents )
        {
            throw(BitDepthMismatchException( ) );
        }

        // Build destination view
        this->_dstView = interleaved_view( std::abs(dBounds.x2 - dBounds.x1), std::abs(dBounds.y2 - dBounds.y1),
                                           static_cast<value_t*>( dst->getPixelData() ),
                                           dst->getRowBytes( ) );

        // Set the render window
        this->setRenderWindow( args.renderWindow );
        // Call the base class process member
        this->process();
    }
    catch( PluginException e )
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
template<class View>
void InvertProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
    try
    {
        // Invert pixels
        View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
                                  procWindow.x2 - procWindow.x1,
                                  procWindow.y2 - procWindow.y1 );
        View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
                                  procWindow.x2 - procWindow.x1,
                                  procWindow.y2 - procWindow.y1 );

        transform_pixels( src, dst, inverter() );
    }
    catch( PluginException err )
    {
        COUT_EXCEPTION( err );
    }
}

}
}
}
