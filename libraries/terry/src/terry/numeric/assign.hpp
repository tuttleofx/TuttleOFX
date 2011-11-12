#ifndef _TERRY_NUMERIC_ASSIGN_HPP_
#define _TERRY_NUMERIC_ASSIGN_HPP_

#include "pixel_numeric_operations.hpp"

namespace terry {

/// \brief std::for_each for a pair of iterators
template <typename Iterator1, typename Iterator2, typename BinaryFunction>
GIL_FORCEINLINE
BinaryFunction for_each( Iterator1 first1, Iterator1 last1, Iterator2 first2, BinaryFunction f )
{
    while( first1 != last1 )
	{
        f( *first1++, *first2++ );
	}
    return f;
}

template <typename SrcIterator, typename DstIterator>
GIL_FORCEINLINE
DstIterator assign_pixels( SrcIterator src, SrcIterator src_end, DstIterator dst )
{
	for_each( src, src_end, dst,
		pixel_assigns_t<typename pixel_proxy<typename std::iterator_traits<SrcIterator>::value_type>::type,
		                typename pixel_proxy<typename std::iterator_traits<DstIterator>::value_type>::type>());
    return dst+(src_end-src);
}


}

#endif
