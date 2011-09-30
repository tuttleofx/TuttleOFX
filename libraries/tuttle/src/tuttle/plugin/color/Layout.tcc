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
		bits32f frac, p, q, r, s, t, h, c;

		if( get_color( src, blue_t() ) > 0.5 )
			c = ( 1.f - get_color( src, blue_t() ) ) * 2.f * get_color( src, green_t() );
		else
			c = 2.f * get_color( src, blue_t() ) * get_color( src, green_t() );




		//to bring hue to a number between 0 and 6, better for the calculations
		// remark: hue is in [ 0; 6 [

		h = ( 1.f * get_color( src, red_t() ) - (float) std::floor( (float) get_color( src, red_t() ) ) ) * 6.0f;
		frac = h - (float) std::floor(h);


		p = 1.f * get_color( src, blue_t() ) - ( 0.5f * c );
		q = 1.f * get_color( src, blue_t() ) + ( 0.5f * c );

		r = 1.f * get_color( src, blue_t() ) + c * ( 0.5f - frac );
		t = 1.f * get_color( src, blue_t() ) + c * ( 0.5f - ( 1.f - frac ) );

		switch( (int) h )
		{
			case 0:
			{
				red	= q;
				green	= t;
				blue	= p;
				break;
			}

			case 6:
			{
				red	= q;
				green	= t;
				blue	= p;
				break;
			}

			case 1:
			{
				red	= r;
				green	= q;
				blue	= p;
				break;
			}

			case 2:
			{
				red	= p;
				green	= q;
				blue	= t;
				break;
			}

			case 3:
			{
				red	= p;
				green	= r;
				blue	= q;
				break;
			}

			case 4:
			{
				red	= t;
				green	= p;
				blue	= q;
				break;
			}

			case 5:
			{
				red	= q;
				green	= p;
				blue	= r;
				break;
			}
		}
	}

	get_color(dst,red_t())		= channel_convert<typename color_element_type< DstP, red_t	>::type>( red	);
	get_color(dst,green_t())	= channel_convert<typename color_element_type< DstP, green_t	>::type>( green	);
	get_color(dst,blue_t())		= channel_convert<typename color_element_type< DstP, blue_t	>::type>( blue	);

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

