
#include <xmmintrin.h>
/*
inline __m128 _mm_madd_ps( const __m128& a, const __m128& b, const __m128& c)
{
	return _mm_add_ps( _mm_mul_ps( a, b), c);
}*/

void processConversion( rgba32f_pixel_t& src, rgba32f_pixel_t& dst, __m128& red, __m128& green, __m128& blue )
{
	//__m128 in = _mm_load_ps( ( float * ) src );
/*
	__m128 rrgg = _mm_unpacklo_ps ( in, in );
	__m128 bbaa = _mm_unpackhi_ps ( in, in );

	__m128 rrrr = _mm_unpacklo_ps ( rrgg, rrgg );
	__m128 gggg = _mm_unpackhi_ps ( rrgg, rrgg );
	__m128 bbbb = _mm_unpacklo_ps ( bbaa, bbaa );
	__m128 aaaa = _mm_unpackhi_ps ( bbaa, bbaa );

	__m128 rrrr = _mm_shuffle_ps ( in, in, _MM_SHUFFLE( 3, 3, 3, 3 ) );
	__m128 gggg = _mm_shuffle_ps ( in, in, _MM_SHUFFLE( 2, 2, 2, 2 ) );
*/
	//__m128 rgrg = _mm_shuffle_ps ( in, in, _MM_SHUFFLE( 3, 2, 3, 2 ) );
	//__m128 values = _mm_shuffle_ps ( red, green, _MM_SHUFFLE( 3, 2, 3, 2 ) );
}


template < typename SrcP, typename DstP >
void convertFromTempA	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempA	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.84520578f * get_color( src, red_t() ) + 0.00000009f * get_color( src, green_t() ) + 0.00000002 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000005f * get_color( src, red_t() ) + 0.82607192f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000001f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.23326041 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempB	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempB	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.24874067f * get_color( src, red_t() ) + 0.00000004f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 0.95096886f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.75399793 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempC	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempC	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.05152202f * get_color( src, red_t() ) + 0.00000017f * get_color( src, green_t() ) + 0.00000018 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 0.97455227f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 1.10034835 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempD50	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempD50	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.17609382f * get_color( src, red_t() ) + 0.00000007f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000006f * get_color( src, red_t() ) + 0.97570127f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 0.72197068 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempD55	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempD55	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.10405362f * get_color( src, red_t() ) + 0.00000012f * get_color( src, green_t() ) + 0.00000006 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 0.98688960f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.82335168 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempD58	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempD58	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.06815648f * get_color( src, red_t() ) + 0.00000016f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000004f * get_color( src, red_t() ) + 0.99201381f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 0.87833548 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempD65	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempD65	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  get_color( src, red_t  () );
	get_color( dst, green_t() )	=  get_color( src, green_t() );
	get_color( dst, blue_t()  )	=  get_color( src, blue_t () );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempD75	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempD75	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.92909920f * get_color( src, red_t() ) + 0.00000008f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 1.00641775f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 1.14529049 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTemp9300	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTemp9300	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.89304549f * get_color( src, red_t() ) + 0.00000012f * get_color( src, green_t() ) + 0.00000012 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 0.99430132f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 1.37155378 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}


template < typename SrcP, typename DstP >
void convertFromTempE	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempE	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.20491767f * get_color( src, red_t() ) + 0.00000008f * get_color( src, green_t() ) + 0.00000012 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000003f * get_color( src, red_t() ) + 0.94827831f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.90876031 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempF2	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempF2	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.34113419f * get_color( src, red_t() ) + 0.00000007f * get_color( src, green_t() ) + 0.00000006 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000004f * get_color( src, red_t() ) + 0.94260973f * get_color( src, green_t() ) - 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 0.56362498 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempF7	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempF7	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.00054073f * get_color( src, red_t() ) + 0.00000006f * get_color( src, green_t() ) + 0.00000012 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000003f * get_color( src, red_t() ) + 0.99999511f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.99845666 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempF11	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempF11	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.20491767f * get_color( src, red_t() ) + 0.00000008f * get_color( src, green_t() ) + 0.00000012 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000003f * get_color( src, red_t() ) + 0.94827831f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.90876031 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertFromTempDCIP3	( const SrcP& src, DstP& dst )
{

}

