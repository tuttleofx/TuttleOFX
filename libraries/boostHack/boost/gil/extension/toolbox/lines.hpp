// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _lines_hpp_
#define _lines_hpp_

#include <boost/gil/gil_all.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <channels.hpp>

namespace layer
{

template <typename view_t, typename dash_t = boost::mpl::vector_c<int, 1, 1, 0> >
struct uniform_horizontal_lines
{
    typedef typename view_t::value_type color_t;
    color_t color;
    int intervals;
    uniform_horizontal_lines(color_t color, int intervals)
        : color(color)
        , intervals(intervals)
    {
    }

    void operator()(view_t& view)
    {
        typedef typename view_t::value_type value_type_t;

        BOOST_ASSERT(view.width());

        std::vector<int> dash;
        boost::mpl::for_each<dash_t>(boost::bind(&std::vector<int>::push_back, &dash, _1));

        channels y(view.height(), intervals);

        for(; y; ++y)
            for(int x = 0; x < view.width();)
                for(int d = 0; d < dash.size(); ++d, ++x)
                    if(dash[d] && x < view.width())
                        view(x, *y) = color;
    }
};

template <typename view_t, typename dash_t = boost::mpl::vector_c<int, 1, 1, 0> >
struct uniform_vertical_lines
{
    typedef typename view_t::value_type color_t;
    color_t color;
    int intervals;
    uniform_vertical_lines(color_t color, int intervals)
        : color(color)
        , intervals(intervals)
    {
    }

    void operator()(view_t& view)
    {
        typedef typename view_t::value_type value_type_t;

        std::vector<int> dash;
        boost::mpl::for_each<dash_t>(boost::bind(&std::vector<int>::push_back, &dash, _1));

        channels x(view.width(), intervals);

        for(; x; x++)
            for(int y = 0; y < view.width();)
                for(int d = 0; d < dash.size(); ++d, ++y)
                    if(dash[d] && y < view.height())
                        view(*x, y) = color;
    }
};

template <typename view_t, typename dash_t = boost::mpl::vector_c<int, 1, 1, 1, 0> >
struct horizontal_line
{
    typedef typename view_t::value_type color_t;
    color_t color;
    double value, minvalue, maxvalue;
    horizontal_line(color_t color, double value, double minvalue, double maxvalue)
        : color(color)
        , minvalue(minvalue)
        , maxvalue(maxvalue)
        , value(value)
    {
    }

    void operator()(view_t& view)
    {
        std::vector<int> dash;
        boost::mpl::for_each<dash_t>(boost::bind(&std::vector<int>::push_back, &dash, _1));

        double diff = maxvalue - minvalue;
        if(diff == 0)
            return;

        double rval = 1 - ((value - minvalue) / (diff));
        int y = (int)(rval * (view.height() - 1));

        for(int x = 0; x < view.width();)
            for(int d = 0; d < dash.size(); ++d, ++x)
                if(dash[d] && x < view.width())
                    view(x, y) = color;
    }
};

template <typename view_t, typename dashlst_t = boost::mpl::vector_c<int, 1, 1, 0> >
struct vertical_lines
{
    typedef typename view_t::value_type color_t;
    color_t color;
    int width;
    int periods;
    vertical_lines(color_t color, int periods, int width)
        : color(color)
        , periods(periods)
        , width(width)
    {
    }

    void operator()(view_t& view)
    {
        std::vector<int> dashlst;
        boost::mpl::for_each<dashlst_t>(boost::bind(&std::vector<int>::push_back, &dashlst, _1));

        int intervals = view.width() / width;
        channels i(periods, intervals);
        channels x(view.width(), periods);

        for(int n = 0; i; ++i)
        {
            for(; n < *i; ++n, ++x)
                ;

            for(int y = 0; y < view.height();)
                for(std::size_t d = 0; d < dashlst.size(); ++d, ++y)
                    if(dashlst[d] && y < view.height())
                        view(*x, y) = color;
        }
    }
};
}

#endif
