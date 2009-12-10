#ifndef GIL_FUNCTORS_HPP
#define GIL_FUNCTORS_HPP

#include "ColorMerge.hpp"
#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace boost {
namespace gil {

/******************************************************************************
* Functors that doesn't need alpha                                           *
******************************************************************************/

template <typename Pixel>
struct FunctorAverage
	: public boost::gil::merge_functor<Pixel, mpl::false_>
{
	typedef typename Pixel::value_type value_t;
	inline void operator()( const value_t& A,
	                        const value_t& B,
	                        value_t& dst )
	{
		// (A + B) / 2
		dst = (value_t)( ( ( (float)A ) + B ) / 2.0f );
	}

};

/*
 * struct FunctorAverage
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // (A + B) / 2
 *      dst = (DstV)( ( ((float)A) + B ) / 2.0f );
 *  }
 * };
 *
 * struct FunctorPlus
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // A + B
 *      dst = (DstV)( A + B );
 *  }
 * };
 *
 * struct FunctorCopy
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // A
 *      dst = (DstV)( A );
 *  }
 * };
 *
 * struct FunctorDifference
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // difference
 *      dst = (DstV)( std::abs(A - B) );
 *  }
 * };
 *
 * struct FunctorDivide
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      if ( A < 0 && B < 0 )
 *          dst = DstV(0);
 *      else
 *          dst = (DstV)( A / B );
 *  }
 * };
 *
 * struct FunctorExclusion
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = (DstV)( A + B - 2.0f * A * B );
 *  }
 * };
 *
 * struct FunctorFrom
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = (DstV)( B - A );
 *  }
 * };
 *
 * struct FunctorGeometric
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = (DstV)( 2*A*B/(A+B) );
 *  }
 * };
 *
 * struct FunctorMultiply
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      if ( A < 0 && B < 0 )
 *          dst = 0;
 *      else
 *          dst = (DstV)( A * B );
 *  }
 * };
 *
 * struct FunctorScreen
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = A + B - A*B;
 *  }
 * };
 *
 * struct FunctorHardLight
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      if ( A < 0.5f )
 *          FunctorMultiply::merge(A, B, dst);
 *      else
 *          FunctorScreen::merge(A, B, dst);
 *  }
 * };
 *
 * struct FunctorHypot
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = DstV(std::sqrt((DstV)(A*A+B*B)));
 *  }
 * };
 *
 * struct FunctorMinus
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = DstV(A - B);
 *  }
 * };
 *
 * struct FunctorDarken
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = DstV( std::min(A, B) );
 *  }
 * };
 *
 * struct FunctorLighten
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = DstV( std::max(DstV(A), DstV(B)) );
 *  }
 * };
 *
 * struct FunctorOverlay
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      if ( B < 0.5f )
 *          FunctorMultiply::merge(A, B, dst);
 *      else
 *          FunctorScreen::merge(A, B, dst);
 *  }
 * };
 *
 * struct FunctorColorDodge
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // f(a,b) = a / (1.0f - b)
 *      if ( B < 1.0f ) {
 *          dst = DstV(A / ( 1.0f - B ));
 *          if ( dst > 1.0f )
 *              dst = 1;
 *      } else
 *          dst = DstV(1);
 *  }
 * };
 *
 * struct FunctorColorBurn
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // f(a,b) = 1.0f - (1.0f - a) / b
 *      if ( B != 0 ) {
 *          dst = 1.0f - DstV(( 1.0f - A ) / B);
 *          if ( dst < 0 )
 *              dst = 0;
 *      } else
 *          dst = DstV(0);
 *  }
 * };
 *
 * struct FunctorPinLight
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      dst = DstV(B >= 0.5f ? std::max(double(A), (B - 0.5f) * 2.0 ) : min(double(A), B * 2.0 ));
 *  }
 * };
 *
 * // Quadratic mode: reflect
 * struct FunctorReflect
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // f(a,b) = a / (1 - b)
 *      if ( B >= 1.0f )
 *          dst = 1.0f;
 *      else {
 *          dst = DstV(A * A / ( 1.0f - B ));
 *          if ( dst > 1.0f )
 *              dst = 1.0f;
 *      }
 *  }
 * };
 *
 * // Quadratic mode: freeze
 * struct FunctorFreeze
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      // f(a,b) = 1 - (1 - a) / b
 *      if ( B == 0.0f )
 *          dst = 0.0f;
 *      else {
 *          dst = 1.0f - sqrt(1.0f - A) / B;
 *          if ( dst < 0.0f )
 *              dst = 0.0f;
 *      }
 *  }
 * };
 *
 * // Similar to average, but smoother...
 * struct FunctorInterpolated
 * : public boost::gil::merge_functor<mpl::false_>
 * {
 *  template <typename SrcAV, typename SrcBV, typename DstV>
 *  inline static void merge( const SrcAV& A, const SrcBV& B, DstV& dst )
 *  {
 *      //f(a,b) =  - cos(pi*a) - cos(pi*b)
 *      // Very slow implementation: that's because we are working with floats.
 *      // on integer types, we should be using a precomputed cosine table.
 *      dst = 0.5f - 0.25f * std::cos( boost::math::constants::pi<SrcAV>() * float(A) )
 *                 - 0.25f * std::cos( boost::math::constants::pi<SrcBV>() * float(B) );
 *  }
 * };
 */

/******************************************************************************
* Functors that does need alpha                                              *
******************************************************************************/
/*
 * template <typename Pixel>
 * struct FunctorATop
 * : public boost::gil::merge_functor<Pixel, mpl::true_>
 * {
 *  typedef typename Pixel::value_type value_t;
 *  // Needed when we want to merge between color spaces.
 *  inline void operator()(const value_t& A,
 *                         const value_t& B,
 *                         const value_t& a,
 *                         const value_t& b,
 *                         value_t& dst)
 *  {
 *      // Ab + B(1-a)
 *      dst = A * b + B * (boost::gil::channel_traits<Pixel>::max_value() - a);
 *  }
 * };
 *
 *
 * struct FunctorConjointOver
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      if ( a > b )
 *          FunctorCopy::merge(A, B, dst);
 *      else
 *          dst = (DstPix)( A + B * (1.0f - a) / b );
 *  }
 * };
 *
 * struct FunctorDisjointOver
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      if ( a + b < 1.0f )
 *          FunctorPlus::merge(A, B, dst);
 *      else
 *          dst = (DstPix)( A + B * (1.0f - a) / b );
 *  }
 * };
 *
 * struct FunctorIn
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( A * b );
 *  }
 * };
 *
 * struct FunctorMask
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( B * a );
 *  }
 * };
 *
 * struct FunctorMatte
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( A * a + B*(1.0f - a) );
 *  }
 * };
 *
 * struct FunctorOut
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( A * (1.0f - b) );
 *  }
 * };
 *
 * struct FunctorOver
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( A + B * (1.0f - a) );
 *  }
 * };
 *
 * struct FunctorStencil
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( B * (1.0f - a) );
 *  }
 * };
 *
 * struct FunctorUnder
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( A * (1.0f - b) + B );
 *  }
 * };
 *
 * struct FunctorXOR
 * : public boost::gil::merge_functor<mpl::true_>
 * {
 *  template <typename PixA, typename PixB, typename DstPix>
 *  inline static void merge( const PixA& A, const PixB& B, DstPix & dst )
 *  {
 *      dst = (DstPix)( A * (1.0f - b) + B * (1.0f - a) );
 *  }
 * };
 */
}
}

#endif //GIL_FUNCTORS_HPP
