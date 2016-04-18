// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _shrink_hpp_
#define _shrink_hpp_

#include <boost/gil/gil_all.hpp>
#include <boost/function.hpp>

namespace layer
{

template <typename view_t>
struct shrink
{
    typedef boost::function<void(view_t&)> funct_t;

    double left;
    double top;
    double right;
    double bottom;
    funct_t funct;

    shrink(funct_t funct, double left = 5, double top = 5, double right = 5, double bottom = 5)
        : funct(funct)
        , left(left)
        , top(top)
        , right(right)
        , bottom(bottom)
    {
    }

    void operator()(view_t& view)
    {
        if(left < 1.0)
            left *= view.width();
        if(right < 1.0)
            right *= view.width();
        if(top < 1.0)
            top *= view.height();
        if(bottom < 1.0)
            bottom *= view.height();

        double width = view.width() - left - right;
        double height = view.height() - top - bottom;
        view_t view2 = boost::gil::subimage_view(view, (int)left, (int)top, (int)width, (int)height);
        funct(view2);
    }
};
}

#endif
