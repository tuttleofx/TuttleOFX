#ifndef _TERRY_COLOR_COLORSPACE_BASE_HPP_
#define _TERRY_COLOR_COLORSPACE_BASE_HPP_

#include <boost/mpl/vector.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/locator.hpp>
#include <boost/gil/typedefs.hpp>

#include <terry/numeric/init.hpp>

namespace terry
{
using namespace ::boost::gil;

#define TERRY_DEFINE_GIL_INTERNALS_4(CS)                                                                                    \
                                                                                                                            \
    template <typename IC>                                                                                                  \
    inline typename type_from_x_iterator<planar_pixel_iterator<IC, color::CS##_colorspace_t> >::view_t planar_##CS##_view(  \
        std::size_t width, std::size_t height, IC a, IC b, IC c, IC d, std::ptrdiff_t rowsize_in_bytes)                     \
    {                                                                                                                       \
        typedef typename type_from_x_iterator<planar_pixel_iterator<IC, color::CS##_colorspace_t> >::view_t RView;          \
        return RView(                                                                                                       \
            width, height,                                                                                                  \
            typename RView::locator(planar_pixel_iterator<IC, color::CS##_colorspace_t>(a, b, c, d), rowsize_in_bytes));    \
    }

#define TERRY_DEFINE_GIL_INTERNALS_3(CS)                                                                                    \
                                                                                                                            \
    template <typename IC>                                                                                                  \
    inline typename type_from_x_iterator<planar_pixel_iterator<IC, color::CS##_colorspace_t> >::view_t planar_##CS##_view(  \
        std::size_t width, std::size_t height, IC a, IC b, IC c, std::ptrdiff_t rowsize_in_bytes)                           \
    {                                                                                                                       \
        typedef typename type_from_x_iterator<planar_pixel_iterator<IC, color::CS##_colorspace_t> >::view_t RView;          \
        return RView(width, height, typename RView::locator(planar_pixel_iterator<IC, color::CS##_colorspace_t>(a, b, c),   \
                                                            rowsize_in_bytes));                                             \
    }

// A customized version of the equivalent GIL define.
// We only have removed the forward declarations
// which creates compilation troubles, with terry namespace...
#define TERRY_GIL_DEFINE_BASE_TYPEDEFS_INTERNAL(T, CS, LAYOUT)                                                              \
    typedef pixel<bits##T, LAYOUT> CS##T##_pixel_t;                                                                         \
    typedef const pixel<bits##T, LAYOUT> CS##T##c_pixel_t;                                                                  \
    typedef pixel<bits##T, LAYOUT>& CS##T##_ref_t;                                                                          \
    typedef const pixel<bits##T, LAYOUT>& CS##T##c_ref_t;                                                                   \
    typedef CS##T##_pixel_t* CS##T##_ptr_t;                                                                                 \
    typedef CS##T##c_pixel_t* CS##T##c_ptr_t;                                                                               \
    typedef memory_based_step_iterator<CS##T##_ptr_t> CS##T##_step_ptr_t;                                                   \
    typedef memory_based_step_iterator<CS##T##c_ptr_t> CS##T##c_step_ptr_t;                                                 \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##_ptr_t> > CS##T##_loc_t;                              \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##c_ptr_t> > CS##T##c_loc_t;                            \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##_step_ptr_t> > CS##T##_step_loc_t;                    \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##c_step_ptr_t> > CS##T##c_step_loc_t;                  \
    typedef image_view<CS##T##_loc_t> CS##T##_view_t;                                                                       \
    typedef image_view<CS##T##c_loc_t> CS##T##c_view_t;                                                                     \
    typedef image_view<CS##T##_step_loc_t> CS##T##_step_view_t;                                                             \
    typedef image_view<CS##T##c_step_loc_t> CS##T##c_step_view_t;                                                           \
    typedef image<CS##T##_pixel_t, false, std::allocator<unsigned char> > CS##T##_image_t;

// A customized version of the equivalent GIL define
// CS = 'bgr' CS_FULL = 'rgb_t' LAYOUT='bgr_layout_t'
#define TERRY_GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(T, CS, CS_FULL, LAYOUT)                                                      \
    TERRY_GIL_DEFINE_BASE_TYPEDEFS_INTERNAL(T, CS, LAYOUT)                                                                  \
    typedef planar_pixel_reference<bits##T&, CS_FULL> CS##T##_planar_ref_t;                                                 \
    typedef planar_pixel_reference<const bits##T&, CS_FULL> CS##T##c_planar_ref_t;                                          \
    typedef planar_pixel_iterator<bits##T*, CS_FULL> CS##T##_planar_ptr_t;                                                  \
    typedef planar_pixel_iterator<const bits##T*, CS_FULL> CS##T##c_planar_ptr_t;                                           \
    typedef memory_based_step_iterator<CS##T##_planar_ptr_t> CS##T##_planar_step_ptr_t;                                     \
    typedef memory_based_step_iterator<CS##T##c_planar_ptr_t> CS##T##c_planar_step_ptr_t;                                   \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##_planar_ptr_t> > CS##T##_planar_loc_t;                \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##c_planar_ptr_t> > CS##T##c_planar_loc_t;              \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##_planar_step_ptr_t> > CS##T##_planar_step_loc_t;      \
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##c_planar_step_ptr_t> > CS##T##c_planar_step_loc_t;    \
    typedef image_view<CS##T##_planar_loc_t> CS##T##_planar_view_t;                                                         \
    typedef image_view<CS##T##c_planar_loc_t> CS##T##c_planar_view_t;                                                       \
    typedef image_view<CS##T##_planar_step_loc_t> CS##T##_planar_step_view_t;                                               \
    typedef image_view<CS##T##c_planar_step_loc_t> CS##T##c_planar_step_view_t;                                             \
    typedef image<CS##T##_pixel_t, true, std::allocator<unsigned char> > CS##T##_planar_image_t;

#define TERRY_DEFINE_ALL_TYPEDEFS(T, CS)                                                                                    \
    TERRY_GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(T, CS, color::CS##_colorspace_t, color::CS##_layout_t)

#define TERRY_DEFINE_COLORSPACE_STANDARD_TYPEDEFS(CS)                                                                       \
    TERRY_DEFINE_ALL_TYPEDEFS(8, CS)                                                                                        \
    TERRY_DEFINE_ALL_TYPEDEFS(8s, CS)                                                                                       \
    TERRY_DEFINE_ALL_TYPEDEFS(16, CS)                                                                                       \
    TERRY_DEFINE_ALL_TYPEDEFS(16s, CS)                                                                                      \
    TERRY_DEFINE_ALL_TYPEDEFS(32, CS)                                                                                       \
    TERRY_DEFINE_ALL_TYPEDEFS(32s, CS)                                                                                      \
    TERRY_DEFINE_ALL_TYPEDEFS(32f, CS)

namespace color
{

/**
 * @brief Base class of all color parameters class.
 */
struct IColorParams
{
    virtual bool operator==(const IColorParams& other) const = 0;
    bool operator!=(const IColorParams& other) const { return !this->operator==(other); };
};

/**
 * @brief Fake class to finish hierachy.
 */
struct None
{
};
/**
 * @brief Fake class to finish hierachy.
 */
struct IsRootReference
{
    typedef None reference;
    typedef None params;
};
}
}

#endif
