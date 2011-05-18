#ifndef _TUTTLE_PLUGIN_GIL_COLOR_HPP_
#define	_TUTTLE_PLUGIN_GIL_COLOR_HPP_

#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <tuttle/plugin/color/colorSpaceAPI.hpp>

namespace tuttle {
namespace plugin {

/**
 * @brief Set each pixel in the destination view as the result of a color transformation over the source view
 * @ingroup ImageAlgorithms
 *
 * The provided implementation works for 2D image views only
 */
template <	typename ColorSpaceIn, // Models ColorSpaceConcept
		typename ColorSpaceOut, // Models ColorSpaceConcept
		typename SrcView, // Models RandomAccess2DImageViewConcept
		typename DstView> // Models MutableRandomAccess2DImageViewConcept
void colorspace_pixels_progress( tuttle::plugin::color::ColorSpaceAPI* colorSpaceAPI, const SrcView& src_view, const DstView& dst_view, tuttle::plugin::IProgress* p, ColorSpaceIn colorSpaceIn = ColorSpaceIn(), ColorSpaceOut colorSpaceOut = ColorSpaceOut() )
{
	for( int y = 0; y < src_view.height(); ++y )
	{
		typename SrcView::x_iterator src_it = src_view.row_begin( y );
		typename DstView::x_iterator dst_it = dst_view.row_begin( y );

		for( int x = 0; x < src_view.width(); ++x, ++src_it, ++dst_it )
		{
			*dst_it = *src_it;
			//colorSpaceAPI->colorspace_convert( colorSpaceIn, colorSpaceOut, *src_it , *dst_it  );
		}
		if( p->progressForward() )
			return;
	}
}

}
}

#endif

