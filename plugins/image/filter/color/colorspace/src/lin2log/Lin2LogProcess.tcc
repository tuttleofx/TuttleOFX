#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>


namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

using namespace boost::gil;

static const float cineonBlackOffset = powf(10.0f, (95.0f-685.0f) / 300.0f);

inline float toCineon(float v)
{
	float t = v*(1.0f-cineonBlackOffset)+cineonBlackOffset;
	if (t <= 0.0f) return -std::numeric_limits<float>::infinity();
	return (log10f(t)*300.0f+685.0f)/1023.0f;
}

struct lin2loger
{
	inline rgba32f_pixel_t operator()( const rgba32f_pixel_t& p ) const
	{
		rgba32f_pixel_t p2;
		for( int n = 0; n < 3; ++n )
		{
			p2[n] = toCineon(p[n]);
		}
		p2[3] = p[3];
		return p2;
	}
};

template<class View>
Lin2LogProcess<View>::Lin2LogProcess( Lin2LogPlugin &instance )
: ImageGilProcessor<View>( instance )
, _plugin( instance )
{
}

template<class View>
void Lin2LogProcess<View>::setup( const OFX::RenderArguments &args )
{
	// source view
	boost::scoped_ptr<OFX::Image> src( _plugin.getSrcClip( )->fetchImage( args.time ) );
	if( !src.get( ) )
		throw( ImageNotReadyException( ) );
	_srcView = this->getView( src.get(), _plugin.getSrcClip()->getPixelRod(args.time) );

	// destination view
	boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip( )->fetchImage( args.time ) );
	if( !dst.get( ) )
		throw( ImageNotReadyException( ) );
	this->_dstView = this->getView( dst.get(), _plugin.getDstClip()->getPixelRod(args.time) );

	// Make sure bit depths are same
	if( src->getPixelDepth( ) != dst->getPixelDepth() ||
	    src->getPixelComponents( ) != dst->getPixelComponents( ) )
		throw( BitDepthMismatchException( ) );
}

/**
 * @brief Function called by rendering thread each time 
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void Lin2LogProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	// Put your computations here
	transform_pixels( color_converted_view<rgba32f_pixel_t>(src),  color_converted_view<rgba32f_pixel_t>(dst), lin2loger() );
}

}
}
}
}
