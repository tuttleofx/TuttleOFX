#ifndef GIL_FUNCTORS_HPP
#define GIL_FUNCTORS_HPP

#include "ColorMerge.hpp"
#include <boost/math/constants/constants.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <cmath>

namespace boost {
namespace gil {
/******************************************************************************
 * Functors that doesn't need alpha                                           *
 ******************************************************************************/

struct FunctorAverage
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// (A + B) / 2
		static pixel_plus_t<PixA, PixB, PixDst> _plus;
		static pixel_divides_scalar_t<PixDst, int, PixDst> _div2;
		dst = _plus(A, B);
		dst = _div2(dst, 2);
	}
};

struct FunctorPlus
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// A + B
		static pixel_plus_t<PixA, PixB, PixDst> _plus;
		dst = _plus(A, B);
	}
};

struct FunctorCopy
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// A
		dst = (PixDst)A;
	}
};

struct FunctorDifference
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// difference
//		dst = (PixDst)( std::abs(A - B) );
	}
};

struct FunctorDivide
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
/*		if ( A < 0 && B < 0 )
			dst = PixDst(0);
		else
			dst = (PixDst)( A / B );
*/
	}
};

struct FunctorExclusion
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A + B - 2.0f * A * B );
	}
};

struct FunctorFrom
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( B - A );
	}
};

struct FunctorGeometric
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( 2*A*B/(A+B) );
	}
};

struct FunctorMultiply
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
/*		if ( A < 0 && B < 0 )
			dst = 0;
		else
			dst = (PixDst)( A * B );
*/
	}
};

struct FunctorScreen
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = A + B - A*B;
	}
};

struct FunctorHardLight
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
/*
		if ( A < 0.5f )
			FunctorMultiply::merge(A, B, dst);
		else
			FunctorScreen::merge(A, B, dst);
*/
	}
};

struct FunctorHypot
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = PixDst(std::sqrt((PixDst)(A*A+B*B)));
	}
};

struct FunctorMinus
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = PixDst(A - B);
	}
};

struct FunctorDarken
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = PixDst( std::min(A, B) );
	}
};

struct FunctorLighten
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = PixDst( std::max(PixDst(A), PixDst(B)) );
	}
};

struct FunctorOverlay
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
/*		if ( B < 0.5f )
			FunctorMultiply::merge(A, B, dst);
		else
			FunctorScreen::merge(A, B, dst);
*/
	}
};

struct FunctorColorDodge
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// f(a,b) = a / (1.0f - b)
/*		if ( B < 1.0f ) {
			dst = PixDst(A / ( 1.0f - B ));
			if ( dst > 1.0f )
				dst = 1;
		} else
			dst = PixDst(1);
*/
	}
};

struct FunctorColorBurn
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// f(a,b) = 1.0f - (1.0f - a) / b
/*		if ( B != 0 ) {
			dst = 1.0f - PixDst(( 1.0f - A ) / B);
			if ( dst < 0 )
				dst = 0;
		} else
			dst = PixDst(0);
*/
	}
};

struct FunctorPinLight
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = PixDst(B >= 0.5f ? std::max(double(A), (B - 0.5f) * 2.0 ) : min(double(A), B * 2.0 ));
	}
};

// Quadratic mode: reflect
struct FunctorReflect
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// f(a,b) = a / (1 - b)
/*		if ( B >= 1.0f )
			dst = 1.0f;
		else {
			dst = PixDst(A * A / ( 1.0f - B ));
			if ( dst > 1.0f )
				dst = 1.0f;

		}
*/
	}
};

// Quadratic mode: freeze
struct FunctorFreeze
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// f(a,b) = 1 - (1 - a) / b
/*		if ( B == 0.0f )
			dst = 0.0f;
		else {
			dst = 1.0f - sqrt(1.0f - A) / B;
			if ( dst < 0.0f )
				dst = 0.0f;
		}
*/
	}
};

// Similar to average, but smoother...
struct FunctorInterpolated
: public boost::gil::merge_functor<need_noalpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		//f(a,b) =  - cos(pi*a) - cos(pi*b)
		// Very slow implementation: that's because we are working with floats.
		// on integer types, we should be using a precomputed cosine table.
//		dst = 0.5f - 0.25f * std::cos( boost::math::constants::pi<PixA>() * float(A) )
//				   - 0.25f * std::cos( boost::math::constants::pi<PixB>() * float(B) );
	}
};


/******************************************************************************
 * Functors that does need alpha                                              *
 ******************************************************************************/
struct FunctorATop
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
		// Ab + B(1-a)
//		dst = (PixDst)( A * b + B * ( 1.0f - a ) );
    }
};

struct FunctorConjointOver
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
/*		if ( a > b )
			FunctorCopy::merge(A, B, dst);
		else
			dst = (PixDst)( A + B * (1.0f - a) / b );
*/
	}
};

struct FunctorDisjointOver
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
/*		if ( a + b < 1.0f )
			FunctorPlus::merge(A, B, dst);
		else
			dst = (PixDst)( A + B * (1.0f - a) / b );
*/
	}
};

struct FunctorIn
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A * b );
	}
};

struct FunctorMask
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( B * a );
	}
};

struct FunctorMatte
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A * a + B*(1.0f - a) );
	}
};

struct FunctorOut
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A * (1.0f - b) );
	}
};

struct FunctorOver
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A + B * (1.0f - a) );
	}
};

struct FunctorStencil
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( B * (1.0f - a) );
	}
};

struct FunctorUnder
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A * (1.0f - b) + B );
	}
};

struct FunctorXOR
: public boost::gil::merge_functor<need_alpha_t>
{
	template <typename PixA, typename PixB, typename PixDst>
	inline static void merge( const PixA& A, const PixB& B, PixDst& dst )
	{
//		dst = (PixDst)( A * (1.0f - b) + B * (1.0f - a) );
	}
};

}
}

#endif //GIL_FUNCTORS_HPP
