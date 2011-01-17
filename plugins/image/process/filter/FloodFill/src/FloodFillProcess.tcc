#include "FloodFillAlgorithm.hpp"
#include "FloodFillPlugin.hpp"

#include <tuttle/common/math/rectOp.hpp>

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations_minmax.hpp>
#include <boost/gil/extension/toolbox/channel_view.hpp>

namespace tuttle {
namespace plugin {
namespace floodFill {

template<class View>
FloodFillProcess<View>::FloodFillProcess( FloodFillPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void FloodFillProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
	if( _params._relativeMinMax )
	{
//		typedef channel_view_type<red_t,View> LocalView;
		typedef kth_channel_view_type<0,View> LocalView;
		typename LocalView::type localView( LocalView::make(this->_srcView) );
		pixel_minmax_by_channel_t<typename LocalView::type::value_type> minmax( localView(0,0) );
		transform_pixels_progress(
			localView,
			minmax,
			*this );
		
		_isConstantImage = minmax.max[0] == minmax.min[0];
		_lowerThres = (_params._lowerThres * (minmax.max[0]-minmax.min[0])) + minmax.min[0];
		_upperThres = (_params._upperThres * (minmax.max[0]-minmax.min[0])) + minmax.min[0];
	}
	else
	{
		_isConstantImage = false;
		_lowerThres = _params._lowerThres;
		_upperThres = _params._upperThres;
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void FloodFillProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	
	static const unsigned int border = 1;
	OfxRectI srcRodCrop = rectangleReduce( this->_srcPixelRod, border );
	OfxRectI procWindowRoWCrop = rectanglesIntersection( procWindowRoW, srcRodCrop );

	fill_pixels( this->_dstView, procWindowOutput, get_black<Pixel>() );

	if( _isConstantImage )
		return;

	switch( _params._method )
	{
		case eParamMethod4:
		{
			flood_fill<Connexity4, IsUpper<Scalar>, IsUpper<Scalar>, View, View>(
				this->_srcView, this->_srcPixelRod,
				this->_dstView, this->_dstPixelRod,
				procWindowRoWCrop,
				IsUpper<Scalar>(_upperThres),
				IsUpper<Scalar>(_lowerThres)
				);
			break;
		}
		case eParamMethod8:
		{
			flood_fill<Connexity8, IsUpper<Scalar>, IsUpper<Scalar>, View, View>(
				this->_srcView, this->_srcPixelRod,
				this->_dstView, this->_dstPixelRod,
				procWindowRoWCrop,
				IsUpper<Scalar>(_upperThres),
				IsUpper<Scalar>(_lowerThres)
				);
			break;
		}
		case eParamMethodBruteForce: // not in production
		{
//			flood_fill_bruteForce<IsUpper<Scalar>, IsUpper<Scalar>, View, View>(
//				this->_srcView, this->_srcPixelRod,
//				this->_dstView, this->_dstPixelRod,
//				procWindowRoWCrop,
//				IsUpper<Scalar>(_upperThres),
//				IsUpper<Scalar>(_lowerThres)
//				);
			break;
		}
	}
}

}
}
}
