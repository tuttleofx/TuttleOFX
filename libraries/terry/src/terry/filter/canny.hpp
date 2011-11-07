#ifndef _TERRY_FILTER_CANNY_HPP_
#define _TERRY_FILTER_CANNY_HPP_

#include "sobel.hpp"
#include "localMaxima.hpp"
#include "floodFill.hpp"
#include "thinning.hpp"

#include <terry/algorithm/transform_pixels.hpp>
#include <terry/draw/fill.hpp>
#include <terry/numeric/pixel_numeric_operations.hpp>

namespace terry {
namespace filter {

/**
 * @brief Canny filtering.
 */
template<class SView, class DView, template<typename> class Alloc>
void canny(
	const SView& srcView,
	DView& sobelViewX, DView& sobelViewY, DView& cannyView,
	const point2<double>& sobelSize,
	const convolve_boundary_option sobelBoundaryOption
	)
{
	typedef typename DView::value_type DPixel;
	typedef typename SView::value_type SPixel;

	typedef typename SView::point_t Point;
	typedef typename channel_mapping_type<DView>::type DChannel;
	typedef typename floating_channel_type_t<DChannel>::type DChannelFloat;
	typedef pixel<DChannelFloat, gray_layout_t> DPixelGray;

	const Point proc_tl( 0, 0 );

	sobel<SView, DView, Alloc>( srcView, sobelViewX, sobelViewY, sobelSize, sobelBoundaryOption );

	applyLocalMaxima( srcView, cannyView );

//	floodFill

//	applyThinning( inView, tmpView, outView );

}



}
}

#endif

