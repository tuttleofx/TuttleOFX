#ifndef _TERRY_COLOR_COMPONENTS_HPP_
#define _TERRY_COLOR_COMPONENTS_HPP_

#include <boost/gil/gil_config.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_base_algorithm.hpp>

namespace terry {
namespace color {
namespace components{

using namespace boost::gil;

enum EConvertToGray{
	eConvertToGrayMean = 0,
	eConvertToGrayRec601,
	eConvertToGrayRec709,
	eConvertToGraySelectRed,
	eConvertToGraySelectGreen,
	eConvertToGraySelectBlue,
	eConvertToGraySelectAlpha
};

struct ConvertionParameters{
	EConvertToGray grayMethod;
	bool           premultiplied;
	ConvertionParameters():
		grayMethod(eConvertToGrayRec709),
		premultiplied(false)
	{}
};

// overload GIL to be sure to do unpremultiplied conversion
template < typename PixelSrc, typename PixelDst >
struct notPremutliplied_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, red_t() )   = get_color( src, red_t() );
		get_color( dst, green_t() ) = get_color( src, green_t() );
		get_color( dst, blue_t() )  = get_color( src, blue_t() );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct notPremutlipliedRGBA_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		return PixelDst( src );
	}
};

template < typename PixelSrc, typename PixelDst >
struct premutliplied_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, red_t() )   = 1.f * get_color( src, red_t() )   * get_color( src, alpha_t() );
		get_color( dst, green_t() ) = 1.f * get_color( src, green_t() ) * get_color( src, alpha_t() );
		get_color( dst, blue_t() )  = 1.f * get_color( src, blue_t() )  * get_color( src, alpha_t() );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct premutlipliedRGBA_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, red_t() )   = 1.f * get_color( src, red_t() )   * get_color( src, alpha_t() );
		get_color( dst, green_t() ) = 1.f * get_color( src, green_t() ) * get_color( src, alpha_t() );
		get_color( dst, blue_t() )  = 1.f * get_color( src, blue_t() )  * get_color( src, alpha_t() );
		get_color( dst, alpha_t() ) = get_color( src, alpha_t() );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayMean_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = 1.f/3 * ( get_color( src, red_t() ) + get_color( src, green_t() ) + get_color( src, blue_t() ) );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayRec601_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		// Y' = 0.299 R' + 0.587 G' + 0.114 B'
		get_color( dst, gray_color_t() ) = ( 0.299f * get_color( src, red_t() ) + 0.587f * get_color( src, green_t() ) + 0.114f * get_color( src, blue_t() ) );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayRec709_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		// Y' = 0.2126 R' + 0.7152 G' + 0.0722 B'
		get_color( dst, gray_color_t() ) = ( 0.2126f * get_color( src, red_t() ) + 0.7152f * get_color( src, green_t() ) + 0.0722f * get_color( src, blue_t() ) );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayFromRed_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, red_t() );
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayFromGreen_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, green_t() ) ;
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayFromBlue_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, blue_t() ) ;
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayFromAlpha_t
{
	GIL_FORCEINLINE
	/*PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, alpha_t() ) ;
		return dst;
	}*/
	PixelDst operator()( const rgba32f_pixel_t& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, alpha_t() ) ;
		return dst;
	}
	PixelDst operator()( const rgba16_pixel_t& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, alpha_t() ) ;
		return dst;
	}
	PixelDst operator()( const rgba8_pixel_t& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = get_color( src, alpha_t() ) ;
		return dst;
	}
	PixelDst operator()( const rgb32f_pixel_t& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = 0 ;
		return dst;
	}
	PixelDst operator()( const rgb16_pixel_t& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = 0 ;
		return dst;
	}
	PixelDst operator()( const rgb8_pixel_t& src ) const
	{
		PixelDst dst ;
		get_color( dst, gray_color_t() ) = 0 ;
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct rgbToRgba_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, red_t() )   = get_color( src, red_t() ) ;
		get_color( dst, green_t() ) = get_color( src, green_t() ) ;
		get_color( dst, blue_t() )  = get_color( src, blue_t() ) ;
		get_color( dst, alpha_t() ) = 0 ;
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayToRgb_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, red_t() )   = get_color( src, gray_color_t() ) ;
		get_color( dst, green_t() ) = get_color( src, gray_color_t() ) ;
		get_color( dst, blue_t() )  = get_color( src, gray_color_t() ) ;
		return dst;
	}
};

