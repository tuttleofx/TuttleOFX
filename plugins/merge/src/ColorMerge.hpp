/*
 *  Copyright 2005-2007 Adobe Systems Incorporated
 *
 *  Use, modification and distribution are subject to the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt).
 *
 *  See http://opensource.adobe.com/gil for most recent version including documentation.
 */
/*************************************************************************************************/

#ifndef GIL_COLOR_MERGE_HPP
#define GIL_COLOR_MERGE_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief GIL default color merging
/// \author Eloi Du Bois
/// \date   October 27, 2009
///
///
////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <boost/static_assert.hpp>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>

namespace boost {
namespace gil {

////////////////////////////////////////////////////////////////////////////////////////
///
///                 COLOR MERGING
///
////////////////////////////////////////////////////////////////////////////////////////

/// @defgroup ColorMerge Color Space Merging
/// \brief	Functors need to heritates from merge_functor<boost::mpl::true_ or false_>
template <typename NEED_ALPHA>
struct merge_functor
{
	// boost::mpl::true_ or false_
	typedef NEED_ALPHA need_alpha_t;
};

/// \ingroup ChannelConvertAlgorithm
/// \brief Same as channel_converter, except it takes the destination channel
///        by reference, which allows us to move the templates from the class
///        level to the method level. This is important when invoking it on
///        heterogeneous pixels.

template<class F, typename ItA, typename ItB>
struct default_alpha_channel_merging
{
	// View iterators are needed because merging depends on each alpha
	// channel
	const ItA& _itA;  /// View iterator src A
	const ItB& _itB;  /// View iterator src B

	default_alpha_channel_merging( const ItA& itA, const ItB& itB )
		: _itA( itA ),
		  _itB( itB ) {}

	template <typename V1, typename V2, typename V3>
	void operator()( const V1& srcA, const V2& srcB, V3& dst ) const
	{
		dst = F::merge<V3>( get_color(_itA, alpha_t()), get_color(_itB, alpha_t()), srcA, srcB );
	}
};

template<class F>
struct default_channel_merging
{
	template <typename V1, typename V2, typename V3>
	void operator()( const V1& srcA, const V2& srcB, V3& dstC ) const
	{
		dstC = F::merge<V3>( srcA, srcB );
	}
};

/// Implementation for pixel merging
template < class F >
struct merge_pixels_alpha_impl {
	template < typename View1, typename View2, typename View3 >
	inline void operator()( View1& src1, View2& src2, View3& dst ) {
		View3 srcA(color_converted_view<View3::value_type>(src1));
		View3 srcB(color_converted_view<View3::value_type>(src2));
		typedef typename View3::value_type channel_t;

		// Need alpha channel on sources & destination
		for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
		{
			typename View3::x_iterator srcIt1 = srcA.row_begin( y );
			typename View3::x_iterator srcIt2 = srcB.row_begin( y );
			typename View3::x_iterator dstIt  = dst.row_begin( y );
			for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
				static_for_each( srcIt1[x],
								 srcIt2[x],
								 dstIt[x],
								 default_alpha_channel_merging
								 <	 F,
									 channel_t,
									 channel_t
								 >( srcIt1[x], srcIt2[x] )
							   );
		}
	}
};

template < class F >
struct merge_pixels_impl {
	template < typename View1, typename View2, typename View3 >
	inline void operator()( View1& src1, View2& src2, View3& dst )
	{
		// Need alpha channel on sources & destination
		for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
		{
			typename View1::x_iterator srcIt1 = src1.row_begin( y );
			typename View2::x_iterator srcIt2 = src2.row_begin( y );
			typename View3::x_iterator dstIt  = dst.row_begin( y );
			for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
				static_for_each( srcIt1[x],
								 srcIt2[x],
								 dstIt[x],
								 default_channel_merging<F>() );
		}
	}
};

template < class F, typename View1, typename View2, typename View3>
void merge_pixels( View1& src1, View2& src2, View3& dst )
{
	// If merging functor needs alpha, check if destination contains alpha.
	typedef typename mpl::contains<typename color_space_type<View3::value_type>::type, alpha_t>::type has_alpha_t;
	BOOST_STATIC_ASSERT((has_alpha_t::value == F::need_alpha_t::value) || F::need_alpha_t::value == false);

	// Chose the right merging function according to alpha channel needs
	mpl::if_< F::need_alpha_t, typename merge_pixels_alpha_impl<F>, typename merge_pixels_impl<F> >::type merger;
	// Apply
	merger(src1, src2, dst);
}

}
}  // namespace boost::gil

#endif
