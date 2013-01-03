#ifndef _TERRY_GENERATOR_COLORWHEEL_HPP_
#define _TERRY_GENERATOR_COLORWHEEL_HPP_

#include <boost/gil/color_convert.hpp>
#include <boost/gil/extension/color/hsl.hpp>
#include <boost/math/constants/constants.hpp>

#include <cmath>

namespace terry {
namespace generator {

// Models a Unary Function
template <typename Pixel>
// Models PixelValueConcept
struct ColorWheelFunctor
{
	//typedef point2<ptrdiff_t>    point_t;
	typedef boost::gil::point2<double>    point_t;

	typedef ColorWheelFunctor const_t;
	typedef Pixel value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	point_t tile_size;
	double scale;

	ColorWheelFunctor() {}
	ColorWheelFunctor( const point_t& tileSize ) :
		tile_size( tileSize ),
		scale( 1.0 / tileSize.x )
	{}
	
	Pixel operator()( const point_t& p ) const
	{
		Pixel pixel;
		
		float h = 1.0;
		float s = 1.0;
		float l = 1.0;
		
		double x = scale * p.x - 0.5;
		double y = scale * p.y - 0.5;
		
		if( x >= 0 )
		{
			h = - atan( y / x );
		}
		else
			h = ( boost::math::constants::pi<double>() - atan( y / x ) );
		
		h /= (2.0 * boost::math::constants::pi<double>() );
		
		l = sqrt( y * y + x * x );
		
		using namespace hsl_color_space;
		
		hsl32f_pixel_t hsl( h, s, l );
		rgb32f_pixel_t rgb;
		rgba32f_pixel_t rgba;
		
		color_convert( hsl, rgb );
		color_convert( rgb, rgba );
		
		if( sqrt( y * y + x * x ) > 0.5 )
		{
			get_color( rgba, red_t()   ) = 0.0;
			get_color( rgba, green_t() ) = 0.0;
			get_color( rgba, blue_t()  ) = 0.0;
			get_color( rgba, alpha_t() ) = 0.0;
		}
		
		color_convert( rgba, pixel );
		
		return pixel;
	}

};

}
}

#endif