template < typename PixelSrc, typename PixelDst >
struct grayToRgba_t
{
	GIL_FORCEINLINE
	PixelDst operator()( const PixelSrc& src ) const
	{
		PixelDst dst ;
		get_color( dst, red_t() )   = get_color( src, gray_color_t() ) ;
		get_color( dst, green_t() ) = get_color( src, gray_color_t() ) ;
		get_color( dst, blue_t() )  = get_color( src, gray_color_t() ) ;
		get_color( dst, alpha_t() ) = 0 ;
		return dst;
	}
};

template<class SView, class DView>
DView& convertPremultipliedView( SView& src, DView& dst, bool premultiplied=false )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );
	if( premultiplied )
	{
		transform_pixels(
				src,
				dst,
				premutliplied_t< typename SView::value_type, typename DView::value_type >() );
	}
	else
	{
		transform_pixels(
				src,
				dst,
				notPremutliplied_t< typename SView::value_type, typename DView::value_type >() );
	}
	return dst;
}

template<class SView, class DView>
DView& convertPremultipliedRGBAView( SView& src, DView& dst, bool premultiplied=false )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );
	if( premultiplied )
	{
		transform_pixels(
				src,
				dst,
				premutlipliedRGBA_t< typename SView::value_type, typename DView::value_type >() );
	}
	else
	{
		transform_pixels(
				src,
				dst,
				notPremutlipliedRGBA_t< typename SView::value_type, typename DView::value_type >() );
	}
	return dst;
}

template<class SView, class DView>
DView& convertToGrayView( SView& src, DView& dst, EConvertToGray& grayMethod, bool premultiplied=false )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );
	switch( grayMethod )
	{
		case eConvertToGrayMean :
			transform_pixels(
				src,
				dst,
				grayMean_t< typename SView::value_type, typename DView::value_type >() );
			break;
		case eConvertToGrayRec601 :
			transform_pixels(
				src,
				dst,
				grayRec601_t< typename SView::value_type, typename DView::value_type >() );
			break;
		case eConvertToGrayRec709 :
			transform_pixels(
				src,
				dst,
				grayRec709_t< typename SView::value_type, typename DView::value_type >() );
			break;
		case eConvertToGraySelectRed :
			transform_pixels(
				src,
				dst,
				grayFromRed_t< typename SView::value_type, typename DView::value_type >() );
			break;
		case eConvertToGraySelectGreen :
			transform_pixels(
				src,
				dst,
				grayFromGreen_t< typename SView::value_type, typename DView::value_type >() );
			break;
		case eConvertToGraySelectBlue :
			transform_pixels(
				src,
				dst,
				grayFromBlue_t< typename SView::value_type, typename DView::value_type >() );
			break;
		case eConvertToGraySelectAlpha :
			transform_pixels(
				src,
				dst,
				grayFromAlpha_t< typename SView::value_type, typename DView::value_type >() );
			break;
	}
	return dst;
}

template<class SView, class DView>
DView& addAlphaChannelOnView( SView& src, DView& dst, bool premultiplied=false )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );

	transform_pixels(
		src,
		dst,
		rgbToRgba_t< typename SView::value_type, typename DView::value_type >() );
	return dst;
}

template<class SView, class DView>
DView& convertToRgbViewAndAddAlpha( SView& src, DView& dst, bool unpremultiplied=false )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );
	
	transform_pixels(
		src,
		dst,
		grayToRgba_t< typename SView::value_type, typename DView::value_type >() );
	return dst;
}

template<class SView, class DView>
DView& convertToRgbView( SView& src, DView& dst, bool unpremultiplied=false )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );
	
	transform_pixels(
		src,
		dst,
		grayToRgb_t< typename SView::value_type, typename DView::value_type >() );
	return dst;
}

// generic template: use for identical components:
// RGB  => RGB
// Gray => Gray
template<class SView, class DView>
void convertComponentsView( SView& src, DView& dst, ConvertionParameters& parameters )
{
	assert( src.dimensions() == dst.dimensions() );
	assert( src.dimensions() == dst.dimensions() );
	// if SView is similar to DView, only copy
	copy_and_convert_pixels( src, dst );
}

