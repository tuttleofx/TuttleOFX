// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _sections_hpp_
#define _sections_hpp_

#include <boost/function.hpp>
#include <boost/gil/gil_all.hpp>

struct sections
{
    struct section
    {
        int pos;
        section* next;
    };

    section* head;
    section* current;

    operator bool() { return current != NULL; }
    operator section*() { return head; }
    double operator*() { return current->pos; }
    void operator++(int) { current = current->next; }
    void operator++() { current = current->next; }
    void reset() { current = head; }

    ~sections()
    {
        while(head)
        {
            section* tmp = head->next;
            free(head);
            head = tmp;
        }

        head = NULL;
    }

    sections(int width, int intervals)
        : head(NULL)
        , current(NULL)
    {
        BOOST_ASSERT(intervals);

        int adj = width / intervals;
        int extra = width - adj * intervals;
        int pos = 0;
        int ppos = 0;

        head = NULL;
        current = NULL;

        while(pos < width)
        {
            int add = adj;
            if(extra-- > 0)
                add++;

            section* tmp = (section*)malloc(sizeof(section));
            pos += add;
            tmp->pos = pos - ppos;
            tmp->next = NULL;
            ppos = pos;

            if(head == NULL)
                head = tmp;
            else
                current->next = tmp;

            current = tmp;
        }

        current = head;
    }
};

namespace layer
{

template <typename view_t>
struct columns
{
    typedef boost::function<void(view_t&)> layer_t;
    typedef std::pair<double, layer_t> pair_t;

    std::vector<pair_t> layers;
    int margin;

    columns(pair_t* p, int size, int margin = 0)
        : margin(margin)
    {
        for(int n = 0; n < size; ++n)
            layers.push_back(p[n]);
    }

    void operator()(view_t& view)
    {
        int zcount = 0;
        int swidth = 0;
        for(int n = 0; n < layers.size(); ++n)
        {
            BOOST_ASSERT(layers[n].first >= 0);
            if(layers[n].first < 1 && layers[n].first > 0)
                layers[n].first = (int)(layers[n].first * view.width());

            swidth += (int)(layers[n].first);
            zcount += layers[n].first == 0 ? 1 : 0;
        }

        sections curr(view.width() - swidth + margin, zcount);
        int x = 0;
        for(int n = 0; n < layers.size(); ++n)
        {
            if(!layers[n].first)
            {
                layers[n].first = *curr;
                curr++;
            }

            int xwidth = (int)(layers[n].first);
            xwidth -= margin;

            view_t v = boost::gil::subimage_view(view, x, 0, xwidth, view.height());
            layers[n].second(v);
            x += xwidth + margin;
        }
    }
};

template <typename view_t>
struct rows
{
    typedef boost::function<void(view_t&)> layer_t;
    typedef std::pair<double, layer_t> pair_t;
    typedef std::vector<std::pair<double, layer_t> > layers_t;

    int margin;
    layers_t layers;

    rows(pair_t* p, int size, int margin = 0)
        : margin(margin)
    {
        for(int n = 0; n < size; ++n)
            layers.push_back(p[n]);
    }

    void operator()(view_t& view)
    {
        int zcount = 0;
        int sheight = 0;
        for(int n = 0; n < layers.size(); ++n)
        {
            BOOST_ASSERT(layers[n].first >= 0);
            if(layers[n].first < 1 && layers[n].first > 0)
                layers[n].first = (int)(layers[n].first * view.height());

            sheight += (int)(layers[n].first);
            zcount += layers[n].first == 0 ? 1 : 0;
        }

        sections curr(view.height() - sheight + margin, zcount);
        int y = 0;
        for(int n = 0; n < layers.size(); ++n)
        {
            if(!layers[n].first)
            {
                layers[n].first = *curr;
                curr++;
            }

            int yheight = (int)(layers[n].first);
            yheight -= margin;

            view_t v = boost::gil::subimage_view(view, 0, y, view.width(), yheight);
            layers[n].second(v);
            y += yheight + margin;
        }
    }
};
};

#endif
