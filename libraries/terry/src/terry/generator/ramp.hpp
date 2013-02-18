#ifndef _TERRY_GENERATOR_RAMP_HPP_
#define _TERRY_GENERATOR_RAMP_HPP_

#include <boost/gil/utilities.hpp>

#include <cmath>

namespace terry {
namespace generator {

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
    BOOST_STATIC_CONSTANT( bool, is_mutable = false );

    int width;
    int height;

    bool horizontal;
    bool color;

    RampFunctor() {}
    RampFunctor( int width, int height, bool horizontal = true, bool color = false ) :
        width  ( width ),
        height ( height ),
        horizontal ( horizontal ),
        color ( color )
    {}

    Pixel operator()( const point_t& p ) const
    {
        Pixel pixel;

        float level = p.x / ( width - 1 );
        if( ! horizontal )
        {
            level = p.y / ( height - 1 );
        }

        if( !color )
        {
            color_convert( rgba32f_pixel_t( level, level, level, 1 ), pixel );
            return pixel;
        }

        if( horizontal )
        {
            if( p.y < 0.25 * height )
            {
                color_convert( rgba32f_pixel_t( level, 0, 0, 1 ), pixel );
            }
            else
            {
                if( p.y < 0.5 * height )
                {
                    color_convert( rgba32f_pixel_t( 0, level, 0, 1 ), pixel );
                }
                else
                {
                    if( p.y < 0.75 * height )
                    {
                        color_convert( rgba32f_pixel_t( 0, 0, level, 1 ), pixel );
                    }
                    else
                    {
                        color_convert( rgba32f_pixel_t( level, level, level, 1 ), pixel );
                    }
                }
            }
        }
        else
        {
            if( p.x < 0.25 * width )
            {
                color_convert( rgba32f_pixel_t( level, 0, 0, 1 ), pixel );
            }
            else
            {
                if( p.x < 0.5 * width )
                {
                    color_convert( rgba32f_pixel_t( 0, level, 0, 1 ), pixel );
                }
                else
                {
                    if( p.x < 0.75 * width )
                    {
                        color_convert( rgba32f_pixel_t( 0, 0, level, 1 ), pixel );
                    }
                    else
                    {
                        color_convert( rgba32f_pixel_t( level, level, level, 1 ), pixel );
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