/*-------------------------------- RGB -> Lab  and Lab -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for L channel
// green channel for a channel
// blue  channel for b channel

template < typename SrcP, typename DstP >
void convertLabToRgb( const SrcP& src, DstP& dst )
{
	bits32f luminance		= get_color( src, red_t() );
	bits32f a_color_opponent	= get_color( src, green_t() );
	bits32f b_color_opponent	= get_color( src, blue_t() );

	// first, transfer to xyz color space
	bits32f normalized_y = ( luminance + 16.f ) / 116.f;
	bits32f normalized_x = ( a_color_opponent / 500.f ) + normalized_y;
	bits32f normalized_z = normalized_y - ( b_color_opponent / 200.f );

	if( pow( normalized_y, 3.f ) > 0.008856f )
	{
		normalized_y = pow( normalized_y, 3.f );
	}
	else
	{
		normalized_y = ( normalized_y - 16.f / 116.f ) / 7.787f;
	}

	if( pow( normalized_x, 3.f ) > 0.008856f )
	{
		normalized_x = pow( normalized_x, 3.f );
	}
	else
	{
		normalized_x = ( normalized_x - 16.f / 116.f ) / 7.787f;
	}

	if( pow( normalized_z, 3.f ) > 0.008856f )
	{
		normalized_z = pow( normalized_z, 3.f );
	}
	else
	{
		normalized_z = ( normalized_z - 16.f / 116.f ) / 7.787f;
	}

	bits32f reference_x = 95.047f;
	bits32f reference_y = 100.000f;
	bits32f reference_z = 108.883f;
	bits32f x, y, z;
	x = reference_x * normalized_x;
	y = reference_y * normalized_y;
	z = reference_z * normalized_z;

	// then, transfer to rgb color space
	normalized_x = x / 100.f;
	normalized_y = y / 100.f;
	normalized_z = z / 100.f;

	bits32f result_r = normalized_x *  3.2406f + normalized_y * -1.5372f + normalized_z * -0.4986f;
	bits32f result_g = normalized_x * -0.9689f + normalized_y *  1.8758f + normalized_z *  0.0415f;
	bits32f result_b = normalized_x *  0.0557f + normalized_y * -0.2040f + normalized_z *  1.0570f;

	if( result_r > 0.0031308f )
	{
		result_r = 1.055f * pow( result_r, 1.f/2.4f ) - 0.055f;
	}
	else
	{
		result_r = 12.92f * result_r;
	}

	if( result_g > 0.0031308f )
	{
		result_g = 1.055f * pow( result_g, 1.f/2.4f ) - 0.055f;
	}
	else
	{
		result_g = 12.92f * result_g;
	}

	if( result_b > 0.0031308f )
	{
		result_b = 1.055f * pow( result_b, 1.f/2.4f ) - 0.055f;
	}
	else
	{
		result_b = 12.92f * result_b;
	}

	bits32f red, green, blue;
	red   = result_r * 255.f;
	green = result_g * 255.f;
	blue  = result_b * 255.f;

	get_color(dst,red_t())  = channel_convert<typename color_element_type<DstP, red_t>::type>( red );
	get_color(dst,green_t())= channel_convert<typename color_element_type<DstP, green_t>::type>( green );
	get_color(dst,blue_t()) = channel_convert<typename color_element_type<DstP, blue_t>::type>( blue );
}

template < typename SrcP, typename DstP >
void convertRgbToLab( const SrcP& src, DstP& dst )
{
	// first, transfer to xyz color space
	SrcP xyzPoint;
	convertRgbToXYZ( src, xyzPoint );

	bits32f x, y, z;
	x = channel_convert<bits32f>( get_color( xyzPoint, red_t()   ));
	y = channel_convert<bits32f>( get_color( xyzPoint, green_t() ));
	z = channel_convert<bits32f>( get_color( xyzPoint, blue_t()  ));

	// then, transfer to lab color space
	bits32f ref_x = 0.372733f;//95.047f;
	bits32f ref_y = 0.392156f;//100.000f;
	bits32f ref_z = 0.395619f;//108.883f;
	bits32f normalized_x = 1.f * x / ref_x;
	bits32f normalized_y = 1.f * y / ref_y;
	bits32f normalized_z = 1.f * z / ref_z;

	if( normalized_x > 0.008856f )
	{
		normalized_x = pow( normalized_x, 0.333f );
	}
	else
	{
		normalized_x = (7.787f * normalized_x) + ( 16.f/116.f );
	}

	if( normalized_y > 0.008856f )
	{
		normalized_y = pow( normalized_y, 0.333f );
	}
	else
	{
		normalized_y = (7.787f * normalized_y) + ( 16.f/116.f );
	}

	if( normalized_z > 0.008856f )
	{
		normalized_z = pow( normalized_z, 0.333f );
	}
	else
	{
		normalized_z = ( 7.787f * normalized_z ) + ( 16.f/116.f );
	}

	bits32f luminance, a_color_opponent, b_color_opponent;
	luminance =( 0.45490196f * normalized_y ) - 0.0627451f; // ( 116.f * normalized_y ) - 16.f;
	a_color_opponent = 1.960784f * ( normalized_x - normalized_y );//500.f * ( normalized_x - normalized_y );
	b_color_opponent = 0.7843137f * ( normalized_y - normalized_z ); // 200.f * ( normalized_y - normalized_z );

	get_color( dst, red_t()   ) = luminance;
	get_color( dst, green_t() ) = a_color_opponent;
	get_color( dst, blue_t()  ) = b_color_opponent;
}

template < typename SrcP, typename DstP >
void convertLabaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertLabToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToLaba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToLab( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/*-------------------------------- RGB -> Luv  and Luv -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for L channel
// green channel for u channel
// blue  channel for v channel

template < typename SrcP, typename DstP >
void convertLuvToRgb( const SrcP& src, DstP& dst )
{
	get_color( dst, red_t() )	= get_color( src, red_t()   );
	get_color( dst, green_t() )	= get_color( src, green_t() );
	get_color( dst, blue_t() )	= get_color( src, blue_t()  );
}

template < typename SrcP, typename DstP >
void convertRgbToLuv( const SrcP& src, DstP& dst )
{
	get_color( dst, red_t() )	= get_color( src, red_t()   );
	get_color( dst, green_t() )	= get_color( src, green_t() );
	get_color( dst, blue_t() )	= get_color( src, blue_t()  );
}

template < typename SrcP, typename DstP >
void convertLuvaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertLuvToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToLuva( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToLuv( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/*-------------------------------- RGB -> XYZ  and XYZ -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for X channel
// green channel for Y channel
// blue  channel for Z channel

template < typename SrcP, typename DstP >
void convertXYZToRgb( const SrcP& src, DstP& dst )
{
	bits32f x = get_color( src, red_t() );
	bits32f y = get_color( src, green_t() );
	bits32f z = get_color( src, blue_t() );

	bits32f result_r = x *  3.2406f + y * -1.5372f + z * -0.4986f;
	bits32f result_g = x * -0.9689f + y *  1.8758f + z *  0.0415f;
	bits32f result_b = x *  0.0557f + y * -0.2040f + z *  1.0570f;

	get_color(dst,red_t  ()) = channel_convert<typename color_element_type<DstP, red_t  >::type>( result_r );
	get_color(dst,green_t()) = channel_convert<typename color_element_type<DstP, green_t>::type>( result_g );
	get_color(dst,blue_t ()) = channel_convert<typename color_element_type<DstP, blue_t >::type>( result_b );
}

template < typename SrcP, typename DstP >
void convertRgbToXYZ( const SrcP& src, DstP& dst )
{
	bits32f temp_red   = channel_convert<bits32f>( get_color( src, red_t()   ));
	bits32f temp_green = channel_convert<bits32f>( get_color( src, green_t() ));
	bits32f temp_blue  = channel_convert<bits32f>( get_color( src, blue_t()  ));

	bits32f x, y, z;
	x = temp_red * 0.4124240f + temp_green * 0.3575790f + temp_blue * 0.1804640f;
	y = temp_red * 0.2126560f + temp_green * 0.7151580f + temp_blue * 0.0721856f;
	z = temp_red * 0.0193324f + temp_green * 0.1191930f + temp_blue * 0.9504440f;

	get_color( dst, red_t() )	= x;
	get_color( dst, green_t() )	= y;
	get_color( dst, blue_t() )	= z;
}

template < typename SrcP, typename DstP >
void convertXYZaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertXYZToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToXYZa( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToXYZ( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

/*-------------------------------- RGB -> Yxy  and Yxy -> RGB Process ------------------------------------*/

