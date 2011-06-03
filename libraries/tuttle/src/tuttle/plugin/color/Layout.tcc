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


/*-------------------------------- RGB -> YUV  and YUV -> RGB Process ------------------------------------*/

// using point rgb with:
// red   channel for Y channel
// green channel for U channel
// blue  channel for V channel

template < typename SrcP, typename DstP >
void convertYuvToRgb( const SrcP& src, DstP& dst )
{
	//std::cout << "convert YUV to RGB" << std::endl;
	get_color( dst, red_t() )	= get_color( src, red_t() )                                         + 1.13983 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= get_color( src, red_t() ) - 0.39465 * get_color( src, green_t() ) - 0.58060 * get_color( src, blue_t() );
	get_color( dst, blue_t() )	= get_color( src, red_t() ) + 2.03211 * get_color( src, green_t() )                                       ;

}

template < typename SrcP, typename DstP >
void convertRgbToYuv( const SrcP& src, DstP& dst )
{
	//std::cout << "convert RGB to YUV" << std::endl;
	get_color( dst, red_t() )	=  0.299   * get_color( src, red_t() ) + 0.587   * get_color( src, green_t() ) + 0.114   * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.14713 * get_color( src, red_t() ) - 0.28886 * get_color( src, green_t() ) + 0.436   * get_color( src, blue_t() );
	get_color( dst, blue_t() )	=  0.615   * get_color( src, red_t() ) - 0.51499 * get_color( src, green_t() ) - 0.10001 * get_color( src, blue_t() );

}

template < typename SrcP, typename DstP >
void convertYuvaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertYuvToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToYuva( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToYuv( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/*-------------------------------- RGB -> YPbPr  and YPbPr -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for Y channel
// green channel for Pb channel
// blue  channel for Pr channel

template < typename SrcP, typename DstP >
void convertYPbPrToRgb( const SrcP& src, DstP& dst )
{
	get_color( dst, red_t() )	= get_color( src, red_t() )                                         + 1.402    * get_color( src, green_t() );
	get_color( dst, green_t() )	= get_color( src, red_t() ) - 0.344136 * get_color( src, blue_t() ) - 0.714136 * get_color( src, green_t() );
	get_color( dst, blue_t() )	= get_color( src, red_t() ) + 1.772    * get_color( src, blue_t() )                                         ;
}

template < typename SrcP, typename DstP >
void convertYPbPrToYuv( const SrcP& src, DstP& dst )
{
	get_color( dst, red_t() )	=  0.299    * get_color( src, red_t() ) + 0.587    * get_color( src, green_t() ) + 0.114    * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.168736 * get_color( src, red_t() ) - 0.331264 * get_color( src, green_t() ) + 0.5      * get_color( src, blue_t() );
	get_color( dst, blue_t() )	=  0.5      * get_color( src, red_t() ) - 0.418688 * get_color( src, green_t() ) - 0.081312 * get_color( src, blue_t() );
}

template < typename SrcP, typename DstP >
void convertYPbPraToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertYPbPrToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertYPbPraToYuva( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToYPbPr( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/* --------------------------------- functions specifications ----------------------------------*/

/* ---------------------------------------- RGB -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::RGB > inColorSpace, const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRGB() );
	return true;
}

/* ---------------------------------------- Yuv -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YUV > inColorSpace, const SrcP& src, DstP& dst )
{
	std::cout << "Generique conversion"<< std::endl;
	return false;
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

/* ---------------------------------------- YPbPr -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const SrcP& src, DstP& dst )
{
	std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYPbPrToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::YPbPr > inColorSpace, const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYPbPraToRgba( src, dst );
	return true;
}

/* ---------------------------------------- HSV -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP, typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const SrcP& src, DstP& dst )
{
	std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYuvToRgb( src, dst );
	return true;
}

template < typename GradationlawIn >
bool convertLayout( const ttlc_colorspace< GradationlawIn, Layout::HSV > inColorSpace, const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYuvaToRgba( src, dst );
	return true;
}


/* ---------------------------------------- HSV -> RGB -----------------------------------------*/
/*
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
}*/

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
