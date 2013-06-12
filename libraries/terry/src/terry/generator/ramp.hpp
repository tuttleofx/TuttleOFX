#ifndef _TERRY_GENERATOR_RAMP_HPP_
#define _TERRY_GENERATOR_RAMP_HPP_

#include <boost/gil/utilities.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>

#include <cmath>

namespace terry {
namespace generator {

template<class Pixel>
Pixel clearChannel( const Pixel& in, bool red, bool green, bool blue, bool alpha, const boost::mpl::true_, const boost::mpl::true_ )
{
	Pixel out = in;
	if( red )
		get_color( out, red_t() ) = 0.0;
	if( green )
		get_color( out, green_t() ) = 0.0;
	if( blue )
		get_color( out, blue_t() ) = 0.0;
	if( alpha )
		get_color( out, gray_color_t() ) = 0.0;
	
	return out;
}

template<class Pixel>
Pixel clearChannel( const Pixel& in, bool red, bool green, bool blue, bool alpha, const boost::mpl::true_, const boost::mpl::false_ )
{
	Pixel out = in;
	if( red )
		get_color( out, red_t() ) = 0.0;
	if( green )
		get_color( out, green_t() ) = 0.0;
	if( blue )
		get_color( out, blue_t() ) = 0.0;
	
	return out;
}

template<class Pixel>
Pixel clearChannel( const Pixel& in, bool red, bool green, bool blue, bool alpha, const boost::mpl::false_, const boost::mpl::true_ )
{
	return in;
}

template<class Pixel>
Pixel clearChannel( const Pixel& in, bool red, bool green, bool blue, bool alpha, const boost::mpl::false_, const boost::mpl::false_ )
{
	return in;
}

// Models a Unary Function
template <typename Pixel>
// Models PixelValueConcept
struct RampFunctor
{
	//typedef point2<ptrdiff_t>    point_t;
	typedef boost::gil::point2<double>    point_t;
	
	typedef RampFunctor const_t;
	typedef Pixel value_type;
	typedef value_type reference;
	typedef value_type const_reference;
	typedef point_t argument_type;
	typedef reference result_type;
	typedef typename contains_color<Pixel, red_t>::type hasRGBChannel;
	typedef typename contains_color<Pixel, gray_color_t>::type hasAlphaChannel;
	BOOST_STATIC_CONSTANT( bool, is_mutable = false );
	
	point_t _tile_size;
	
	value_type _start_color;
	value_type _end_color;
	
	bool _horizontal;
	bool _multiColor;
	
	RampFunctor() {}
	RampFunctor( const point_t& tileSize, const value_type& start_color, const value_type& end_color, bool horizontal = true, bool _multiColor = false ) :
		_tile_size   ( tileSize ),
		_start_color ( start_color ),
		_end_color   ( end_color ),
		_horizontal  ( horizontal ),
		_multiColor  ( _multiColor )
	{}
	
	Pixel operator()( const point_t& p ) const
	{
		Pixel pixel;
		
		float level = p.x / ( _tile_size.x - 1 );
		if( ! _horizontal )
		{
			level = p.y / ( _tile_size.y - 1 );
		}
		
		Pixel pixelStart = numeric::pixel_multiplies_scalar_t<Pixel, float>()( _start_color, 1.0 - level );
		Pixel pixelEnd   = numeric::pixel_multiplies_scalar_t<Pixel, float>()( _end_color, level );
		
		pixel = numeric::pixel_plus_t<Pixel, Pixel, Pixel>()( pixelStart, pixelEnd );
		
		if( !_multiColor )
		{
			return pixel;
		}
		
		if( _horizontal )
		{
			if( p.y < 0.25 * _tile_size.y )
			{
				pixel = clearChannel( pixel, false, true, true, false, hasRGBChannel(), hasAlphaChannel() );
			}
			else
			{
				if( p.y < 0.5 * _tile_size.y )
				{
					pixel = clearChannel( pixel, true, false, true, false, hasRGBChannel(), hasAlphaChannel() );
				}
				else
				{
					if( p.y < 0.75 * _tile_size.y )
					{
						pixel = clearChannel( pixel, true, true, false, false, hasRGBChannel(), hasAlphaChannel() );
					}
				}
			}
		}
		else
		{
			if( p.x < 0.25 * _tile_size.x )
			{
				pixel = clearChannel( pixel, false, true, true, false, hasRGBChannel(), hasAlphaChannel() );
			}
			else
			{
				if( p.x < 0.5 * _tile_size.x )
				{
					pixel = clearChannel( pixel, true, false, true, false, hasRGBChannel(), hasAlphaChannel() );
				}
				else
				{
					if( p.x < 0.75 * _tile_size.x )
					{
						pixel = clearChannel( pixel, true, true, false, false, hasRGBChannel(), hasAlphaChannel() );
					}
				}
			}
		}
		return pixel;
	}
	
};

}
}

#endif
