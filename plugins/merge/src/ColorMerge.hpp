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
#include <boost/gil/gil_all.hpp>
/*
#include <boost/gil/gil_config.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/image_view_factory.hpp>
*/

namespace boost {
namespace gil {

////////////////////////////////////////////////////////////////////////////////////////
///
///                 COLOR MERGING
///
////////////////////////////////////////////////////////////////////////////////////////

typedef mpl::true_  need_alpha_t;
typedef mpl::false_ need_noalpha_t;

/// @defgroup ColorMerge Color Space Merging
/// \brief	Functors need to heritates from merge_functor<boost::mpl::true_ or false_>
template <typename NEED_ALPHA>
struct merge_functor
{
	// boost::mpl::true_ or false_
	typedef NEED_ALPHA need_alpha_t;
};

namespace detail {

/// Implementation for pixel merging
template < class F >
struct merge_pixels_alpha_impl {
	template < typename View >
	inline void operator()( const View& srcA, const View& srcB, View& dst ) {
		typedef typename View::value_type channel_t;

		// Need alpha channel on sources & destination
		for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
		{
			typename View::x_iterator srcIt1 = srcA.row_begin( y );
			typename View::x_iterator srcIt2 = srcB.row_begin( y );
			typename View::x_iterator dstIt  = dst.row_begin( y );
			for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
				F::merge( srcIt1[x], srcIt2[x], dstIt[x] );
		}
	}
};

template < class F >
struct merge_pixels_impl {
	template < typename View >
	inline void operator()( const View& srcA, const View& srcB, View& dst )
	{
// @todo: the separation between alpha & non-alpha is temporary.
// I don't know yet if it will be usefull in the future.
// Following comments shows what need to be done:

//		typename View::value_type alpha_max = channel_traits< typename channel_type< typename View::value_type >::type >::max_value();
		// Need alpha channel on sources & destination
		for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
		{
			typename View::x_iterator srcIt1 = srcA.row_begin( y );
			typename View::x_iterator srcIt2 = srcB.row_begin( y );
			typename View::x_iterator dstIt = dst.row_begin( y );
			for( std::ptrdiff_t x = 0; x < dst.width(); ++x ) {
				F::merge( srcIt1[x], srcIt2[x], dstIt[x] );
//				get_color(dstIt1[x], alpha_t()) = alpha_max;
			}
		}
	}
};

}

template < class F, typename View1, typename View2, typename View3>
void merge_pixels( const View1& src1, const View2& src2, View3& dst )
{
	// If merging functor needs alpha, check if destination contains alpha.
	typedef typename mpl::contains<
			typename color_space_type<typename View3::value_type>::type
									  , alpha_t>::type has_alpha_t;
	BOOST_STATIC_ASSERT((has_alpha_t::value == F::need_alpha_t::value)
						|| F::need_alpha_t::value == false);

	// Chose the right merging function according to alpha channel needs
	typename mpl::if_< typename F::need_alpha_t,
					   typename detail::merge_pixels_alpha_impl<F>,
					   typename detail::merge_pixels_impl<F> >::type merger;
	// Apply
	merger(color_converted_view<typename View3::value_type>(src1),
		   color_converted_view<typename View3::value_type>(src2),
		   dst);
}

}
}  // namespace boost::gil

#endif
