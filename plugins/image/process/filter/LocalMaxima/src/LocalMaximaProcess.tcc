#include "LocalMaximaPlugin.hpp"
#include "LocalMaximaAlgorithm.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/math/constants/constants.hpp>

#include <queue>
#include <utility>

namespace tuttle {
namespace plugin {
namespace localmaxima {

template<class View>
LocalMaximaProcess<View>::LocalMaximaProcess( LocalMaximaPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template <class View>
void LocalMaximaProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void LocalMaximaProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	namespace bgil = boost::gil;
	namespace bm = boost::math;
	typedef bgil::point2<std::ptrdiff_t> Point2;
	
	OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
	                             procWindowRoW.y2 - procWindowRoW.y1 };
	static const unsigned int border = 1;
	OfxRectI srcRodCrop = rectangleReduce( this->_srcPixelRod, border );
	OfxRectI procWindowRoWCrop = rectanglesIntersection( procWindowRoW, srcRodCrop );

	OfxRectI procWindowOutputCrop = translateRegion( procWindowRoWCrop, this->_dstPixelRod );

	if( _params._border == eParamBorderBlack )
	{
		View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
												  procWindowSize.x, procWindowSize.y );

		// fill borders
		Pixel pixelZero; bgil::pixel_zeros_t<Pixel>()( pixelZero );
		boost::gil::fill_pixels( dst, pixelZero );
	}

	transform_pixels_locator_progress( this->_srcView, this->_srcPixelRod,
	                                   this->_dstView, this->_dstPixelRod,
									   procWindowRoWCrop,
									   pixel_locator_gradientLocalMaxima_t<View>(this->_srcView),
									   *this );
}

}
}
}
