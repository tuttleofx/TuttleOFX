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

typedef mpl::true_  op_value_t;
typedef mpl::false_ op_pixel_t;
typedef mpl::true_  uses_alpha_t;
typedef mpl::false_ uses_noalpha_t;

namespace detail {

template<typename Pix, typename USES_ALPHA>
struct merge_functor_impl {
};

template<typename Pix>
struct merge_functor_impl<Pix, mpl::true_> {
	typedef typename Pix::value_type value_t;
	inline virtual void operator()(const value_t & A, const value_t & B,
										const value_t & a, const value_t & b,
										value_t & d) = 0;
};

template<typename Pix>
struct merge_functor_impl<Pix, mpl::false_> {
	typedef typename Pix::value_type value_t;
	inline virtual void operator()(const value_t & A, const value_t & B,
							   value_t & d) = 0;
};

/// \defgroup ViewMerge
/// \brief merging functor base class
template <typename Pix, typename USES_ALPHA>
struct merge_functor
: public detail::merge_functor_impl<Pix, USES_ALPHA>
{
	typedef typename Pix::value_type value_t;
	// boost::mpl::true_ or false_
	typedef USES_ALPHA uses_alpha_t;

	inline virtual void merge(const Pix & A, const Pix & B, Pix & d) {
		static_transform(A, B, d, *this);
	}
};

template <typename Pix>
struct merge_functor<Pix, mpl::true_>
: public detail::merge_functor_impl<Pix, mpl::true_>
{
	typedef typename Pix::value_type value_t;
	value_t _a;		/// Alpha source A
	value_t _b;		/// Alpha source B

	// boost::mpl::true_ or false_
	typedef mpl::true_ uses_alpha_t;

	inline virtual void operator()(const value_t & A, const value_t & B, value_t & d) {
		detail::merge_functor_impl<Pix, mpl::true_>(A, B, _a, _b, d);
	}

	inline virtual void merge(const Pix & A, const Pix & B, Pix & d) {
		_a = get_color(A, alpha_t());
		_b = get_color(B, alpha_t());
		static_transform(A, B, d, *this);
	}
};

template <typename Pix>
struct merge_functor<Pix, mpl::false_> : public detail::merge_functor_impl<Pix, mpl::false_>
{
	typedef typename Pix::value_type value_t;
	// boost::mpl::true_ or false_
	typedef mpl::false_ uses_alpha_t;

	virtual void merge(const Pix & A, const Pix & B, Pix & d) {
		static_transform(A, B, d, *this);
	}
};

template < typename F, typename View>
void merge_pixels( const View& srcA, const View& srcB, View& dst, F fun )
{
	// If merging functor needs alpha, check if destination contains alpha.
	typedef typename
			mpl::contains<typename color_space_type<typename View::value_type>::type, alpha_t>::type has_alpha_t;

	BOOST_STATIC_ASSERT((has_alpha_t::value == F::uses_alpha_t::value) ||
						   F::uses_alpha_t::value == false);

	typedef typename View::value_type channel_t;

	// Need alpha channel on sources & destination
	for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
	{
		typename View::x_iterator srcIt1 = srcA.row_begin( y );
		typename View::x_iterator srcIt2 = srcB.row_begin( y );
		typename View::x_iterator dstIt  = dst.row_begin( y );
//		for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
//			fun.merge( srcIt1[x], srcIt2[x], dstIt[x] );
	}
}

}
}  // namespace boost::gil

#endif
