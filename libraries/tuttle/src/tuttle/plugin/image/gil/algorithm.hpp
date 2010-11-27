#ifndef _TUTTLE_PLUGIN_GIL_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_GIL_ALGORITHM_HPP_

#include <tuttle/plugin/IProgress.hpp>

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

namespace tuttle {
namespace plugin {

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
F transform_pixels_progress( const View& dst, F& fun, IProgress& p )
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
template <typename View, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View& dst, const F& fun, IProgress& p )
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
F transform_pixels_progress( const View1& src, const View2& dst, F& fun, IProgress& p )
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

template <typename View1, typename View2, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View1& src, const View2& dst, const F& fun, IProgress& p )
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
F transform_pixels_progress( const View1& src1, const View2& src2, const View3& dst, F& fun, IProgress& p )
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

template <typename View1, typename View2, typename View3, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View1& src1, const View2& src2, const View3& dst, const F& fun, IProgress& p )
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
F transform_pixels_progress( const View1& src1, const View2& src2, const View3& src3, const View4& dst, F& fun, IProgress& p )
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

template <typename View1, typename View2, typename View3, typename View4, typename F>
GIL_FORCEINLINE
F transform_pixels_progress( const View1& src1, const View2& src2, const View3& src3, const View4& dst, const F& fun, IProgress& p )
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


//////////////////////////////////////////////////////////////////////////////////////
///
/// transform_pixels_locator
///
//////////////////////////////////////////////////////////////////////////////////////

/// \defgroup ImageViewSTLAlgorithmsTransformPixelLocator transform_pixels_locator
/// \ingroup ImageViewSTLAlgorithms
/// \brief for image views (passes locators, instead of pixel references, to the function object)

// 1 source/dest

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View::xy_locator dloc = dst.xy_at( renderWin.x1-dstRod.x1, renderWin.y1-dstRod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++dloc.x() )
		{
			fun( dloc );
		}
		dloc.x() -= renderWidth; ++dloc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, const F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View::xy_locator dloc = dst.xy_at( renderWin.x1-dstRod.x1, renderWin.y1-dstRod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++dloc.x() )
		{
			fun( dloc );
		}
		dloc.x() -= renderWidth; ++dloc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}

// 1 source, 1 dest

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View, typename ViewDst, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View& src, const OfxRectI& srcRod,
									 const ViewDst& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View::xy_locator sloc = src.xy_at( renderWin.x1-srcRod.x1, renderWin.y1-srcRod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		typename ViewDst::x_iterator dstIt = dst.x_at( renderWin.x1-dstRod.x1, y-dstRod.y1 );
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++sloc.x(), ++dstIt )
		{
			*dstIt = fun( sloc );
		}
		sloc.x() -= renderWidth; ++sloc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View, typename ViewDst, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View& src, const OfxRectI& srcRod,
									 const ViewDst& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, const F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View::xy_locator sloc = src.xy_at( renderWin.x1-srcRod.x1, renderWin.y1-srcRod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		typename ViewDst::x_iterator dstIt = dst.x_at( renderWin.x1-dstRod.x1, y-dstRod.y1 );
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++sloc.x(), ++dstIt )
		{
			*dstIt = fun( sloc );
		}
		sloc.x() -= renderWidth; ++sloc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}

