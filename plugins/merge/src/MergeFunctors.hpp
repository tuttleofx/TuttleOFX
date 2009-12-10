#ifndef GIL_FUNCTORS_HPP
#define GIL_FUNCTORS_HPP

#include "ViewsMerging.hpp"
#include <boost/math/constants/constants.hpp>
#include <cmath>

namespace boost {
namespace gil {

/******************************************************************************
* Functors that doesn't need alpha                                           *
******************************************************************************/

template <typename Pixel>
struct FunctorAverage
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template<typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		// (A + B) / 2
		dst = (Channel)( ( A + B ) / 2 );
	}

};

template <typename Pixel>
struct FunctorPlus
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		// A + B
		dst = (Channel)( A + B );
	}

};

template <typename Pixel>
struct FunctorCopy
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		// A
		dst = (Channel)( A );
	}

};

template <typename Pixel>
struct FunctorDifference
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		// difference
		dst = (Channel)( std::abs( A - B ) );
	}

};

template <typename Pixel>
struct FunctorDivide
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		if( A < 0 && B < 0 )
			dst = Channel( 0 );
		else
			dst = (Channel)( A / B );
	}

};

template <typename Pixel>
struct FunctorExclusion
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	// idea: on a 16 short type, A*B >> 15,
	// on 32 int types: (((int64_t)A)*B)>> 31
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( A + B - 2 * A * B );
	}

};

template <typename Pixel>
struct FunctorFrom
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( B - A );
	}

};

template <typename Pixel>
struct FunctorGeometric
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	// idea: on a 16 short type, A*B >> 15,
	// on 32 int types: (((int64_t)A)*B)>> 31
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( 2 * A * B / ( A + B ) );
	}

};

template <typename Pixel>
struct FunctorMultiply
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		if( A < 0 && B < 0 )
			dst = 0;
		else
			dst = (Channel)( A * B );
	}

};

template <typename Pixel>
struct FunctorScreen
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	// idea: on a 16 short type, A*B >> 15,
	// on 32 int types: (((int64_t)A)*B)>> 31
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = A + B - A * B;
	}

};

template <typename Pixel>
struct FunctorHardLight
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	// idea: on a 16 short type, A*B >> 15,
	// on 32 int types: (((int64_t)A)*B)>> 31
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max   = boost::gil::channel_traits<Channel>::max_value();
		static const Channel maxs2 = max / 2;

		if( A < maxs2 )
			dst = 2 * A * B;
		else
			// Screen
			dst = max - 2 * ( max - A ) * ( max - B );
	}

};

template <typename Pixel>
struct FunctorHypot
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	// idea: on a 16 short type, A*B >> 15,
	// on 32 int types: (((int64_t)A)*B)>> 31
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = Channel( std::sqrt( (float)( A * A + B * B ) ) );
	}

};

template <typename Pixel>
struct FunctorMinus
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = Channel( A - B );
	}

};

template <typename Pixel>
struct FunctorDarken
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = Channel( std::min( A, B ) );
	}

};

template <typename Pixel>
struct FunctorLighten
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = Channel( std::max( A, B ) );
	}

};

template <typename Pixel>
struct FunctorOverlay
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max   = boost::gil::channel_traits<Channel>::max_value();
		static const Channel maxs2 = max / 2;

		if( B < maxs2 )
			// Multiply
			dst = 2 * A * B;
		else
			// Screen
			dst = max - 2 * ( max - A ) * ( max - B );
	}

};

template <typename Pixel>
struct FunctorColorDodge
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		// f(a,b) = a / (1.0f - b)
		if( A < max )
		{
			dst = Channel( B / ( max - A ) );
			if( dst > max )
				dst = max;
		}
		else
			dst = max;
	}

};

template <typename Pixel>
struct FunctorColorBurn
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		// f(a,b) = 1.0f - (1.0f - a) / b
		if( B != 0 )
		{
			dst = max - Channel( ( max - A ) / B );
			if( dst < 0 )
				dst = 0;
		}
		else
			dst = Channel( 0 );
	}

};

