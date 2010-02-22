/// \file
/// \brief Test file for convolve_rows() and convolve_cols() in the numeric extension
/// \author Lubomir Bourdev and Hailin Jin
/// \date February 27, 2007

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include <cmath>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace checkerboard {

// Models a Unary Function
template <typename P>   // Models PixelValueConcept
struct Checkerboard_fn
{
    //typedef point2<ptrdiff_t>    point_t;
    typedef boost::gil::point2<double>    point_t;

    typedef Checkerboard_fn      const_t;
    typedef P                    value_type;
    typedef value_type           reference;
    typedef value_type           const_reference;
    typedef point_t              argument_type;
    typedef reference            result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable=false);

    value_type                    _in_color,_out_color;
    point_t                       _tile_size, _tile_size_2;

    Checkerboard_fn() {}
    Checkerboard_fn(const point_t& tileSize, const value_type& in_color, const value_type& out_color)
    : _in_color(in_color), _out_color(out_color), _tile_size(tileSize), _tile_size_2(tileSize*2.0) {}

    result_type operator()(const point_t& p) const
    {
        const point_t mp( fmod( p.x, _tile_size_2.x), fmod( p.y, _tile_size_2.y ) );
        /*
        std::cout << "__________" << std::endl;
        std::cout << "p.x: " << p.x << " p.y: " << p.y << std::endl;
        std::cout << "mp.x: " << mp.x << " mp.y: " << mp.y << std::endl;
        std::cout << "_tile_size.x: " << _tile_size.x << " _tile_size.y: " << _tile_size.y << std::endl;
        */
        if( (mp.x > _tile_size.x) != (mp.y > _tile_size.y) )
            return _in_color;
        return _out_color;
    }
};

void test()
{
    using namespace boost::gil;
    typedef Checkerboard_fn<rgb32f_pixel_t> deref_t;
    typedef deref_t::point_t            point_t;
    typedef virtual_2d_locator<deref_t,false> locator_t;
    typedef image_view<locator_t> my_virt_view_t;

    boost::function_requires<PixelLocatorConcept<locator_t> >();
    gil_function_requires<StepIteratorConcept<locator_t::x_iterator> >();
    
    const double nbTiles = 5.0;
    point_t dims(500,880);
    double ratio = dims.y / dims.x;
    double shift = (1.0-ratio) / 2.0;
    //double shift = 0;
    point_t tileSize(dims.x/nbTiles, dims.x/nbTiles);
    my_virt_view_t mandel(point_t(dims.x, dims.x), locator_t(point_t(0,0), point_t(1,1), deref_t(tileSize, rgb32f_pixel_t(1.0,1.0,1.0), rgb32f_pixel_t(0.0,0.0,0.0))));
    png_write_view("out-Checkerboard-shift.png",color_converted_view<rgb8_pixel_t>(subimage_view<>(mandel, 0, (dims.x-dims.y)*0.5, dims.x, dims.y)));
    png_write_view("out-Checkerboard.png",color_converted_view<rgb8_pixel_t>(mandel));
}

}
}
}

