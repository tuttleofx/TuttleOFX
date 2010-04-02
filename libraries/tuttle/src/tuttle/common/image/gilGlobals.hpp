#ifndef _GIL_GLOBALS_HPP_
#define _GIL_GLOBALS_HPP_

#include "tuttle/common/math/minmax.hpp"

#include <ofxCore.h>

#include <boost/gil/gil_all.hpp>
#include <boost/type_traits.hpp>

#include <ostream>

namespace tuttle {

namespace bgil = boost::gil;

struct alpha_max_filler
{
	template< class P>
	inline P operator()( const P& p ) const
	{
		using namespace bgil;
		gil_function_requires<ColorSpacesCompatibleConcept<
		                      typename color_space_type<P>::type,
		                      rgba_t> >( );
		P p2;
		p2[3] = channel_traits< typename channel_type< P >::type >::max_value();
		return p2;
	}

};

struct black_filler
{
	template< class P>
	inline P operator()( const P& p ) const
	{
		using namespace bgil;
		P p2;
		for( int v = 0; v < num_channels<P>::type::value; ++v )
		{
			p2[v] = 0;
		}
		return p2;
	}

};

template <class View>
struct image_from_view
{
	typedef typename View::value_type value_type; // pixel_t
	typedef typename boost::gil::image<value_type, boost::gil::is_planar<View>::value> type;
};

/**
 * @brief Get black color value
 */
template<class Pixel>
static inline const Pixel get_black()
{
	using namespace bgil;
	Pixel black;
	color_convert( gray32f_pixel_t( 0.0 ), black );
	return black;
}
template<class View>
static inline const typename View::value_type get_black( const View& )
{
	return get_black<typename View::value_type>();
}

template<class View>
static inline typename View::value_type get_white()
{
	using namespace bgil;
	typename View::value_type white;
	color_convert( gray32f_pixel_t( 1.0 ), white );
	return white;
}
template<class View>
static inline typename View::value_type get_white( const View& )
{
	return get_white<View>();
}

template <class View>
void fill_alpha_max( const View& v )
{
	using namespace bgil;
	transform_pixels( v, v, alpha_max_filler() );
}

/**
 * @brief Remplit une image en noir (tous les canneaux a 0 sauf la couche alpha a 1 (ou 255, ou ...))
 */
template <class View>
void fill_black( View& v )
{
	using namespace bgil;
	transform_pixels( v, v, black_filler() );
	// Following doesn't work for built-in pixel types
//	fill_pixels( v, get_black( v ) );
}

template <class View>
inline float max_value()
{
	using namespace bgil;
	return channel_traits< typename channel_type< View >::type >::max_value();
}

template <class View>
inline float domain_max_value()
{
	using namespace bgil;
	return channel_traits< typename channel_type< View >::type >::max_value() + 1.0f;
}

struct color_clamper_converter
{
	template <typename SrcP, typename DstP>
	inline void operator()( const SrcP& src, DstP& dst ) const
	{
		using namespace bgil;
		typedef typename color_space_type<SrcP>::type SrcColorSpace;
		typedef typename color_space_type<DstP>::type DstColorSpace;
		SrcP tmpPix;
		for( int v = 0; v < num_channels<SrcP>::type::value; ++v )
		{
			if( src[v] > channel_traits< typename channel_type< SrcP >::type >::max_value() )
				tmpPix[v] = channel_traits< typename channel_type< SrcP >::type >::max_value();
			else if( src[v] < channel_traits< typename channel_type< SrcP >::type >::min_value() )
				tmpPix[v] = channel_traits< typename channel_type< SrcP >::type >::min_value();
			else
				tmpPix[v] = src[v];
		}
		default_color_converter_impl<SrcColorSpace, DstColorSpace>() ( tmpPix, dst );
	}

};

template <typename DstP, typename S_VIEW>
inline typename bgil::color_converted_view_type<S_VIEW, DstP, color_clamper_converter>::type clamp( const S_VIEW& sView )
{
	using namespace bgil;
	return color_converted_view<DstP>( sView, color_clamper_converter() );
}

/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
bgil::point2<T> operator*( const bgil::point2<T>& p, const double t ) { return bgil::point2<T >( p.x * t, p.y * t ); }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
bgil::point2<T>& operator*=( bgil::point2<T>& p, const double t ) { p.x *= t; p.y *= t; return p; }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
bgil::point2<T> operator*( const bgil::point2<T>& a, const bgil::point2<T>& b ) { return bgil::point2<T>( a.x * b.x, a.y * b.y ); }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
bgil::point2<T>& operator*=( bgil::point2<T>& a, const bgil::point2<T>& b ) { a.x *= b.x; a.y *= b.y; return a; }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
bgil::point2<T> operator/( const bgil::point2<T>& a, const bgil::point2<T>& b ) { return bgil::point2<T>( a.x / b.x, a.y / b.y ); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bgil::point2<double> operator/( const double t, const bgil::point2<T>& p )
{
    bgil::point2<double> res(0,0);
    if( p.x != 0 )
        res.x = t / p.x;
    if( p.y != 0 )
        res.y = t / p.y;
    return res;
}

template <typename T>
std::ostream& operator<<( std::ostream& out, const bgil::point2<T>& p )
{
	return out << "x:" << p.x << " y:" << p.y;
}


inline bgil::point2<double> ofxToGil( const OfxPointD& p )
{
    return bgil::point2<double>( p.x, p.y );
}

inline OfxPointD gilToOfx( const bgil::point2<double>& p )
{
	OfxPointD r = { p.x, p.y };
    return r;
}

inline bgil::point2<int> ofxToGil( const OfxPointI& p )
{
    return bgil::point2<int>( p.x, p.y );
}

inline OfxPointI gilToOfx( const bgil::point2<int>& p )
{
	OfxPointI r = { p.x, p. y };
    return r;
}




/**
 * @brief Compute min & max value from a view
 *
 * @param[in]   view     Source view
 * @param[out]  max      maximum image value
 * @param[out]  min      minimum image value
 *
 */
template <typename View, typename T>
void maxmin( const View& view, T& max, T& min )
{
	using namespace bgil;
	typedef typename View::x_iterator iterator;
	typedef typename channel_type<View>::type dPix_t;
	const int nc = view.num_channels();
	int w        = view.width();
	int h        = view.height();
	max = min = view( 0, 0 )[0];
	for( int y = 0; y < h; ++y )
	{
		iterator view_it = view.row_begin( y );
		for( int x = 0; x < w; ++x )
		{
			for( int c = 0; c < nc; c++ )
			{
				const dPix_t val = ( *view_it )[c];
				if( val > max )
				{
					max = val;
				}
				else if( val < min )
				{
					min = val;
				}
			}
			++view_it;
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
D_VIEW& normalize( const S_VIEW& src, D_VIEW& dst, const T a, const T b )
{
	using namespace bgil;
	typedef typename S_VIEW::x_iterator sIterator;
	typedef typename D_VIEW::x_iterator dIterator;
	typedef typename channel_type<D_VIEW>::type dPix_t;
	dPix_t m, M;
	maxmin( dst, M, m );
	const float fm = m, fM = M;
	int w          = dst.width();
	int h          = dst.height();

	if( m == M )
		fill_black( dst );
	else if( m != a || M != b )
	{
		int nc = dst.num_channels();
		for( int y = 0; y < h; ++y )
		{
			sIterator src_it = src.row_begin( y );
			dIterator dst_it = dst.row_begin( y );
			for( int x = 0; x < w; ++x )
			{
				for( int c = 0; c < nc; c++ )
				{
					( *dst_it )[c] = (dPix_t)( ( ( *src_it )[c] - fm ) / ( fM - fm ) * ( b - a ) + a );
				}
				++dst_it;
				++src_it;
			}
		}
	}
	return dst;
}

template <class S_VIEW, class D_VIEW, typename T>
D_VIEW& multiply( const S_VIEW& src, D_VIEW& dst, const T factor )
{
	using namespace bgil;
	typedef typename S_VIEW::x_iterator sIterator;
	typedef typename D_VIEW::x_iterator dIterator;
	typedef typename channel_type<D_VIEW>::type dPix_t;

	const int nc = src.num_channels();
	const int w  = src.width();
	const int h  = src.height();
	int x, y, c;
	for( y = 0; y < h; y++ )
	{
		sIterator src_it = src.row_begin( y );
		dIterator dst_it = dst.row_begin( y );
		for( x = 0; x < w; x++ )
		{
			for( c = 0; c < nc; c++ )
			{
				( *dst_it )[c] = (dPix_t)( ( *src_it )[c] * factor );
			}
			++src_it;
			++dst_it;
		}
	}
	return dst;
}

}

#endif
