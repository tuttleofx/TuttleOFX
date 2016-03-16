// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _aligned_hpp_
#define _aligned_hpp_

#include <boost/gil/gil_all.hpp>
#include <blend.hpp>

namespace layer
{

template <typename view_t>
struct aligned
{
    enum
    {
        left = (0x1 << 0),
        center = (0x1 << 1),
        right = (0x1 << 2),
        top = (0x1 << 3),
        middle = (0x1 << 5),
        bottom = (0x1 << 7),
    };

    view_t& v2;
    int align;
    aligned(view_t v2, int align = center | middle)
        : v2(v2)
        , align(align)
    {
    }

    void operator()(view_t& view)
    {
        using namespace boost::gil;

        int w = v2.width();
        int h = v2.height();

        if(h > view.height() || w > view.width())
            return;

        int x = 0;
        if(align & center)
            x = (view.width() - w) / 2;
        else if(align & right)
            x = view.width() - w;

        int y = 0;
        if(align & middle)
            y = (view.height() - h) / 2;
        else if(align & bottom)
            y = view.height() - h;

        view_t v3 = subimage_view(view, x, y, w, h);
        boost::gil::copy_pixels(v2, v3);
    }
};
}

#endif
