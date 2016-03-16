#ifndef _rounded_hpp_
#define _rounded_hpp_

#include <boost/gil/gil_all.hpp>
#include <blend.hpp>

namespace layer
{

template <typename view_t>
struct rounded
{
    typedef typename view_t::value_type color_t;
    color_t backcolor;
    color_t frontcolor;
    color_t bordercolor;
    rounded(color_t backcolor, color_t frontcolor, color_t bordercolor)
        : backcolor(backcolor)
        , frontcolor(frontcolor)
        , bordercolor(bordercolor)
    {
    }

    void operator()(view_t& view)
    {
        using namespace boost::gil;
#define ___ 0

        static unsigned char buffer2[] = {
            ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 255, 230, ___, ___, ___, ___, ___, ___,
            ___, ___, ___, ___, ___, ___, ___, 255, 220, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___,
            ___, 255, 170, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 140, 255, 140, ___, ___, ___,
            ___, ___, ___, ___, ___, ___, ___, ___, ___, 180, 255, 110, ___, ___, ___, ___, ___, ___, ___, ___, ___,
            ___, ___, ___, 220, 220, 065, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 180, 255, 150, ___,
            ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 255, 220, 070, ___, ___, ___, ___, ___, ___, ___,
            ___, ___, ___, ___, 255, 255, 170, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 180, 255, 180,
            ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 255, 255, 190, 060, ___, ___, ___, ___, ___, ___,
            ___, ___, ___, 180, 255, 255, 185, 060, ___, ___, ___, ___, ___, ___, ___, 140, 180, 220, 255, 215, 170,
            ___, ___, ___, ___, ___, ___, 255, 255, 255, 255, 255, 215, 165, 100, ___, ___, ___, ___, ___, ___, ___,
            230, 200, 170, 140, 110, 065, ___, ___, ___, ___, ___, ___, ___, ___, ___};

        static unsigned char buffer3[] = {
            ___, ___, ___, ___, ___, ___, ___, ___, ___, ___, 255, ___, ___, ___, ___, ___, ___, ___, ___, ___, 255,
            255, ___, ___, ___, ___, ___, ___, ___, ___, ___, 255, 255, ___, ___, ___, ___, ___, ___, ___, ___, 255,
            255, 255, ___, ___, ___, ___, ___, ___, ___, ___, 255, 255, 255, ___, ___, ___, ___, ___, ___, ___, 255,
            255, 255, 255, ___, ___, ___, ___, ___, ___, 255, 255, 255, 255, 255, ___, ___, ___, ___, ___, 255, 255,
            255, 255, 255, 255, ___, ___, ___, ___, 255, 255, 255, 255, 255, 255, 255, ___, ___, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

        int width = 15;
        int height = 15;

        boost::gil::fill_pixels(view, frontcolor);

        for(int x = width; x < view.width() - width; ++x)
        {
            view(x, 0) = bordercolor;
            view(x, 1) = bordercolor;
            view(x, view.height() - 1) = bordercolor;
            view(x, view.height() - 2) = bordercolor;
        }

        for(int y = height; y < view.height() - height; ++y)
        {
            view(0, y) = bordercolor;
            view(1, y) = bordercolor;
            view(view.width() - 1, y) = bordercolor;
            view(view.width() - 2, y) = bordercolor;
        }

        width = 11;
        height = 11;
        gray8c_view_t grayview = interleaved_view(width, height, (gray8_pixel_t*)buffer3, sizeof(unsigned char) * width);

        copy<alpha24_blend>(backcolor, rotated180_view(grayview), subimage_view(view, 0, 0, width, height));
        copy<alpha24_blend>(backcolor, grayview,
                            subimage_view(view, view.width() - width, view.height() - height, width, height));
        copy<alpha24_blend>(backcolor, rotated90ccw_view(grayview),
                            subimage_view(view, view.width() - width, 0, width, height));
        copy<alpha24_blend>(backcolor, rotated90cw_view(grayview),
                            subimage_view(view, 0, view.height() - height, width, height));

        width = 15;
        height = 15;
        grayview = interleaved_view(width, height, (gray8_pixel_t*)buffer2, sizeof(unsigned char) * width);

        copy<alpha24_blend>(bordercolor, rotated180_view(grayview), subimage_view(view, 0, 0, width, height));
        copy<alpha24_blend>(bordercolor, grayview,
                            subimage_view(view, view.width() - width, view.height() - height, width, height));
        copy<alpha24_blend>(bordercolor, rotated90ccw_view(grayview),
                            subimage_view(view, view.width() - width, 0, width, height));
        copy<alpha24_blend>(bordercolor, rotated90cw_view(grayview),
                            subimage_view(view, 0, view.height() - height, width, height));
    }
};
}

#endif
