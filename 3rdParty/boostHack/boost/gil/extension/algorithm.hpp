#ifndef _GIL_EXTENSION_ALGORITHM_HPP_
#define	_GIL_EXTENSION_ALGORITHM_HPP_

#include <boost/gil/algorithm.hpp>

namespace boost {
namespace gil {

/// \ingroup ImageViewSTLAlgorithmsTransformPixels
/// \brief std::transform for image views
template <typename View, typename F> GIL_FORCEINLINE
F transform_pixels( const View& dst, F& fun )
{
	assert( src.dimensions( ) == dst.dimensions( ) );
	for( std::ptrdiff_t y = 0; y < dst.height( ); ++y )
	{
		typename View::x_iterator dstIt = dst.row_begin( y );
		for( std::ptrdiff_t x = 0; x < dst.width( ); ++x )
			fun( dstIt[x] );
	}
	return fun;
}

}
}

#endif

