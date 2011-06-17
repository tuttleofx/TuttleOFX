
#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/color_base_algorithm.hpp>

using namespace boost::gil;

/*-------------------------------- RGB -> RGB  and RGB -> RGB Process ------------------------------------*/

struct computeRGB
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
	get_color( dst, red_t() )	= get_color( src, red_t() )                                          + 1.402    * get_color( src, blue_t() );
	get_color( dst, green_t() )	= get_color( src, red_t() ) - 0.344136 * get_color( src, green_t() ) - 0.714136 * get_color( src, blue_t() );
	get_color( dst, blue_t() )	= get_color( src, red_t() ) + 1.772    * get_color( src, green_t() )                                        ;
}

template < typename SrcP, typename DstP >
void convertRgbToYPbPr( const SrcP& src, DstP& dst )
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
void convertRgbaToYPbPra( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToYPbPr( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/*-------------------------------- RGB -> HSV  and HSV -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for H channel
// green channel for S channel
// blue  channel for V channel

template < typename SrcP, typename DstP >
void convertHsvToRgb( const SrcP& src, DstP& dst )
{
	bits32f red =0, green=0, blue=0;
	//bits32 i;

	//If saturation is 0, the color is a shade of gray
	if( get_color( src, green_t() ) == 0.0f  )
	{
		// If saturation is 0, the color is a shade of gray
		red   = get_color( src, blue_t() );
		green = get_color( src, blue_t() );
		blue  = get_color( src, blue_t() );
	}
	else
	{
		bits32f frac, p, q, t, h;


		//to bring hue to a number between 0 and 6, better for the calculations
		// remark: hue is in [ 0; 6 [

		/*h = 6.f * get_color( src, red_t() );
		if( h >= 1.f ) h -= 1.f;

		i = (char) ;

		frac = h - i;*/


		h = ( 1.f * get_color( src, red_t() ) - (float) std::floor( (float) get_color( src, red_t() ) ) ) * 6.0f;
		frac = h - (float) std::floor(h);


		p = 1.f * get_color( src, blue_t() ) * ( 1.f -   get_color( src, green_t() ) );

		q = 1.f * get_color( src, blue_t() ) * ( 1.f - ( get_color( src, green_t() ) * frac ) );
		t = 1.f * get_color( src, blue_t() ) * ( 1.f - ( get_color( src, green_t() ) * ( 1.f - frac ) ) );

		switch( (int) h )
		{
			case 0:
			{
				red	= get_color( src, blue_t() );
				green	= t;
				blue	= p;
				break;
			}

			case 6:
			{
				red	= get_color( src, blue_t() );
				green	= t;
				blue	= p;
				break;
			}

			case 1:
			{
				red	= q;
				green	= get_color( src, blue_t() );
				blue	= p;
				break;
			}

			case 2:
			{
				red	= p;
				green	= get_color( src, blue_t() );
				blue	= t;
				break;
			}

			case 3:
			{
				red	= p;
				green	= q;
				blue	= get_color( src, blue_t() );
				break;
			}

			case 4:
			{
				red	= t;
				green	= p;
				blue	= get_color( src, blue_t() );
				break;
			}

			case 5:
			{
				red	= get_color( src, blue_t() );
				green	= p;
				blue	= q;
				break;
			}
		}
	}

	/*bits32f a = get_color( src, red_t() );
	bits32f b = 0.1 * (get_color( src, red_t() ) * 6.0);
	bits32f c = 0.1*(std::floor( get_color( src, red_t() ) * 6.02 ));*/

	get_color(dst,red_t())		= channel_convert<typename color_element_type< DstP, red_t	>::type>( red	);
	get_color(dst,green_t())	= channel_convert<typename color_element_type< DstP, green_t	>::type>( green	);
	get_color(dst,blue_t())		= channel_convert<typename color_element_type< DstP, blue_t	>::type>( blue	);
}

template < typename SrcP, typename DstP >
void convertRgbToHsv( const SrcP& src, DstP& dst )
{
	bits32f hue = 0.0;
	bits32f saturation = 0.0;
	bits32f value = 0.0;

	bits32f temp_red	= channel_convert<bits32f>( get_color( src, red_t()	));
	bits32f temp_green	= channel_convert<bits32f>( get_color( src, green_t()	));
	bits32f temp_blue	= channel_convert<bits32f>( get_color( src, blue_t()	));

	bits32f min_color	= (std::min)( temp_red, (std::min)( temp_green, temp_blue ));
	bits32f max_color	= (std::max)( temp_red, (std::max)( temp_green, temp_blue ));

	value = max_color;

	bits32f diff = max_color - min_color;

	if( max_color == 0.0f )
	{
		saturation = 0.0f;
	}
	else
	{
		saturation = diff / max_color;
	}

	if( saturation == 0.0f )
	{
		// it's a gray colorh
		hue		= 0.0f;
	}
	else
	{
		if( max_color == temp_red )
		{
			hue =		1.f * ( temp_green - temp_blue ) / diff;
		}
		else if( max_color == temp_green )
		{
			hue = 2.f +	1.f * ( temp_blue  - temp_red  ) / diff;
		}
		else
		{
			hue = 4.f +	1.f * ( temp_red  - temp_green ) / diff;
		}

		//to bring it to a number between 0 and 1
		hue /= 6.f;

		if( hue < 0.f )
		{
			hue++;
		}
	}

	get_color( dst, red_t() )	= hue;
	get_color( dst, green_t() )	= saturation;
	get_color( dst, blue_t() )	= value;
}