template <typename Pixel>
struct FunctorPinLight
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel maxs2 = boost::gil::channel_traits<Channel>::max_value() / 2;

		dst = Channel( B >= maxs2 ? std::max( A, (Channel)( ( B - maxs2 ) * 2 ) )
					   : std::min( A, (Channel)( B * 2 ) ) );
	}

};

// Quadratic mode: reflect
template <typename Pixel>
struct FunctorReflect
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		// f(a,b) = a² / (1 - b)
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		if( B >= max )
			dst = max;
		else
		{
			dst = Channel( A * A / ( max - B ) );
			if( dst > max )
				dst = max;
		}
	}

};

// Quadratic mode: freeze
template <typename Pixel>
struct FunctorFreeze
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		// f(a,b) = 1 - sqrt(1 - A) / B
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		if( B == 0 )
			dst = 0;
		else
		{
			dst = max - ( Channel ) std::sqrt( (float)max - A ) / B;
			if( dst < 0 )
				dst = 0;
		}
	}

};

// Similar to average, but smoother...
template <typename Pixel>
struct FunctorInterpolated
	: public boost::gil::merge_functor<Pixel, fun_op_no_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max   = boost::gil::channel_traits<Channel>::max_value();
		static const Channel maxs2 = max / 2;
		static const Channel maxs4 = max / 4;

		//f(a,b) =  - cos(pi*a) - cos(pi*b)
		// Very slow implementation: that's because we are working with floats.
		// on integer types, we should be using a precomputed cosine table.
		dst = maxs2 - maxs4 * ( Channel ) std::cos( boost::math::constants::pi<float>() * (float)A )
		      - maxs4 * ( Channel ) std::cos( boost::math::constants::pi<float>() * (float)B );
	}

};

/******************************************************************************
* Functors that does need alpha                                              *
******************************************************************************/

template <typename Pixel>
struct FunctorATop
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template<typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& d )
	{
		// Ab + B(1-a)
		d = A * b + B * ( boost::gil::channel_traits<Channel>::max_value() - a );
	}

};

template <typename Pixel>
struct FunctorConjointOver
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		if( a > b )
			// Copy
			dst = A;
		else
			dst = (Channel)( A + B * ( max - a ) / b );
	}

};

template <typename Pixel>
struct FunctorDisjointOver
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		if( a + b < max )
			dst = A + B;
		else
			dst = (Channel)( A + B * ( max - a ) / b );
	}

};

template <typename Pixel>
struct FunctorIn
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( A * b );
	}

};

template <typename Pixel>
struct FunctorMask
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( B * a );
	}

};

template <typename Pixel>
struct FunctorMatte
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		dst = (Channel)( A * a + B * ( max - a ) );
	}

};

template <typename Pixel>
struct FunctorOut
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		dst = (Channel)( A * ( max - b ) );
	}

};

template <typename Pixel>
struct FunctorOver
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( A + B * ( boost::gil::channel_traits<Channel>::max_value() - a ) );
	}

};

template <typename Pixel>
struct FunctorStencil
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( B * ( boost::gil::channel_traits<Channel>::max_value() - a ) );
	}

};

template <typename Pixel>
struct FunctorUnder
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		dst = (Channel)( A * ( boost::gil::channel_traits<Channel>::max_value() - b ) + B );
	}

};

template <typename Pixel>
struct FunctorXOR
	: public boost::gil::merge_functor<Pixel, fun_op_alpha_t>
{
	//@todo: this functor only work on floats, on int types it
	//       needs to be specialized
	template <typename Channel>
	inline void operator()( const Channel& A, const Channel& B, Channel& dst )
	{
		static const Channel max = boost::gil::channel_traits<Channel>::max_value();

		dst = (Channel)( A * ( max - b ) + B * ( max - a ) );
	}

};

}
}

#endif //GIL_FUNCTORS_HPP
