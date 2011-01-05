#include "ThinningAlgorithm.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <boost/mpl/if.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

template<class View>
ThinningProcess<View>::ThinningProcess( ThinningPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

template<class View>
void ThinningProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ThinningProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost;
	using namespace boost::gil;
    typedef typename View::point_t Point;
    typedef typename View::coord_t Coord;

//	typedef typename mpl::if_c< num_channels<View>::value==1,
//	                           View,
//							   typename kth_channel_view_type<0,View>::type >::type CView;
	typedef View CView;
	typedef typename image_from_view<CView>::type CImage;

	static const std::size_t border = 1;
	OfxRectI srcRodCrop1 = rectangleReduce( this->_srcPixelRod, border );
	OfxRectI srcRodCrop2 = rectangleReduce( srcRodCrop1, border );
	OfxRectI procWindowRoWCrop1 = rectanglesIntersection( rectangleGrow( procWindowRoW, border ), srcRodCrop1 );
	OfxRectI procWindowRoWCrop2 = rectanglesIntersection( procWindowRoW, srcRodCrop2 );
	
//	TUTTLE_COUT_X( 20, "-");
//	TUTTLE_COUT_VAR( this->_srcPixelRod );
//	TUTTLE_COUT_VAR( srcRodCrop1 );
//	TUTTLE_COUT("");
//	TUTTLE_COUT_VAR( procWindowRoW );
//	TUTTLE_COUT_VAR( procWindowRoWCrop1 );
//	TUTTLE_COUT_X( 20, "-");

	/// @todo use an allocator
	OfxPointI tmpSize;
	tmpSize.x = procWindowRoWCrop1.x2-procWindowRoWCrop1.x1;
	tmpSize.y = procWindowRoWCrop1.y2-procWindowRoWCrop1.y1;
	CImage image_tmp( tmpSize.x, tmpSize.y );
	CView view_tmp = view( image_tmp );

	transform_pixels_locator_progress( this->_srcView, this->_srcPixelRod,
									   view_tmp, procWindowRoWCrop1,
									   procWindowRoWCrop1, pixel_locator_thinning_t<View,CView>(this->_srcView, lutthin1), *this );
	transform_pixels_locator_progress( view_tmp, procWindowRoWCrop1, //srcRodCrop1,
									   this->_dstView, this->_dstPixelRod,
									   procWindowRoWCrop2, pixel_locator_thinning_t<CView,View>(view_tmp, lutthin2), *this );

//	transform_pixels_locator_progress( this->_srcView, this->_srcPixelRod,
//									   this->_dstView, this->_dstPixelRod,
//									   procWindowRoWCrop2, pixel_locator_thinning_t<View>(this->_dstView, lutthin2), *this );
}

}
}
}
