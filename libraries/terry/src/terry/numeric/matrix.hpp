#ifndef _TERRY_NUMERIC_MATRIX_HPP_
#define _TERRY_NUMERIC_MATRIX_HPP_

#include <terry/numeric/assign.hpp>

#include <boost/numeric/ublas/matrix.hpp>

namespace terry {
namespace numeric {

template <typename PixelRef, typename Matrix, typename PixelR = PixelRef> // models pixel concept
struct pixel_matrix33_multiply_t
{
	typedef typename channel_type<PixelR>::type ChannelR;
	const Matrix _matrix;

	pixel_matrix33_multiply_t( const Matrix& m )
	: _matrix(m)
	{}

	GIL_FORCEINLINE
	PixelR operator ( ) ( const PixelRef & p ) const
	{
		PixelR result;
		pixel_assigns_t<PixelRef, PixelR>()( p, result );
		//color_convert( p, result );
		result[0] = _matrix( 0, 0 ) * p[0] + _matrix( 0, 1 ) * p[1] + _matrix( 0, 2 ) * p[2];
		result[1] = _matrix( 1, 0 ) * p[0] + _matrix( 1, 1 ) * p[1] + _matrix( 1, 2 ) * p[2];
		result[2] = _matrix( 2, 0 ) * p[0] + _matrix( 2, 1 ) * p[1] + _matrix( 2, 2 ) * p[2];
		return result;
	}
};

template <typename PixelRef, typename Matrix, typename PixelR = PixelRef> // models pixel concept
struct pixel_matrix44_multiply_t
{
	typedef typename channel_type<PixelR>::type ChannelR;
	const Matrix _matrix;

	pixel_matrix44_multiply_t( const Matrix& m )
		: _matrix(m)
	{}

	GIL_FORCEINLINE
	PixelR operator ( ) ( const PixelRef & p ) const
	{
		PixelR result;
		result[0] = _matrix( 0, 0 ) * p[0] + _matrix( 0, 1 ) * p[1] + _matrix( 0, 2 ) * p[2] + _matrix( 0, 3 ) * p[3];
		result[1] = _matrix( 1, 0 ) * p[0] + _matrix( 1, 1 ) * p[1] + _matrix( 1, 2 ) * p[2] + _matrix( 1, 3 ) * p[3];
		result[2] = _matrix( 2, 0 ) * p[0] + _matrix( 2, 1 ) * p[1] + _matrix( 2, 2 ) * p[2] + _matrix( 2, 3 ) * p[3];
		result[3] = _matrix( 3, 0 ) * p[0] + _matrix( 3, 1 ) * p[1] + _matrix( 3, 2 ) * p[2] + _matrix( 3, 3 ) * p[3];
		return result;
	}
};

}
}

#endif
