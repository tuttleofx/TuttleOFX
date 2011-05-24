#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/color_base_algorithm.hpp>
using namespace boost::gil;
/*-------------------------------- method to process pixels values ------------------------------------*/

struct computeRGB
{
	template <typename SrcChannel, typename DstChannel>
	void operator()( SrcChannel& src, DstChannel& dst ) const
	{
		dst = DstChannel( src );
	}
};

struct computeYUV
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


/*
	B = 1.164(Y - 16)                   + 2.018(U - 128)
	G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
	R = 1.164(Y - 16) + 1.596(V - 128)
*/

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const SrcP& src, DstP& dst )
{
	return false;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const rgb_t& src, rgb_t& dst )
{
	// using:
	// red channel for Y channel
	// green channel for U channel
	// blue channel for V channel

	//typedef typename color_element_type<rgb_t, red_t>::type red_channel_t;
	//( *dst ) [0] = 0.5;
	//get_color(dst, red_t()) = channel_traits<red_channel_t>::max_value();
	//get_color( dst, red_t() )	= get_color( src, blue_t() );
	//get_color( dst, red_t() )	= channel_convert<typename color_element_type<DstP, red_t  >::type>( 1.164 * ( get_color( src, red_t() ) - 16 ) + 1.596 * ( get_color( src, blue_t() ) - 128 )							);
	//get_color( dst, green_t() )	= channel_convert<typename color_element_type<DstP, green_t>::type>( 1.164 * ( get_color( src, red_t() ) - 16 ) - 0.813 * ( get_color( src, blue_t() ) - 128 )	- 0.391 * ( get_color( src, green_t() ) - 128 )	);
	//get_color( dst, blue_t() )	= channel_convert<typename color_element_type<DstP, blue_t >::type>( 1.164 * ( get_color( src, red_t() ) - 16 )							+ 2.018 * ( get_color( src, green_t() ) - 128 )	);

	return true;
}

/*
template < typename SrcP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const SrcP& src, rgba_t& dst )
{
	// using:
	// red channel for Y channel
	// green channel for U channel
	// blue channel for V channel
	//get_color( dst, red_t() )	=  0.5 ;
	//get_color( dst, red_t() )	= channel_convert<typename color_element_type<DstP, red_t  >::type>( 1.164 * ( get_color( src, red_t() ) - 16 ) + 1.596 * ( get_color( src, blue_t() ) - 128 )							);
	//get_color( dst, green_t() )	= channel_convert<typename color_element_type<DstP, green_t>::type>( 1.164 * ( get_color( src, red_t() ) - 16 ) - 0.813 * ( get_color( src, blue_t() ) - 128 )	- 0.391 * ( get_color( src, green_t() ) - 128 )	);
	//get_color( dst, blue_t() )	= channel_convert<typename color_element_type<DstP, blue_t >::type>( 1.164 * ( get_color( src, red_t() ) - 16 )							+ 2.018 * ( get_color( src, green_t() ) - 128 )	);

	return true;
}*/

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
