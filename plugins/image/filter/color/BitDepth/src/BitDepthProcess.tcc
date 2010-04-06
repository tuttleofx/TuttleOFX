#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include "BitDepthPlugin.hpp"

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

#include "BitDepthDefinitions.hpp"


namespace tuttle {
namespace plugin {
namespace bitDepth {

using namespace boost::gil;

template<class SView, class DView>
BitDepthProcess<SView, DView>::BitDepthProcess( BitDepthPlugin &instance )
: ImageGilProcessor<DView>( instance )
, _plugin( instance )
{
}

template<class SView, class DView>
void BitDepthProcess<SView, DView>::setup( const OFX::RenderArguments &args )
{
	// destination view
	this->_dst.reset( _plugin.getDstClip( )->fetchImage( args.time ) );
	if( !this->_dst.get( ) )
		throw( ImageNotReadyException( ) );
	if( this->_dst->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_dstView = getView<DView>( this->_dst.get(), _plugin.getDstClip()->getPixelRod(args.time) );

	// source view
	_src.reset( _plugin.getSrcClip( )->fetchImage( args.time ) );
	if( !_src.get( ) )
		throw( ImageNotReadyException( ) );
	if( _src->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	this->_srcView = getView<SView>( _src.get(), _plugin.getSrcClip( )->getPixelRod(args.time) );
}

/**
 * @brief Function called by rendering thread each time 
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class SView, class DView>
void BitDepthProcess<SView, DView>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	SView src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );
	DView dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
							   procWindow.x2 - procWindow.x1,
							   procWindow.y2 - procWindow.y1 );
	
	copy_and_convert_pixels( src, dst );
}

}
}
}
