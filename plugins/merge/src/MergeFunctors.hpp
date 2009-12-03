#ifndef GIL_FUNCTORS_HPP
#define GIL_FUNCTORS_HPP

#include "ColorMerge.hpp"

namespace boost {
namespace gil {

struct FunctorATop
: public boost::gil::merge_functor<boost::mpl::true_>
{
    template <typename AlphaA, typename AlphaB,
              typename SrcAV, typename SrcBV, typename DstV>
    inline static void merge( const AlphaA& a, const AlphaB& b,
                              const SrcAV& A, const SrcBV& B, DstV & dst )
    {
        // Ab + B(1-a)
        dst = (DstV)( A * b + B * ( 1 - a ) );
    }
};

struct FunctorAverage
: public boost::gil::merge_functor<boost::mpl::false_>
{
    template <typename SrcAV, typename SrcBV, typename DstV>
	inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
    {
        // (A + B) / 2
		dst = (DstV)( ( ((float)A) + B ) / 2.0f );
    }
};

struct FunctorPlus
: public boost::gil::merge_functor<boost::mpl::false_>
{
    template <typename SrcAV, typename SrcBV, typename DstV>
	inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
    {
        // A + B
		dst = (DstV)( A + B );
    }
};

}
}

#endif //GIL_FUNCTORS_HPP
