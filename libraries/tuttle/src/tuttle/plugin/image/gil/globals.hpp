#ifndef _TUTTLE_PLUGIN_GIL_GLOBALS_HPP_
#define _TUTTLE_PLUGIN_GIL_GLOBALS_HPP_

#include <tuttle/common/math/minmax.hpp>

#include <ofxCore.h>

#include <boost/gil/gil_all.hpp>
#include <boost/type_traits.hpp>

#include <ostream>

namespace tuttle {
namespace plugin {

template<class View>
View getFullView( View tileView, const OfxRectI& bounds, const OfxRectI& rod )
{
	using namespace boost::gil;
	typedef typename View::value_type Pixel;

	// if the tile is equals to the full image
	// directly return the tile
	if( bounds.x1 == rod.x1 && bounds.y1 == rod.y1 &&
	    bounds.x2 == rod.x2 && bounds.y2 == rod.y2 )
		return tileView;

	// view the tile as a full image
	return subimage_view( tileView, rod.x1 - bounds.x1, rod.y1 - bounds.y1, rod.x2 - rod.x1, rod.y2 - rod.y1 );
}

/**
 * @todo tuttle: to rewrite !!!
 */
struct alpha_max_filler
{
	template< class P>
	inline P operator()( const P& p ) const
	{
		using namespace boost::gil;
		gil_function_requires<ColorSpacesCompatibleConcept<
		                          typename color_space_type<P>::type,
		                          rgba_t> >( );
		P p2;
		p2[3] = channel_traits< typename channel_type< P >::type >::max_value();
		return p2;
	}

};

/**
 * @todo tuttle: to rewrite !!!
 */
struct black_filler
{
	template< class P>
	inline P operator()( const P& p ) const
	{
		using namespace boost::gil;
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
// typedef typename view_type_from_pixel<OutPixelType, boost::gil::is_planar<View>::value >::type OutView;

/**
 * @brief Get black color value
 */
template<class Pixel>
static inline const Pixel get_black()
{
	using namespace boost::gil;
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
	using namespace boost::gil;
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
	using namespace boost::gil;
	transform_pixels( v, v, alpha_max_filler() );
}

/**
 * @brief Fill an image in black, all channels to 0.0 value and alpha channel to 1.0 (if exists)
 * @todo tuttle: to rewrite !!!
 */
template <class View>
void fill_black( View& v )
{
	using namespace boost::gil;
	transform_pixels( v, v, black_filler() );
	// Following doesn't work for built-in pixel types
	//	fill_pixels( v, get_black( v ) );
}

template <class View>
inline float max_value()
{
	using namespace boost::gil;
	return channel_traits< typename channel_type< View >::type >::max_value();
}

template <class View>
inline float domain_max_value()
{
	using namespace boost::gil;
	return channel_traits< typename channel_type< View >::type >::max_value() + 1.0f;
}

struct color_clamper_converter
{
	template <typename SrcP, typename DstP>
	inline void operator()( const SrcP& src, DstP& dst ) const
	{
		using namespace boost::gil;
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
inline typename boost::gil::color_converted_view_type<S_VIEW, DstP, color_clamper_converter>::type clamp( const S_VIEW& sView )
{
	using namespace boost::gil;
	return color_converted_view<DstP>( sView, color_clamper_converter() );
}

/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
boost::gil::point2<T> operator*( const boost::gil::point2<T>& p, const double t ) { return boost::gil::point2<T >( p.x * t, p.y * t ); }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
boost::gil::point2<T>& operator*=( boost::gil::point2<T>& p, const double t ) { p.x *= t; p.y *= t; return p; }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
boost::gil::point2<T> operator*( const boost::gil::point2<T>& a, const boost::gil::point2<T>& b ) { return boost::gil::point2<T>( a.x * b.x, a.y * b.y ); }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
boost::gil::point2<T>& operator*=( boost::gil::point2<T>& a, const boost::gil::point2<T>& b ) { a.x *= b.x; a.y *= b.y; return a; }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
boost::gil::point2<T> operator/( const boost::gil::point2<T>& a, const boost::gil::point2<T>& b ) { return boost::gil::point2<T>( a.x / b.x, a.y / b.y ); }
/// \ingroup PointModel
template <typename T>
GIL_FORCEINLINE
boost::gil::point2<double> operator/( const double t, const boost::gil::point2<T>& p )
{
	boost::gil::point2<double> res( 0, 0 );
	if( p.x != 0 )
		res.x = t / p.x;
	if( p.y != 0 )
		res.y = t / p.y;
	return res;
}

template <typename T>
std::ostream& operator<<( std::ostream& out, const boost::gil::point2<T>& p )
{
	return out << "x:" << p.x << " y:" << p.y;
}


}
}

#endif
