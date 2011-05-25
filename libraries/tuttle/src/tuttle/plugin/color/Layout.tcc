#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/color_base_algorithm.hpp>

using namespace boost::gil;

/*-------------------------------- method to process channels values ------------------------------------*/

struct computeRGB
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeYPbPr

{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeHSL
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeHSV
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeLab
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeLuv
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeXYZ
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeYxy
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

/*------------------------- methods to convert input to RGB --------------------------------*/
template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::RGB > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRGB() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const SrcP& src, DstP& dst )
{
	std::cout << "Generique conversion"<< std::endl;
	return false;
}

/*
	R = 1.0 * Y + 0.000 * U + 1.140 * V
	G = 1.0 * Y - 0.395 * U - 0.581 * V
	B = 1.0 * Y + 2.032 * U + 0.000 * V

	note: we have U and V value between [ 0 ... 1 ] and in the formula, U and V values needs to be in [ -0.5 ... 0.5 ]
*/
template < typename SrcP, typename DstP >
void convertYuvToRgb( const SrcP& src, DstP& dst )
{
	//std::cout << "convert YUV to RGB" << std::endl;
	// using src with:
	// green channel for Y channel
	// blue  channel for U channel
	// red   channel for V channel
	get_color( dst, red_t() )	= get_color( src, red_t() )                                                + 1.140 * ( get_color( src, green_t() ) - 0.5 );
	get_color( dst, green_t() )	= get_color( src, red_t() ) - 0.395 * ( get_color( src, blue_t() ) - 0.5 ) + 0.581 * ( get_color( src, green_t() ) - 0.5 );
	get_color( dst, blue_t() )	= get_color( src, red_t() ) + 2.032 * ( get_color( src, blue_t() ) - 0.5 )                                                ;

}

template < typename SrcP, typename DstP >
void convertYuvaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertYuvToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn>
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeYPbPr() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeHSV() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSL > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeHSL() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::Lab > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeLab() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::Luv > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeLuv() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::XYZ > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeXYZ() );
	return true;
}

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::Yxy > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeYxy() );
	return true;
}
