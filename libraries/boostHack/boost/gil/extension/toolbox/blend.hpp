#ifndef _blend_hpp_
#define _blend_hpp_

#include <boost/gil/gil_all.hpp>

struct alpha24_blend
{
    short alpha;
    alpha24_blend(short alpha)
        : alpha(alpha)
    {
    }

    template <typename T>
    void operator()(T& dst, const T src)
    {
        dst = ((dst + 1) * alpha >> 8) - ((src + 1) * alpha >> 8) + src;
    }
};

template <typename blend_t, typename pixel_t, typename grayview_t, typename view_t>
inline void copy(pixel_t pixel, const grayview_t& grayview, const view_t& view)
{
    using namespace boost::gil;
    BOOST_ASSERT(grayview.width() == view.width());

    typedef typename view_t::x_iterator x_iterator_t;
    typedef typename grayview_t::x_iterator x_iterator2_t;

    for(int y = 0; y < view.height(); ++y)
    {
        x_iterator_t it_view = view.row_begin(y);
        x_iterator2_t it_gray = grayview.row_begin(y);
        for(int x = 0; x < view.width(); ++x)
        {
            pixel_t dst = pixel;
            static_for_each(dst, it_view[x], blend_t(it_gray[x]));
            it_view[x] = dst;
        }
    }
}

template <typename blend_t, typename grayview_t, typename view_t>
inline void copy(const grayview_t& gray, const view_t& bottom, const view_t& top)
{
    using namespace boost::gil;
    BOOST_ASSERT(gray.width() == bottom.width());
    BOOST_ASSERT(bottom.width() == top.width());

    typedef typename view_t::x_iterator x_iterator_t;
    typedef typename grayview_t::x_iterator x_iterator2_t;
    typedef typename view_t::value_type value_type_t;

    for(int y = 0; y < top.height(); ++y)
    {
        x_iterator_t it_top = top.row_begin(y);
        x_iterator_t it_bot = bottom.row_begin(y);
        x_iterator2_t it_gray = gray.row_begin(y);
        for(int x = 0; x < top.width(); ++x)
        {
            value_type_t dst = it_bot[x];
            static_for_each(dst, it_top[x], blend_t(it_gray[x]));
            it_top[x] = dst;
        }
    }
}

#endif