// using poitn rgb with:
// red   channel for Y channel
// green channel for x channel
// blue  channel for y channel

template < typename SrcP, typename DstP >
void convertYxyToRgb( const SrcP& src, DstP& dst )
{
	DstP XYZPoint;
	if( get_color( src, green_t() ) == 0.f )
	{
		get_color( XYZPoint, red_t() )		= 0.f ;
		get_color( XYZPoint, green_t() )	= 0.f ;
		get_color( XYZPoint, blue_t() )		= 0.f ;
	}
	else
	{
		get_color( XYZPoint, red_t() )		= 1.f * get_color( src, green_t() ) * get_color( src, red_t() ) / get_color( src, blue_t() );
		get_color( XYZPoint, green_t() )	= get_color( src, red_t() );
		get_color( XYZPoint, blue_t() )		= (  1.f - get_color( src, green_t() ) - get_color( src, blue_t() ) ) * get_color( src, red_t() ) / get_color( src, blue_t() );
	}
	convertXYZToRgb( XYZPoint, dst );
}

template < typename SrcP, typename DstP >
void convertRgbToYxy( const SrcP& src, DstP& dst )
{
	SrcP XYZPoint;
	convertRgbToXYZ( src, XYZPoint );
	bits32f som = get_color( XYZPoint, red_t() ) + get_color( XYZPoint, green_t() ) + get_color( XYZPoint, blue_t() );
	if( som != 0.f )
	{
		get_color( dst, red_t() )	= get_color( XYZPoint, green_t() );
		get_color( dst, green_t() )	= get_color( XYZPoint, red_t() ) / som;
		get_color( dst, blue_t() )	= get_color( XYZPoint, green_t() ) / som;
	}
	else
	{
		get_color( dst, red_t() )	= 0.f;//0.31271f ;
		get_color( dst, green_t() )	= 0.f;//0.32902f ;
		get_color( dst, blue_t() )	= get_color( XYZPoint, green_t() );
	}
}