template < >
void convertFromTempDCIP3	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.88602084f * get_color( src, red_t() ) + 0.00000013f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000003f * get_color( src, red_t() ) + 1.04855490f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000000f * get_color( src, green_t() ) + 0.85470724 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}


template < typename SrcP, typename DstP >
void convertToTempA	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempA	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.54194504f * get_color( src, red_t() ) + 0.00000004f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= +0.00000008f * get_color( src, red_t() ) + 1.21054840f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	= -0.00000001f * get_color( src, red_t() ) - 0.00000006f * get_color( src, green_t() ) + 0.85470724 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempB	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempB	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.80080682f * get_color( src, red_t() ) + 0.00000004f * get_color( src, green_t() ) - 0.00000006 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000008f * get_color( src, red_t() ) + 1.05155921f * get_color( src, green_t() ) + 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000001f * get_color( src, red_t() ) + 0.00000003f * get_color( src, green_t() ) + 1.32802486 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempC	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempC	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.95100260f * get_color( src, red_t() ) + 0.00000006f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 1.02611196f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000001f * get_color( src, green_t() ) + 0.90880305 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempD50	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempD50	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.85027254f * get_color( src, red_t() ) + 0.00000001f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000001f * get_color( src, red_t() ) + 1.02490389f * get_color( src, green_t() ) - 0.00000004 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000001f * get_color( src, green_t() ) + 1.38509774 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempD55	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempD55	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.90575325f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 1.01328468f * get_color( src, green_t() ) - 0.00000004 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000001f * get_color( src, green_t() ) + 1.21454775 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempD58	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempD58	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.93619251f * get_color( src, red_t() ) + 0.00000009f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000001f * get_color( src, red_t() ) + 1.00805032f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	= -0.00000001f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 1.13851726 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempD65	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempD65	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  get_color( src, red_t  () );
	get_color( dst, green_t() )	=  get_color( src, green_t() );
	get_color( dst, blue_t()  )	=  get_color( src, blue_t () );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempD75	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempD75	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.07631159f * get_color( src, red_t() ) + 0.00000002f * get_color( src, green_t() ) + 0.00000006 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000001f * get_color( src, red_t() ) + 0.99362320f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.87314081 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTemp9300	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTemp9300	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.11976373f * get_color( src, red_t() ) - 0.00000004f * get_color( src, green_t() ) - 0.00000006 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000000f * get_color( src, red_t() ) + 1.00573123f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.72909999 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempE	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempE	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.82993227f * get_color( src, red_t() ) - 0.00000005f * get_color( src, green_t() ) - 0.00000009 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000005f * get_color( src, red_t() ) + 1.05454266f * get_color( src, green_t() ) + 0.00000003 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 1.10040021 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempF2	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempF2	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.74563771f * get_color( src, red_t() ) + 0.00000003f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000002f * get_color( src, red_t() ) + 1.06088448f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000003f * get_color( src, green_t() ) + 1.77422941 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempF7	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempF7	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.99945968f * get_color( src, red_t() ) - 0.00000008f * get_color( src, green_t() ) - 0.00000003 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000001f * get_color( src, red_t() ) + 1.00000501f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 1.00154579 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempF11	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempF11	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  0.70728123f * get_color( src, red_t() ) + 0.00000006f * get_color( src, green_t() ) + 0.00000006 * get_color( src, blue_t() );
	get_color( dst, green_t() )	= -0.00000002f * get_color( src, red_t() ) + 1.08209848f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	= -0.00000001f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 1.87809896 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

template < typename SrcP, typename DstP >
void convertToTempDCIP3	( const SrcP& src, DstP& dst )
{

}

template < >
void convertToTempDCIP3	( const rgba32f_pixel_t& src, rgba32f_pixel_t& dst )
{
	get_color( dst, red_t()   )	=  1.12864172f * get_color( src, red_t() ) - 0.00000001f * get_color( src, green_t() ) + 0.00000000 * get_color( src, blue_t() );
	get_color( dst, green_t() )	=  0.00000000f * get_color( src, red_t() ) + 0.95369333f * get_color( src, green_t() ) - 0.00000001 * get_color( src, blue_t() );
	get_color( dst, blue_t()  )	=  0.00000000f * get_color( src, red_t() ) + 0.00000000f * get_color( src, green_t() ) + 1.16999125 * get_color( src, blue_t() );
	get_color( dst, alpha_t() )	=  get_color( src, alpha_t() );
}

