#ifndef _TERRY_GENERATOR_COLORCUBE_HPP_
#define _TERRY_GENERATOR_COLORCUBE_HPP_

#include <boost/gil/utilities.hpp>

#include <terry/numeric/operations.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>

#include <cmath>

namespace terry {
namespace generator {

// Models a Unary Function
template <typename Pixel>
// Models PixelValueConcept
struct ColorCubeFunctor
{
	typedef boost::gil::point2<double>    point_t;

	typedef ColorCubeFunctor const_t;
	typedef Pixel value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	point_t tileSize;
	double  scale;
	size_t step;

	ColorCubeFunctor() {}
	ColorCubeFunctor( const point_t& tileSize, const size_t step ) :
		tileSize( tileSize ),
		scale( 1.0 / tileSize.x ),
		step ( step )
	{}
	
	Pixel operator()( const point_t& p ) const
	{
		double x = scale * p.x - 0.5;
		double y = scale * p.y - 0.5;
		
		if( std::abs( x ) > 0.5 || std::abs( y ) > 0.5 )
		{
			Pixel pixel;
			numeric::pixel_zeros_t<Pixel>( )( pixel );
			return pixel;
		}
		
		size_t cubeStep = sqrt( (float) step );
		Pixel pixel;
		
		float red   = 0.0;
		float green = 0.0;
		float blue  = 0.0;

		float xStep = floor( scale * p.x * cubeStep );
		float yStep = floor( scale * p.y * cubeStep );

		red   = floor( scale * p.x * step * cubeStep ) / ( step - 1 ) - xStep - xStep * 1 / ( step - 1 );
		green = floor( scale * p.y * step * cubeStep ) / ( step - 1 ) - yStep - yStep * 1 / ( step - 1 );
		blue  = xStep / ( step - 1 ) + cubeStep * yStep / ( step - 1 );

		color_convert( rgba32f_pixel_t( red, green, blue, 1 ), pixel );
		return pixel;
	}

};

}
}

#endif
