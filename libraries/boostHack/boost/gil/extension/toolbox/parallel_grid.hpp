// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _parallel_grid_hpp_
#define _parallel_grid_hpp_

#include <vector>
#include <boost/gil/gil_all.hpp>
#include <boost/function.hpp>

#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

#include <sections.hpp>

namespace layer
{

template <typename view_t>
struct parallel_draw
{
    typedef boost::function<void(view_t&)> layer_t;

    std::vector<view_t> views;
    std::vector<layer_t> layers;
    parallel_draw(std::vector<layer_t>& layers, std::vector<view_t>& views)
        : views(views)
        , layers(layers)
    {
    }

    void operator()(const tbb::blocked_range<size_t>& blocks) const
    {
        BOOST_ASSERT(layers.size() == views.size());
        for(std::size_t block = blocks.begin(); block != blocks.end(); ++block)
        {
            view_t v = views[block];
            layers[block](v);
        }
    }
};

template <typename view_t>
struct parallel_grid
{
    typedef boost::function<void(view_t&)> layer_t;
    typedef std::vector<layer_t> layers_t;
    layers_t layers;
    int cols;
    int margin;

    parallel_grid(layer_t* p, int total, int cols, int margin = 5)
        : cols(cols)
        , margin(margin)
    {
        for(int n = 0; n < total; ++n)
            layers.push_back(p[n]);
    }

    void operator()(view_t& view)
    {
        using namespace boost::gil;

        int y = 0;
        std::vector<view_t> views;

        int rows = (int)ceil(layers.size() / (double)cols);
        sections ycurr(view.height() + margin, rows);
        for(; ycurr; ycurr++)
        {
            int yheight = *ycurr - margin;
            int x = 0;

            sections xcurr(view.width() + margin, cols);
            for(; xcurr; xcurr++)
            {
                int xwidth = *xcurr - margin;
                views.push_back(subimage_view(view, x, y, xwidth, yheight));
                x += xwidth + margin;
            }

            y += yheight + margin;
        }

        tbb::parallel_for(tbb::blocked_range<std::size_t>(0, views.size()), parallel_draw<view_t>(layers, views),
                          tbb::auto_partitioner());
    }
};
}

#endif
