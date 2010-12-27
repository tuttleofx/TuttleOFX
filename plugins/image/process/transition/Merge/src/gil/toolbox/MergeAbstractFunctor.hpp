#ifndef _BOOST_GIL_MERGEABSTRACTFUNCTOR_HPP_
#define _BOOST_GIL_MERGEABSTRACTFUNCTOR_HPP_

#include <boost/gil/color_convert.hpp>
#include <boost/gil/pixel.hpp>

namespace boost {
namespace gil {

struct merge_per_channel {}; ///< standard per channel functor
struct merge_per_channel_with_alpha {}; ///< standard per channel functor with the alpha values stored inside the functor
struct merge_per_pixel {}; ///< per pixel functor (instead of per channel)

/**
 * @defgroup ViewsMerging
 * @brief merging functor base class
 */
template <typename Pixel, typename OPERATES>
struct merge_functor
{
	typedef OPERATES operating_mode_t;
};

/**
 * @defgroup ViewsMerging
 * @brief merging functor that allows users to implement pixel merging operator
 */
template <typename Pixel>
struct merge_functor<Pixel, merge_per_pixel>
{
	typedef merge_per_pixel operating_mode_t;
	// no pure virtual here because virtual is not inlined with all compilators
	inline void operator()( const Pixel& A,
	                        const Pixel& B, Pixel& d );
};

/**
 * @defgroup ViewsMerging
 * @brief merging functor that allows users to implement channel merging
 *		  operator with alpha
 */
template <typename Pixel>
struct merge_functor<Pixel, merge_per_channel_with_alpha>
{
	typedef typename channel_type<Pixel>::type value_t;
	typedef merge_per_channel_with_alpha operating_mode_t;
	value_t a;      /// Alpha source A
	value_t b;      /// Alpha source B

	// no pure virtual here because virtual is not inlined with all compilators
	template<typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& d );
};

/**
 * @defgroup ViewsMerging
 * @brief merging functor that allows users to implement channel merging
 *		  operator with no alpha
 */
template <typename Pixel>
struct merge_functor<Pixel, merge_per_channel>
{
	typedef merge_per_channel operating_mode_t;

	// no pure virtual here because virtual is not inlined with all compilators
	template<typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& d );
};

}
}

#endif