template<>
void convertComponentsView<rgba32f_view_t, rgba32f_view_t>( rgba32f_view_t& src, rgba32f_view_t& dst, ConvertionParameters& parameters )
{
	convertPremultipliedRGBAView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba16_view_t, rgba16_view_t>( rgba16_view_t& src, rgba16_view_t& dst, ConvertionParameters& parameters )
{
	convertPremultipliedRGBAView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba8_view_t, rgba8_view_t>( rgba8_view_t& src, rgba8_view_t& dst, ConvertionParameters& parameters )
{
	convertPremultipliedRGBAView( src, dst, parameters.premultiplied );
}


template<>
void convertComponentsView<rgba32f_view_t, rgb32f_view_t>( rgba32f_view_t& src, rgb32f_view_t& dst, ConvertionParameters& parameters )
{
	convertPremultipliedView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba16_view_t, rgb16_view_t>( rgba16_view_t& src, rgb16_view_t& dst, ConvertionParameters& parameters )
{
	convertPremultipliedView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba8_view_t, rgb8_view_t>( rgba8_view_t& src, rgb8_view_t& dst, ConvertionParameters& parameters )
{
	convertPremultipliedView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba32f_view_t, gray32f_view_t>( rgba32f_view_t& src, gray32f_view_t& dst, ConvertionParameters& parameters )
{
	convertToGrayView( src, dst, parameters.grayMethod, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba16_view_t, gray16_view_t>( rgba16_view_t& src, gray16_view_t& dst, ConvertionParameters& parameters )
{
	convertToGrayView( src, dst, parameters.grayMethod, parameters.premultiplied );
}

template<>
void convertComponentsView<rgba8_view_t, gray8_view_t>( rgba8_view_t& src, gray8_view_t& dst, ConvertionParameters& parameters )
{
	convertToGrayView( src, dst, parameters.grayMethod, parameters.premultiplied );
}

template<>
void convertComponentsView<rgb32f_view_t, rgba32f_view_t>( rgb32f_view_t& src, rgba32f_view_t& dst, ConvertionParameters& parameters )
{
	addAlphaChannelOnView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgb16_view_t, rgba16_view_t>( rgb16_view_t& src, rgba16_view_t& dst, ConvertionParameters& parameters )
{
	addAlphaChannelOnView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgb8_view_t, rgba8_view_t>( rgb8_view_t& src, rgba8_view_t& dst, ConvertionParameters& parameters )
{
	addAlphaChannelOnView( src, dst, parameters.premultiplied );
}

template<>
void convertComponentsView<rgb32f_view_t, gray32f_view_t>( rgb32f_view_t& src, gray32f_view_t& dst, ConvertionParameters& parameters )
{
	convertToGrayView( src, dst, parameters.grayMethod );
}

template<>
void convertComponentsView<rgb16_view_t, gray16_view_t>( rgb16_view_t& src, gray16_view_t& dst, ConvertionParameters& parameters )
{
	convertToGrayView( src, dst, parameters.grayMethod );
}

template<>
void convertComponentsView<rgb8_view_t, gray8_view_t>( rgb8_view_t& src, gray8_view_t& dst, ConvertionParameters& parameters )
{
	convertToGrayView( src, dst, parameters.grayMethod );
}

template<>
void convertComponentsView<gray32f_view_t, rgba32f_view_t>( gray32f_view_t& src, rgba32f_view_t& dst, ConvertionParameters& parameters )
{
	convertToRgbViewAndAddAlpha( src, dst );
}

template<>
void convertComponentsView<gray16_view_t, rgba16_view_t>( gray16_view_t& src, rgba16_view_t& dst, ConvertionParameters& parameters )
{
	convertToRgbViewAndAddAlpha( src, dst );
}

template<>
void convertComponentsView<gray8_view_t, rgba8_view_t>( gray8_view_t& src, rgba8_view_t& dst, ConvertionParameters& parameters )
{
	convertToRgbViewAndAddAlpha( src, dst );
}

template<>
void convertComponentsView<gray32f_view_t, rgb32f_view_t>( gray32f_view_t& src, rgb32f_view_t& dst, ConvertionParameters& parameters )
{
	convertToRgbView( src, dst );
}

template<>
void convertComponentsView<gray16_view_t, rgb16_view_t>( gray16_view_t& src, rgb16_view_t& dst, ConvertionParameters& parameters )
{
	convertToRgbView( src, dst );
}

template<>
void convertComponentsView<gray8_view_t, rgb8_view_t>( gray8_view_t& src, rgb8_view_t& dst, ConvertionParameters& parameters )
{
	convertToRgbView( src, dst );
}

}
}
}

#endif
