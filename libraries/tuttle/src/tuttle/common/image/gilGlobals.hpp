#ifndef GIL_GLOBALS_HPP
#define GIL_GLOBALS_HPP

#include "tuttle/common/math/minmax.hpp"
#include <boost/gil/gil_all.hpp>
#include <boost/type_traits.hpp>

namespace tuttle {

using namespace boost::gil;

struct alpha_max_filler
{
    template< class P>
    P operator( ) ( const P& p ) const
    {
        gil_function_requires<ColorSpacesCompatibleConcept<
                                typename color_space_type<P>::type,
                                rgba_t> >();
        P p2;
        p2[3] = channel_traits< typename channel_type< P >::type >::max_value( );
        return p2;
    }
};

template <class View>
struct image_from_view
{
    typedef typename View::value_type value_type; // pixel_t
    typedef typename bgil::image<value_type, bgil::is_planar<View>::value> type;
};

/**
 * @brief Get black color value
 */
template<class View>
static inline typename View::value_type get_black( const View& view ) {
    typename View::value_type black;
    color_convert( bgil::gray8_pixel_t( 0 ), black );
    return black;
}

template<class View>
static inline typename View::value_type get_white( const View& view ) {
    typename View::value_type white;
    color_convert( bgil::gray8_pixel_t( channel_traits< typename channel_type< View >::type >::max_value( ) ), white );
    return white;
}

template <class View>
void fill_alpha_max(const View & view) {
    transform_pixels(view, view, alpha_max_filler());
}

/**
 * @brief Remplit une image en noir (tous les canneaux a 0 sauf la couche alpha a 1 (ou 255, ou ...))
 */
template <class View>
void fill_black( const View& view )
{
    fill_pixels( view, get_black(view) );
}

template <class View>
inline float max_value() {
    return channel_traits< typename channel_type< View >::type >::max_value( );
}

template <class View>
inline float domain_max_value() {
    return channel_traits< typename channel_type< View >::type >::max_value( ) + 1.0f;
}

struct color_clamper_converter {
    template <typename SrcP, typename DstP>
    void operator()(const SrcP& src,DstP& dst) const {
        typedef typename color_space_type<SrcP>::type SrcColorSpace;
        typedef typename color_space_type<DstP>::type DstColorSpace;
        SrcP tmpPix;
        for(int v = 0; v < num_channels<SrcP>::type::value; v++) {
            if ( src[v] > channel_traits< typename channel_type< SrcP >::type >::max_value( ) )
                tmpPix[v] = channel_traits< typename channel_type< SrcP >::type >::max_value( );
            else if ( src[v] < channel_traits< typename channel_type< SrcP >::type >::min_value( ) )
                tmpPix[v] = channel_traits< typename channel_type< SrcP >::type >::min_value( );
            else
                tmpPix[v] = src[v];
        }
        default_color_converter_impl<SrcColorSpace, DstColorSpace>()(tmpPix, dst);
    }
};

//views_get_ccv_type
template <typename DstP, typename S_VIEW> inline
typename color_converted_view_type<S_VIEW, DstP, color_clamper_converter>::type clamp( const S_VIEW& sView )
{
    return color_converted_view<DstP>(sView, color_clamper_converter());
}

/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bgil::point2<T> operator*( const bgil::point2<T>& p, double t ) { return bgil::point2<T > ( p.x*t, p.y * t ); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bgil::point2<T> operator*( const bgil::point2<T>& a, const bgil::point2<T>& b ) { return bgil::point2<T>( a.x*b.x, a.y*b.y ); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bgil::point2<T> & operator*=( bgil::point2<T>& p, double t ) { p.x *= t; p.y *= t; return p; }


/**
 * @brief Compute min & max value from a view
 *
 * @param[in]   view     Source view
 * @param[out]  max      maximum image value
 * @param[out]  min      minimum image value
 *
 */
template <typename View, typename T>
void 
maxmin(const View & view, T & max, T & min) {
    typedef typename View::x_iterator iterator;
    typedef typename channel_type<View>::type dPix_t;
    const int nc = view.num_channels();
    int w = view.width();
    int h = view.height();
    max = min = view(0, 0)[0];
    for (int y=0; y < h; ++y) {
        iterator view_it = view.row_begin(y);
        for (int x=0; x < w; ++x) {
            for (int c=0; c < nc; c++) {
                const dPix_t val = (*view_it)[c];
                if (val > max) {
                    max = val;
                } else if (val < min) {
                    min = val;
                }
            }
            view_it++;
        }
    }
}

/**
 * @brief Normalize a view (using contrast streching)
 *
 * @param[in, out]  dst     Source and destination view
 * @param[in]       a       lower limit
 * @param[in]       b       upper limit
 * @return Return the normalized image
 */
template <class S_VIEW, class D_VIEW, typename T>
D_VIEW &
normalize(const S_VIEW & src, D_VIEW & dst, const T a, const T b) {
    typedef typename S_VIEW::x_iterator sIterator;
    typedef typename D_VIEW::x_iterator dIterator;
    typedef typename channel_type<D_VIEW>::type dPix_t;
    dPix_t m, M;
    maxmin(dst, M, m);
    const float fm = m, fM = M;
    int w = dst.width();
    int h = dst.height();

    if (m==M) return fill(dst, 0);
    if (m != a || M != b) {
        int nc = dst.num_channels();
        for (int y=0; y < h; ++y) {
            sIterator src_it = src.row_begin(y);
            dIterator dst_it = dst.row_begin(y);
            for (int x=0; x < w; ++x) {
                for (int c=0; c < nc; c++) {
                    (*dst_it)[c] = (dPix_t)(((*src_it)[c] - fm)/(fM - fm)*(b - a) + a);
                }
                dst_it++;
                src_it++;
            }
        }
    }
    return dst;
}

template <class S_VIEW, class D_VIEW, typename T>
D_VIEW &
multiply(const S_VIEW & src, D_VIEW & dst, const T factor) {
    typedef typename S_VIEW::x_iterator sIterator;
    typedef typename D_VIEW::x_iterator dIterator;
    typedef typename channel_type<D_VIEW>::type dPix_t;

    const int nc = src.num_channels();
    const int w  = src.width();
    const int h  = src.height();
    int x, y, c;
    for(y = 0; y < h; y++) {
        sIterator src_it = src.row_begin(y);
        dIterator dst_it = dst.row_begin(y);
        for(x = 0; x < w; x++) {
            for(c = 0; c < nc; c++) {
                (*dst_it)[c] = (dPix_t)((*src_it)[c] * factor);
            }
            src_it++;
            dst_it++;
        }
    }
    return dst;
}


}

#endif