template < typename SrcP, typename DstP >
void convertHsvaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertHsvToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToHsva( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToHsv( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/*-------------------------------- RGB -> HSL  and HSL -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for H channel
// green channel for S channel
// blue  channel for L channel

template < typename SrcP, typename DstP >
void convertHslToRgb( const SrcP& src, DstP& dst )
{
	get_color( dst, red_t() )	= get_color( src, red_t() )                                         + 1.402    * get_color( src, green_t() );
	get_color( dst, green_t() )	= get_color( src, red_t() ) - 0.344136 * get_color( src, blue_t() ) - 0.714136 * get_color( src, green_t() );
	get_color( dst, blue_t() )	= get_color( src, red_t() ) + 1.772    * get_color( src, blue_t() )                                         ;
}

template < typename SrcP, typename DstP >
void convertRgbToHsl( const SrcP& src, DstP& dst )
{
	// only bits32f for hsl is supported
	bits32f temp_red   = channel_convert<bits32f>( get_color( src, red_t()   ));
	bits32f temp_green = channel_convert<bits32f>( get_color( src, green_t() ));
	bits32f temp_blue  = channel_convert<bits32f>( get_color( src, blue_t()  ));

	bits32f hue, saturation, lightness;

	bits32f min_color = (std::min)( temp_red, (std::min)( temp_green, temp_blue ));
	bits32f max_color = (std::max)( temp_red, (std::max)( temp_green, temp_blue ));


	bits32f diff = max_color - min_color;

	if( diff == 0.0 )
	{
		// rgb color is gray

		hue		= 0.f;
		saturation	= 0.f;

		// doesn't matter which rgb channel we use.
		lightness	= temp_red;
	}
	else
	{
		// lightness calculation

		lightness = ( min_color + max_color ) * 0.5f;

		// saturation calculation

		if( lightness < 0.5f )
		{
			saturation = diff / ( min_color + max_color );
		}
		else
		{
			saturation = ( max_color - min_color ) / ( 2.f - diff );
		}

		// hue calculation
		if( max_color == temp_red )
		{
			// max_color is red
			hue = (double)( temp_green - temp_blue ) / diff;

		}
		else if( max_color == temp_green )
		{
			// max_color is green
			// 2.0 + (b - r) / (maxColor - minColor);
			hue = 2.f + (double)( temp_blue - temp_red ) / diff;
		}
		else
		{
			// max_color is blue
			hue = 4.f + (double)( temp_red - temp_green ) / diff;
		}

		if( hue < 0.f )
		{
			hue += 6.f;
		}

		hue /= 6.f;
	}

	get_color( dst, red_t() )	= hue;
	get_color( dst, green_t() )	= saturation;
	get_color( dst, blue_t() )	= lightness;
}

template < typename SrcP, typename DstP >
void convertHslaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertHslToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToHsla( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToHsl( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}


/* --------------------------------- functions specifications ----------------------------------*/

/* ---------------------------------------- RGB -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertFromRgbLayout ( const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRGB() );
	return true;
}

/* ---------------------------------------- RGB -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertToRgbLayout ( const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRGB() );
	return true;
}

/* ---------------------------------------- Yuv -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertFromYuvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertFromYuvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
bool convertFromYuvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> Yuv -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertToYuvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertToYuvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToYuv( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToYuv( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToYuv( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToYuv ( src, dst );
	return true;
}

template < >
bool convertToYuvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

/* ---------------------------------------- YPbPr -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP>
bool convertFromYPbPrLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertFromYPbPrLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
bool convertFromYPbPrLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYPbPraToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> YPbPr -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertToYPbPrLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertToYPbPrLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToYPbPra( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToYPbPra( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToYPbPra( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
bool convertToYPbPrLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToYPbPra( src, dst );
	return true;
}

/* ---------------------------------------- HSV -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertFromHsvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertFromHsvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertHsvToRgb( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertHsvaToRgba( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertHsvToRgb( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertHsvaToRgba( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertHsvToRgb( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertHsvaToRgba( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHsvToRgb( src, dst );
	return true;
}

template < >
bool convertFromHsvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHsvaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> HSV -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertToHsvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertToHsvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToHsva( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToHsva( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToHsva( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
bool convertToHsvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToHsva( src, dst );
	return true;
}


/* ---------------------------------------- HSL -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertFromHslLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertFromHslLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertHslToRgb( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertHslaToRgba( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertHslToRgb( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertHslaToRgba( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertHslToRgb( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertHslaToRgba( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHslToRgb( src, dst );
	return true;
}

template < >
bool convertFromHslLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHslaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> HSL -----------------------------------------*/

template < typename SrcP, typename DstP >
bool convertToHslLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
bool convertToHslLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToHsla( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToHsla( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToHsla( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
bool convertToHslLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToHsla( src, dst );
	return true;
}
