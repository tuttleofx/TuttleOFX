#ifndef _TUTTLE_PLUGIN_IMAGE_GIL_FILL_HPP_
#define _TUTTLE_PLUGIN_IMAGE_GIL_FILL_HPP_


namespace tuttle {
namespace plugin {


/**
 * @brief fill all pixels inside the @p window, with the color @p pixelValue.
 *
 * @param[out] dstView Image view to fill.
 * @param[in] window Rectangle region of the image to fill.
 * @param[in] pixelValue Pixel value used to fill.
 */
template<class View>
TUTTLE_FORCEINLINE
void fill_pixels( View& dstView, const OfxRectI& window,
				  const typename View::value_type& pixelValue )
{
	typedef typename View::value_type Pixel;

	View dst = subimage_view( dstView, window.x1, window.y1,
	                                   window.x2-window.x1, window.y2-window.y1 );
	boost::gil::fill_pixels( dst, pixelValue );
}

/**
 * @brief fill a range of pixels (on the same line or with an 1d_traversable image).
 *
 * @param[out] dstBegin Begining of the region to fill. The content is filled but the original iterator is not modified (not a reference parameter).
 * @param[in] dstEnd End of the region to fill.
 * @param[in] pixelValue Pixel value used to fill.
 */
template<class DIterator, class DPixel>
TUTTLE_FORCEINLINE
void fill_pixels_range( DIterator dstBegin, const DIterator& dstEnd, const DPixel& pixelValue )
{
	do
	{
		*dstBegin = pixelValue;
		++dstBegin;
	}
	while( dstBegin != dstEnd );
}



}
}

#endif