template < typename SrcP, typename DstP >
void convertYxyaToRgba( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertYxyToRgb( src, dst );
	get_color( dst, alpha_t() )	= channel_convert<typename color_element_type<DstP, alpha_t  >::type>( get_color( src, alpha_t() ) );
}

template < typename SrcP, typename DstP >
void convertRgbaToYxya( const SrcP& src, DstP& dst )
{
	//std::cout << "alpha is present" << std::endl;
	convertRgbToYxy( src, dst );
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

/* ---------------------------------------- Lab -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromLabLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromLabLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertLabToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertLabaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertLabToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertLabaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertLabToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertLabaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertLabToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLabLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertLabaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> Lab -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToLabLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToLabLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToLab( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToLaba( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToLab( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToLaba( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToLab( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToLaba( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToLab( src, dst );
	return true;
}

template < >
inline bool convertToLabLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToLaba( src, dst );
	return true;
}


/* ---------------------------------------- Luv -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromLuvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromLuvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertLuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertLuvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertLuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertLuvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertLuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertLuvaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertLuvToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromLuvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertLuvaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> Luv -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToLuvLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToLuvLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToLuv( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToLuva( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToLuv( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToLuva( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToLuv( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToLuva( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToLuv( src, dst );
	return true;
}

template < >
inline bool convertToLuvLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToLuva( src, dst );
	return true;
}

/* ---------------------------------------- XYZ -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromXYZLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromXYZLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertXYZToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertXYZaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertXYZToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertXYZaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertXYZToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertXYZaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertXYZToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromXYZLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertXYZaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> XYZ -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToXYZLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToXYZLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToXYZ( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToXYZa( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToXYZ( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToXYZa( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToXYZ( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToXYZa( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToXYZ( src, dst );
	return true;
}

template < >
inline bool convertToXYZLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToXYZa( src, dst );
	return true;
}

/* ---------------------------------------- Yxy -> RGB -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertFromYxyLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertFromYxyLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertYxyToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertYxyaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertYxyToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertYxyaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertYxyToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertYxyaToRgba( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYxyToRgb( src, dst );
	return true;
}

template < >
inline bool convertFromYxyLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertYxyaToRgba( src, dst );
	return true;
}

/* ---------------------------------------- RGB -> Yxy -----------------------------------------*/

template < typename SrcP, typename DstP >
inline bool convertToYxyLayout( const SrcP& src, DstP& dst )
{
	//std::cout << "Generique conversion"<< std::endl;
	return false;
}

template < >
inline bool convertToYxyLayout( const rgb8_pixel_t& src, rgb8_pixel_t& dst )
{
	convertRgbToYxy( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgba8_pixel_t& src, rgba8_pixel_t& dst )
{
	convertRgbaToYxya( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgb16_pixel_t& src, rgb16_pixel_t& dst )
{
	convertRgbToYxy( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgba16_pixel_t& src, rgba16_pixel_t& dst )
{
	convertRgbaToYxya( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgb32_pixel_t& src, rgb32_pixel_t& dst )
{
	convertRgbToYxy( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgba32_pixel_t& src, rgba32_pixel_t& dst )
{
	convertRgbaToYxya( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgb32f_pixel_t& src, rgb32f_pixel_t& dst )
{
	convertRgbToYxy( src, dst );
	return true;
}

template < >
inline bool convertToYxyLayout( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	//std::cout << "compute in 32 bits float" << std::endl;
	convertRgbaToYxya( src, dst );
	return true;
}

}
}
}