// 2 sources, 1 dest

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View1, typename View2, typename ViewDst, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View1& src1, const OfxRectI& src1Rod,
									 const View2& src2, const OfxRectI& src2Rod,
									 const ViewDst& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View1::xy_locator s1loc = src1.xy_at( renderWin.x1-src1Rod.x1, renderWin.y1-src1Rod.y1 );
	typename View2::xy_locator s2loc = src2.xy_at( renderWin.x1-src2Rod.x1, renderWin.y1-src2Rod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		typename ViewDst::x_iterator dstIt = dst.x_at( renderWin.x1-dstRod.x1, y-dstRod.y1 );
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++s1loc.x(), ++s2loc.x(), ++dstIt )
		{
			*dstIt = fun( s1loc, s2loc );
		}
		s1loc.x() -= renderWidth; ++s1loc.y();
		s2loc.x() -= renderWidth; ++s2loc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View1, typename View2, typename ViewDst, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View1& src1, const OfxRectI& src1Rod,
									 const View2& src2, const OfxRectI& src2Rod,
									 const ViewDst& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, const F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View1::xy_locator s1loc = src1.xy_at( renderWin.x1-src1Rod.x1, renderWin.y1-src1Rod.y1 );
	typename View2::xy_locator s2loc = src2.xy_at( renderWin.x1-src2Rod.x1, renderWin.y1-src2Rod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		typename ViewDst::x_iterator dstIt = dst.x_at( dstRod.x1, y-dstRod.y1 );
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++s1loc.x(), ++s2loc.x(), ++dstIt )
		{
			*dstIt = fun( s1loc, s2loc );
		}
		s1loc.x() -= renderWidth; ++s1loc.y();
		s2loc.x() -= renderWidth; ++s2loc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}


// 3 sources, 1 dest

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View1, typename View2, typename View3, typename ViewDst, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View1& src1, const OfxRectI& src1Rod,
									 const View2& src2, const OfxRectI& src2Rod,
									 const View2& src3, const OfxRectI& src3Rod,
									 const ViewDst& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View1::xy_locator s1loc = src1.xy_at( renderWin.x1-src1Rod.x1, renderWin.y1-src1Rod.y1 );
	typename View2::xy_locator s2loc = src2.xy_at( renderWin.x1-src2Rod.x1, renderWin.y1-src2Rod.y1 );
	typename View3::xy_locator s3loc = src3.xy_at( renderWin.x1-src3Rod.x1, renderWin.y1-src3Rod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		typename ViewDst::x_iterator dstIt = dst.x_at( dstRod.x1, y-dstRod.y1 );
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++s1loc.x(), ++s2loc.x(), ++s3loc.x(), ++dstIt )
		{
			*dstIt = fun( s1loc, s2loc, s3loc );
		}
		s1loc.x() -= renderWidth; ++s1loc.y();
		s2loc.x() -= renderWidth; ++s2loc.y();
		s3loc.x() -= renderWidth; ++s3loc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View1, typename View2, typename View3, typename ViewDst, typename F>
GIL_FORCEINLINE
F transform_pixels_locator_progress( const View1& src1, const OfxRectI& src1Rod,
									 const View2& src2, const OfxRectI& src2Rod,
									 const View2& src3, const OfxRectI& src3Rod,
									 const ViewDst& dst, const OfxRectI& dstRod,
									 const OfxRectI& renderWin, const F& fun, IProgress& p )
{
	const std::ptrdiff_t renderWidth = renderWin.x2 - renderWin.x1;
	typename View1::xy_locator s1loc = src1.xy_at( renderWin.x1-src1Rod.x1, renderWin.y1-src1Rod.y1 );
	typename View2::xy_locator s2loc = src2.xy_at( renderWin.x1-src2Rod.x1, renderWin.y1-src2Rod.y1 );
	typename View3::xy_locator s3loc = src3.xy_at( renderWin.x1-src3Rod.x1, renderWin.y1-src3Rod.y1 );
	for( std::ptrdiff_t y = renderWin.y1; y < renderWin.y2; ++y )
	{
		typename ViewDst::x_iterator dstIt = dst.x_at( dstRod.x1, y-dstRod.y1 );
		for( std::ptrdiff_t x = renderWin.x1;
		     x < renderWin.x2;
		     ++x, ++s1loc.x(), ++s2loc.x(), ++s3loc.x(), ++dstIt )
		{
			*dstIt = fun( s1loc, s2loc, s3loc );
		}
		s1loc.x() -= renderWidth; ++s1loc.y();
		s2loc.x() -= renderWidth; ++s2loc.y();
		s3loc.x() -= renderWidth; ++s3loc.y();
		if( p.progressForward( renderWidth ) )
			return fun;
	}
	return fun;
}




}
}

#endif
