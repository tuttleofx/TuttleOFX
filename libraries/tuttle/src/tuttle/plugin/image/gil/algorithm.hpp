#ifndef _TUTTLE_PLUGIN_GIL_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_GIL_ALGORITHM_HPP_

#include <tuttle/plugin/Progress.hpp>

#include <boost/gil/gil_config.hpp>
#include <boost/gil/gil_concept.hpp>
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/bit_aligned_pixel_iterator.hpp>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <memory>
#include <typeinfo>

namespace boost {
namespace gil {

using tuttle::plugin::Progress;

//////////////////////////////////////////////////////////////////////////////////////
///
/// transform_pixels
///
//////////////////////////////////////////////////////////////////////////////////////

/// \defgroup ImageViewSTLAlgorithmsTransformPixels transform_pixels
/// \ingroup ImageViewSTLAlgorithms
/// \brief std::transform for image views

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View& dst, F fun, Progress& p )
{
	for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
	{
		typename View::x_iterator dstIt = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
			fun( dstIt[x] );
		if( p.progressForward( dst.width() ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View1, typename View2, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View1& src, const View2& dst, F fun, Progress& p )
{
	assert( src.dimensions() == dst.dimensions() );
	for( std::ptrdiff_t y = 0; y < src.height(); ++y )
	{
		typename View1::x_iterator srcIt = src.row_begin( y );
		typename View2::x_iterator dstIt = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < src.width(); ++x )
			dstIt[x] = fun( srcIt[x] );
		if( p.progressForward( dst.width() ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief transform_pixels with two sources
template <typename View1, typename View2, typename View3, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View1& src1, const View2& src2, const View3& dst, F fun, Progress& p )
{
	assert( src1.dimensions() == dst.dimensions() );
	assert( src2.dimensions() == dst.dimensions() );
	for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
	{
		typename View1::x_iterator srcIt1 = src1.row_begin( y );
		typename View2::x_iterator srcIt2 = src2.row_begin( y );
		typename View3::x_iterator dstIt  = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
			dstIt[x] = fun( srcIt1[x], srcIt2[x] );
		if( p.progressForward( dst.width() ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief transform_pixels with two sources
template <typename View1, typename View2, typename View3, typename View4, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View1& src1, const View2& src2, const View3& src3, const View4& dst, F fun, Progress& p )
{
	assert( src1.dimensions() == dst.dimensions() );
	assert( src2.dimensions() == dst.dimensions() );
	assert( src3.dimensions() == dst.dimensions() );
	for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
	{
		typename View1::x_iterator srcIt1 = src1.row_begin( y );
		typename View2::x_iterator srcIt2 = src2.row_begin( y );
		typename View3::x_iterator srcIt3 = src3.row_begin( y );
		typename View4::x_iterator dstIt  = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
			dstIt[x] = fun( srcIt1[x], srcIt2[x], srcIt3[x] );
		if( p.progressForward( dst.width() ) )
			return fun;
	}
	return fun;
}

/// \defgroup ImageViewSTLAlgorithmsTransformPixelPositions transform_pixel_positions
/// \ingroup ImageViewSTLAlgorithms
/// \brief adobe::transform_positions for image views (passes locators, instead of pixel references, to the function object)

/// \ingroup ImageViewSTLAlgorithmsTransformPixelPositions
/// \brief Like transform_pixels but passes to the function object pixel locators instead of pixel references
template <typename View1, typename View2, typename F>
GIL_FORCEINLINE
F transform_pixel_positions_progress( const View1& src, const View2& dst, F fun, Progress& p )
{
	assert( src.dimensions() == dst.dimensions() );
	typename View1::xy_locator loc = src.xy_at( 0, 0 );
	for( std::ptrdiff_t y = 0; y < src.height(); ++y )
	{
		typename View2::x_iterator dstIt = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < src.width(); ++x, ++loc.x() )
			dstIt[x] = fun( loc );
		loc.x() -= src.width(); ++loc.y();
		if( p.progressForward( src.width() ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixelPositions
/// \brief transform_pixel_positions with two sources
template <typename View1, typename View2, typename View3, typename F>
GIL_FORCEINLINE
F transform_pixel_positions_progress( const View1& src1, const View2& src2, const View3& dst, F fun, Progress& p )
{
	assert( src1.dimensions() == dst.dimensions() );
	assert( src2.dimensions() == dst.dimensions() );
	typename View1::xy_locator loc1 = src1.xy_at( 0, 0 );
	typename View2::xy_locator loc2 = src2.xy_at( 0, 0 );
	for( std::ptrdiff_t y = 0; y < src1.height(); ++y )
	{
		typename View3::x_iterator dstIt = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < src1.width(); ++x, ++loc1.x(), ++loc2.x() )
			dstIt[x] = fun( loc1, loc2 );
		loc1.x() -= src1.width(); ++loc1.y();
		loc2.x() -= src2.width(); ++loc2.y();
		if( p.progressForward( src1.width() ) )
			return fun;
	}
	return fun;
}

}
}

#endif
