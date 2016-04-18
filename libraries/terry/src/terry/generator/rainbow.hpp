#ifndef _TERRY_GENERATOR_RAINBOW_HPP_
#define _TERRY_GENERATOR_RAINBOW_HPP_

#include <boost/gil/color_convert.hpp>
#include <boost/gil/extension/color/hsl.hpp>
#include <boost/math/constants/constants.hpp>

#include <terry/numeric/operations.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>

#include <cmath>

namespace terry
{
namespace generator
{

// Models a Unary Function
template <typename Pixel>
// Models PixelValueConcept
struct RainbowFunctor
{
    // typedef point2<ptrdiff_t>    point_t;
    typedef boost::gil::point2<double> point_t;

    typedef RainbowFunctor const_t;
    typedef Pixel value_type;
    typedef value_type reference;
    typedef value_type const_reference;
    typedef point_t argument_type;
    typedef reference result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable = false);

    point_t tile_size;
    double scale;

    RainbowFunctor() {}
    RainbowFunctor(const point_t& tileSize)
        : tile_size(tileSize)
        , scale(1.0 / tileSize.x)
    {
    }

    Pixel operator()(const point_t& p) const
    {
        Pixel pixel;

        float h = p.x / (tile_size.x - 1);
        float s = 1.0;
        float l = p.y / (tile_size.y - 1);

        using namespace hsl_color_space;

        hsl32f_pixel_t hsl(h, s, l);
        rgb32f_pixel_t rgb;
        rgba32f_pixel_t rgba;

        color_convert(hsl, rgb);
        color_convert(rgb, rgba);
        color_convert(rgba, pixel);

        return pixel;
    }
};
}
}

#endif
