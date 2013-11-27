#include "HudAlgorithm.hpp"
#include <terry/draw/fill.hpp>
#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <terry/merge/ViewsMerging.hpp>

namespace tuttle {
namespace plugin {
namespace hud {

template<class View, class Functor>
HudProcess<View, Functor>::HudProcess( HudPlugin &effect )
: ImageGilFilterProcessor<View>( effect, eImageOrientationIndependant )
, _plugin( effect )
{
}

template<class View, class Functor>
void HudProcess<View, Functor>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams<Pixel>( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View, class Functor>
void HudProcess<View, Functor>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );
	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
	const OfxPointI procWindowSize = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};
	
	terry::draw::fill_pixels( this->_dstView, ofxToGil(procWindowOutput), _params._color );

	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							                  procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							                  procWindowSize.x, procWindowSize.y );
	merge_views( src, dst, dst, Functor() );
	/*
	for( int y = procWindowOutput.y1;
			 y < procWindowOutput.y2;
			 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
			 x < procWindowOutput.x2;
			 ++x, ++src_it, ++dst_it )
		{
			(*dst_it) = (*src_it);
		}
		if( this->progressForward( procWindowSize.x ) )
			return;
	}
	*/

}

}
}
}
