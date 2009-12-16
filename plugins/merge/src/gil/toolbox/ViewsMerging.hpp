#ifndef GIL_VIEWS_MERGING_HPP
#define GIL_VIEWS_MERGING_HPP

////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief GIL default views merging
/// \author Eloi Du Bois
/// \date   October 27, 2009
///
///
////////////////////////////////////////////////////////////////////////////////

#include <boost/static_assert.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/mpl/integral_c.hpp>

namespace boost {
namespace gil {

////////////////////////////////////////////////////////////////////////////////
///
///                 VIEWS MERGING
///
////////////////////////////////////////////////////////////////////////////////

/// Functor that operates with pixels (user have to define what to do)
/// for example: a rgb view that need yuv to merge
typedef mpl::integral_c<char, 0>::type fun_op_pixel_t;
/// Functor that need alpha
typedef mpl::integral_c<char, 1>::type fun_op_alpha_t;
/// Functor that doesn't need alpha
typedef mpl::integral_c<char, 2>::type fun_op_no_alpha_t;

/**
 * \defgroup ViewsMerging
 * \brief merging functor base class
 */
template <typename Pixel, typename OPERATES>
struct merge_functor
{
	typedef OPERATES operating_mode_t;
};

/**
 * \defgroup ViewsMerging
 * \brief merging functor that allows users to implement pixel merging operator
 */
template <typename Pixel>
struct merge_functor<Pixel, fun_op_pixel_t>
{
	typedef fun_op_pixel_t operating_mode_t;
	// no pure virtual here because virtual is not inlined with all compilators
	inline void operator()( const Pixel& A,
	                        const Pixel& B, Pixel& d );
};

/**
 * \defgroup ViewsMerging
 * \brief merging functor that allows users to implement channel merging
 *		  operator with alpha
 */
template <typename Pixel>
struct merge_functor<Pixel, fun_op_alpha_t>
{
	typedef typename channel_type<Pixel>::type value_t;
	typedef fun_op_alpha_t operating_mode_t;
	value_t a;      /// Alpha source A
	value_t b;      /// Alpha source B

	// no pure virtual here because virtual is not inlined with all compilators
	template<typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& d );
};

/**
 * \defgroup ViewsMerging
 * \brief merging functor that allows users to implement channel merging
 *		  operator with no alpha
 */
template <typename Pixel>
struct merge_functor<Pixel, fun_op_no_alpha_t>
{
	typedef fun_op_no_alpha_t operating_mode_t;

	// no pure virtual here because virtual is not inlined with all compilators
	template<typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& d );
};

namespace detail {
template<class OPERATES>
struct merger {};

template<>
struct merger<fun_op_pixel_t>
{
	template <class Pixel, class Functor>
	static inline void merge( const Pixel& A, const Pixel& B, Pixel& d, Functor & fun )
	{
		fun( A, B, d );
	}

};
template<>
struct merger<fun_op_alpha_t>
{
	template <class Pixel, class Functor>
	static inline void merge( const Pixel& A, const Pixel& B, Pixel& d, Functor & fun )
	{
		fun.a = get_color( A, alpha_t() );
		fun.b = get_color( B, alpha_t() );
		static_for_each( A, B, d, fun );
	}

};
template<>
struct merger<fun_op_no_alpha_t>
{
	template <class Pixel, class Functor>
	static inline void merge( const Pixel& A,
	                          const Pixel& B, Pixel& d,
	                          Functor & fun )
	{
		static_for_each( A, B, d, fun );
	}

};
} // end namespace detail

/**
 * \defgroup ViewsMerging
 * \brief Merge two views by means of a given functor.
 **/
template < class F, class View>
void merge_pixels( const View& srcA, const View& srcB, View& dst, F fun )
{
	typedef detail::merger<typename F::operating_mode_t> merger_t;
	// If merging functor needs alpha, check if destination contains alpha.
	typedef typename
	mpl::contains<
	    typename color_space_type<typename View::value_type>::type,
	    alpha_t>::type has_alpha_t;
	BOOST_STATIC_ASSERT( ( has_alpha_t::value &&
	                       ( F::operating_mode_t::value == fun_op_alpha_t::value ) )
	                     || F::operating_mode_t::value != fun_op_alpha_t::value );

	// Merge views.
	for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
	{
		typename View::x_iterator srcIt1 = srcA.row_begin( y );
		typename View::x_iterator srcIt2 = srcB.row_begin( y );
		typename View::x_iterator dstIt  = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
			merger_t::merge( srcIt1[x], srcIt2[x], dstIt[x], fun );
	}
}

}
}  // namespace boost::gil

#endif
