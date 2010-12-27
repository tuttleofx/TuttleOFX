#ifndef _BOOST_GIL_VIEWS_MERGING_HPP_
#define _BOOST_GIL_VIEWS_MERGING_HPP_

#include "MergeAbstractFunctor.hpp"

#include <boost/static_assert.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/utilities.hpp>

namespace boost {
namespace gil {

namespace detail {
template<class OPERATES>
struct merger {};

template<>
struct merger<merge_per_pixel>
{
	template <class Pixel, class Functor>
	GIL_FORCEINLINE
	void operator()( const Pixel& A, const Pixel& B, Pixel& d, Functor& fun ) const
	{
		fun( A, B, d );
	}
};

template<>
struct merger<merge_per_channel_with_alpha>
{
	template <class Pixel, class Functor>
	GIL_FORCEINLINE
	void operator()( const Pixel& A, const Pixel& B, Pixel& d, Functor& fun ) const
	{
		fun.a = alpha_or_max( A );
		fun.b = alpha_or_max( B );
		static_for_each( A, B, d, fun );
	}

};

template<>
struct merger<merge_per_channel>
{
	template <class Pixel, class Functor>
	GIL_FORCEINLINE
	void operator()( const Pixel& A,
	                 const Pixel& B,
	                 Pixel& d,
	                 Functor& fun ) const
	{
		static_for_each( A, B, d, fun );
	}

};
} // end namespace detail

/**
 * @defgroup ViewsMerging
 * @brief Merge two views by means of a given functor.
 **/
template < class F, class View>
void merge_pixels( const View& srcA, const View& srcB, View& dst, F fun )
{
	detail::merger<typename F::operating_mode_t> merge_op;
	// If merging functor needs alpha, check if destination contains alpha.
	typedef typename contains_color< typename View::value_type, alpha_t>::type has_alpha_t;
//	BOOST_STATIC_ASSERT(( boost::is_same<typename F::operating_mode_t, merge_per_channel_with_alpha>::value ? has_alpha_t::value : true ));

	// Merge views.
	for( std::ptrdiff_t y = 0; y < dst.height(); ++y )
	{
		typename View::x_iterator srcIt1 = srcA.row_begin( y );
		typename View::x_iterator srcIt2 = srcB.row_begin( y );
		typename View::x_iterator dstIt  = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < dst.width(); ++x )
			merge_op( srcIt1[x], srcIt2[x], dstIt[x], fun );
	}
}

}
}

#endif
