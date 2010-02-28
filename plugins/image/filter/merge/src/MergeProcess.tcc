#include "gil/toolbox/hsl.hpp"
#include "gil/toolbox/ViewsMerging.hpp"
#include <tuttle/common/math/rectOp.hpp>
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
namespace merge {

using namespace boost::gil;

template<class View, class Functor>
MergeProcess<View, Functor>::MergeProcess( MergePlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{}

template<class View, class Functor>
void MergeProcess<View, Functor>::setup( const OFX::RenderArguments& args )
{
	// sources view
	// clip A
	boost::scoped_ptr<OFX::Image> srcA( _plugin.getSrcClipA()->fetchImage( args.time ) );
	if( !srcA.get() )
		throw( ImageNotReadyException() );
	this->_srcViewA = this->getView( srcA.get(), _plugin.getSrcClipA()->getPixelRod(args.time) );
	// clip B
	boost::scoped_ptr<OFX::Image> srcB( _plugin.getSrcClipB()->fetchImage( args.time ) );
	if( !srcB.get() )
		throw( ImageNotReadyException() );
	this->_srcViewB = this->getView( srcB.get(), _plugin.getSrcClipB()->getPixelRod(args.time) );
	// destination view
	boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time ) );
	if( !dst.get() )
		throw( ImageNotReadyException() );
	this->_dstView = this->getView( dst.get(), _plugin.getDstClip()->getPixelRod(args.time) );

	// Make sure bit depths are the same
	if( srcA->getPixelDepth() != dst->getPixelDepth() ||
	    srcB->getPixelDepth() != dst->getPixelDepth() ||
		srcA->getPixelComponents() != dst->getPixelComponents() ||
	    srcB->getPixelComponents() != dst->getPixelComponents() )
	{
		throw( BitDepthMismatchException() );
	}

}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View, class Functor>
void MergeProcess<View, Functor>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	try
	{
		using namespace hsl_color_space;
		View srcA = subimage_view( this->_srcViewA,
		                           procWindow.x1 - this->_renderWindow.x1,
		                           procWindow.y1 - this->_renderWindow.y1,
		                           procWindow.x2 - procWindow.x1,
		                           procWindow.y2 - procWindow.y1 );
		View srcB = subimage_view( this->_srcViewB,
		                           procWindow.x1 - this->_renderWindow.x1,
		                           procWindow.y1 - this->_renderWindow.y1,
		                           procWindow.x2 - procWindow.x1,
		                           procWindow.y2 - procWindow.y1 );
		View dst = subimage_view( this->_dstView,
		                          procWindow.x1 - this->_renderWindow.x1,
		                          procWindow.y1 - this->_renderWindow.y1,
		                          procWindow.x2 - procWindow.x1,
		                          procWindow.y2 - procWindow.y1 );
		merge_pixels( srcA, srcB, dst, Functor() );
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

}
}
}
