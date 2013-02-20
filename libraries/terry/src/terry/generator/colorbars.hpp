#ifndef _TERRY_GENERATOR_COLORBARS_HPP_
#define _TERRY_GENERATOR_COLORBARS_HPP_

#include <boost/gil/utilities.hpp>

#include <cmath>

namespace terry {
namespace generator {

// Models a Unary Function
template <typename Pixel>
// Models PixelValueConcept
struct ColorBarsFunctor
{
	//typedef point2<ptrdiff_t>    point_t;
	typedef boost::gil::point2<double>    point_t;

	typedef ColorBarsFunctor const_t;
	typedef Pixel value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );

	int width1;
	int width2;
	int width3;
	int width4;
	int width5;
	int width6;
	
	float level;

	ColorBarsFunctor() {}
	ColorBarsFunctor( int width, bool is75percents ) :
		width1 ( 1.0 / 7 * width ),
		width2 ( 2.0 / 7 * width ),
		width3 ( 3.0 / 7 * width ),
		width4 ( 4.0 / 7 * width ),
		width5 ( 5.0 / 7 * width ),
		width6 ( 6.0 / 7 * width ),
		level  ( is75percents ? 0.75 : 1.0 )
	{}
	
	Pixel operator()( const point_t& p ) const
	{
		Pixel pixel;
		
		float red   = 0.0;
		float green = 0.0;
		float blue  = 0.0;

		if( p.x < width4 )
		{
			green = level;
		}
		if( p.x < width2 || ( p.x >= width4 && p.x < width6 ) )
		{
			red = level;
		}
		if( p.x < width1 || ( p.x >= width2 && p.x < width3 )  || ( p.x >= width4 && p.x < width5 )  || ( p.x >= width6 ) )
		{
			blue = level;
		}
		
		color_convert( rgba32f_pixel_t( red, green, blue, 1 ), pixel );
		return pixel;
	}

};

}
}

#endif
