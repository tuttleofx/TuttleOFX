#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
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
namespace checkerboard {


template<class View>
CheckerboardProcess<View>::CheckerboardProcess( CheckerboardPlugin &instance )
: ImageGilProcessor<View>( instance )
, _plugin( instance )
{
}

template<class View>
void CheckerboardProcess<View>::setup( const OFX::RenderArguments &args )
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
void CheckerboardProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	
	// Put your computations here
	View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							  procWindow.x2 - procWindow.x1,
							  procWindow.y2 - procWindow.y1 );
	copy_pixels( src, dst );
}

}
}
}
