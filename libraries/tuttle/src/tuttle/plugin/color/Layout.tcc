
#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/color_base_algorithm.hpp>


namespace tuttle {
namespace plugin {
namespace color {

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
	bits32f red, green, blue;

	//If saturation is 0, the color is a shade of gray
	if( abs( get_color( src, green_t() )) < 0.0001f  )
	{
		// If saturation is 0, the color is a shade of gray
		red   = get_color( src, blue_t() );
		green = get_color( src, blue_t() );
		blue  = get_color( src, blue_t() );
	}
	else
	{
		bits32f frac, p, q, t, h;
		bits32 i;

		//to bring hue to a number between 0 and 6, better for the calculations
		h = get_color( src, red_t() );
		h *= 6.f;

		i = (int) floor( h );

		frac = h - i;

		p = get_color( src, blue_t() ) * ( 1.f -   get_color( src, green_t() ));
		q = get_color( src, blue_t() ) * ( 1.f - ( get_color( src, green_t() ) * frac ));
		t = get_color( src, blue_t() ) * ( 1.f - ( get_color( src, green_t() ) * ( 1.f - frac )));

		switch( i )
		{
			case 0:
			{
				//red	= get_color( src, blue_t() );
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

	get_color(dst,red_t())		= channel_convert<typename color_element_type< DstP, red_t	>::type>( red	);
	get_color(dst,green_t())	= channel_convert<typename color_element_type< DstP, green_t	>::type>( green	);
	get_color(dst,blue_t())		= channel_convert<typename color_element_type< DstP, blue_t	>::type>( blue	);
}

template < typename SrcP, typename DstP >
void convertRgbToHsv( const SrcP& src, DstP& dst )
{
	bits32f hue, saturation, value;

	bits32f temp_red	= channel_convert<bits32f>( get_color( src, red_t()	));
	bits32f temp_green	= channel_convert<bits32f>( get_color( src, green_t()	));
	bits32f temp_blue	= channel_convert<bits32f>( get_color( src, blue_t()	));

	bits32f min_color	= (std::min)( temp_red, (std::min)( temp_green, temp_blue ));
	bits32f max_color	= (std::max)( temp_red, (std::max)( temp_green, temp_blue ));

	value = max_color;

	bits32f diff = max_color - min_color;

	if( max_color < 0.0001f )
	{
		saturation = 0.f;
	}
	else
	{
		saturation = diff / max_color;
	}


	if( saturation < 0.0001f )
	{
		//it doesn't matter what value it has
		hue = 0.f;
	}
	else
	{
		if( (std::abs)( temp_red - max_color ) < 0.0001f )
		{
			hue =       ( temp_green - temp_blue ) / diff;
		}
		else if( temp_green == max_color )
		{
			hue = 2.f + ( temp_blue  - temp_red  ) / diff;
		}
		else
		{
			hue = 4.f + ( temp_red  - temp_green ) / diff;
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

// using rgb point with:
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
	get_color( dst, red_t() )	=  0.299    * get_color( src, red_t() ) + 0.587    * get_color( src, green_t() ) + 0.114    * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.168736 * get_color( src, red_t() ) - 0.331264 * get_color( src, green_t() ) + 0.5      * get_color( src, blue_t() );
	get_color( dst, blue_t() )	=  0.5      * get_color( src, red_t() ) - 0.418688 * get_color( src, green_t() ) - 0.081312 * get_color( src, blue_t() );
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
inline bool convertFromRgbLayout ( const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRGB() );
	return true;
}

/* ---------------------------------------- RGB -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToRgbLayout ( const SrcP& src, DstP& dst )
{
	static_for_each( src, dst, computeRGB() );
	return true;
}

/* ---------------------------------------- Yuv -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromYuvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromYuvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertYuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYuvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	convertYuvaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> Yuv -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToYuvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToYuvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToYuv( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToYuv( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToYuv( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToYuv ( src, dst );
	return true;
}

template < >
inline bool convertToYuvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	convertRgbaToYuva( src, dst );
	return true;
}

/* ---------------------------------------- YPbPr -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP>
inline bool convertFromYPbPrLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromYPbPrLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYPbPraToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYPbPrToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYPbPrLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYPbPraToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> YPbPr -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToYPbPrLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToYPbPrLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToYPbPra( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToYPbPra( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToYPbPra( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbToYPbPr( src, dst );
	return true;
}

template < >
inline bool convertToYPbPrLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToYPbPra( src, dst );
	return true;
}

/* ---------------------------------------- HSV -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromHsvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromHsvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertHsvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertHsvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertHsvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertHsvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertHsvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertHsvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHsvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHsvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHsvaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> HSV -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToHsvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToHsvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToHsva( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToHsva( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToHsva( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToHsv( src, dst );
	return true;
}

template < >
inline bool convertToHsvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToHsva( src, dst );
	return true;
}


/* ---------------------------------------- HSL -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromHslLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromHslLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertHslToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertHslaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertHslToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertHslaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertHslToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertHslaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHslToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromHslLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertHslaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> HSL -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToHslLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToHslLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToHsla( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToHsla( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToHsla( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToHsl( src, dst );
	return true;
}

template < >
inline bool convertToHslLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToHsla( src, dst );
	return true;
}

}
}
}
