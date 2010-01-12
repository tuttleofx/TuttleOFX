/**
 * @file Log2LinProcess.tcc
 * @brief
 * @author
 * @date    08/01/10 15:39
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


namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

using namespace boost::gil;

static const float cineonBlackOffset = powf(10.0f, (95.0f-685.0f) / 300.0f);

inline float fromCineon(float x)
{
	return (powf(10.0f,(1023.0f*x-685.0f)/300.0f)-cineonBlackOffset)/(1.0f-cineonBlackOffset);
}

struct log2liner
{
	inline rgba32f_pixel_t operator()( const rgba32f_pixel_t& p ) const
	{
		rgba32f_pixel_t p2;
		for( int n = 0; n < 3; ++n )
		{
			p2[n] = fromCineon(p[n]);
		}
		p2[3] = p[3];
		return p2;
	}
};

template<class View>
Log2LinProcess<View>::Log2LinProcess( Log2LinPlugin &instance )
: tuttle::plugin::ImageGilProcessor<View>( instance )
, tuttle::plugin::Progress( instance )
, _plugin( instance )
{
}

template<class View>
void Log2LinProcess<View>::setupAndProcess( const OFX::RenderArguments &args )
{
    try
    {
        boost::scoped_ptr<OFX::Image> src( _plugin.getSrcClip( )->fetchImage( args.time ) );
        if( !src.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI sBounds = src->getBounds( );
        OFX::BitDepthEnum srcBitDepth = src->getPixelDepth( );
        OFX::PixelComponentEnum srcComponents = src->getPixelComponents( );

        // Build source view
        this->_srcView = interleaved_view( std::abs( sBounds.x2 - sBounds.x1 ), std::abs( sBounds.y2 - sBounds.y1 ),
                                           static_cast < value_t* > ( src->getPixelData( ) ),
                                           src->getRowBytes( ) );

        boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time ) );
        if( !dst.get( ) )
            throw( ImageNotReadyException( ) );
        OfxRectI dBounds = dst->getBounds( );
        OFX::BitDepthEnum dstBitDepth = dst->getPixelDepth( );
        OFX::PixelComponentEnum dstComponents = dst->getPixelComponents( );

        // Make sure bit depths are same
        if( srcBitDepth != dstBitDepth || srcComponents != dstComponents )
        {
            throw( BitDepthMismatchException( ) );
        }

        // Build destination view
        this->_dstView = interleaved_view( std::abs( dBounds.x2 - dBounds.x1 ), std::abs( dBounds.y2 - dBounds.y1 ),
                                           static_cast < value_t* > ( dst->getPixelData( ) ),
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
template<class View>
void Log2LinProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
    try
    {
        View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
                                  procWindow.x2 - procWindow.x1,
                                  procWindow.y2 - procWindow.y1 );
        View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
                                  procWindow.x2 - procWindow.x1,
                                  procWindow.y2 - procWindow.y1 );
        // Put your computations here
        transform_pixels( color_converted_view<rgba32f_pixel_t>(src),  color_converted_view<rgba32f_pixel_t>(dst), log2liner() );
    }
    catch( PluginException& e )
    {
        COUT_EXCEPTION( e );
    }
}

}
}
}
}
