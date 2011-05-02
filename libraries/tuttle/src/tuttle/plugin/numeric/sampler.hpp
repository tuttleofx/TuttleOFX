#ifndef _TUTTLE_PLUGIN_SAMPLER_HPP_
#define _TUTTLE_PLUGIN_SAMPLER_HPP_


#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/math/constants/constants.hpp>

#include <boost/math/constants/constants.hpp>

#include <cmath>

double valB = 0.0;
double valC = 0.0;

namespace boost {
namespace gil {


/// \brief A sampler that sets the destination pixel without interpolation from the closest pixels from the source.
// http://www.paulinternet.nl/?page=bicubic

struct ttl_nearest_neighbor_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_nearest_neighbor_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	point2<std::ptrdiff_t> center( iround( p ) );
	if( center.x < 0 ||
	    center.y < 0 ||
	    center.x > src.width() - 1 ||
	    center.y > src.height() - 1 )
	{
		return false;
	}

	result = src( center.x, center.y );
	return true;
}

namespace ttl_detail {

template <typename Weight>
struct add_dst_mul_src_channel
{
	const Weight _w;

	add_dst_mul_src_channel( const Weight w ) : _w( w ) { }

	template <typename SrcChannel, typename DstChannel>
		void operator( )( const SrcChannel& src, DstChannel & dst ) const
	{
		dst += DstChannel( src * _w );
	}
};

template <typename SrcP, typename Weight, typename DstP>
struct add_dst_mul_src
{

	void operator( )( const SrcP& src, const Weight weight, DstP & dst ) const
	{
		static_for_each( src, dst, add_dst_mul_src_channel<Weight > ( weight ) );
	}
};

}

/// \brief A sampler that sets the destination pixel as the bilinear interpolation of the four closest pixels from the source.
/// If outside the bounds, it doesn't change the destination
/// \ingroup ImageAlgorithms

struct ttl_bilinear_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_bilinear_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;

	// Interpolation of the four closest pixels
	// _________________________________
	// |               |               |
	// |               |               |
	// |      TL...............TR      |
	// |      :        |       :       |
	// |______:________|_______:_______|
	// |      :    x   |       :       | // x: floating point position of p
	// |      :        |       :       |
	// |      BL...............BR      |
	// |               |               |
	// |_______________|_______________|
	point2<std::ptrdiff_t> pTL( ifloor( p ) ); // the closest integer coordinate top left from p

	point2<F> frac( p.x - pTL.x, p.y - pTL.y );

	// if we are outside the image
	if( pTL.x < -1 ||
	    pTL.y < -1 ||
	    pTL.x > src.width( )-1 ||
	    pTL.y > src.height( )-1 )
	{
		return false;
	}

	if( pTL.x == -1 )
	{
		pTL.x = 0;
		frac.x = 0;
	}
	if( pTL.y == -1 )
	{
		pTL.y = 0;
		frac.y = 0;
	}
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > PixelW;
	PixelW mp( 0 ); // suboptimal

	typename SrcView::xy_locator loc = src.xy_at( pTL.x, pTL.y );

	if( pTL.x + 1 < src.width( ) )
	{
		if( pTL.y + 1 < src.height( ) )
		{
			// most common case - inside the image, not on the last row or column
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( *loc, ( 1.0 - frac.x )*( 1.0 - frac.y ), mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( loc.x( )[1], frac.x * ( 1.0 - frac.y ), mp );
			++loc.y( );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( *loc, ( 1.0 - frac.x ) * frac.y, mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( loc.x( )[1], frac.x * frac.y, mp );
		}
		else
		{
			// on the last row, but not the bottom-right corner pixel
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( *loc, ( 1.0 - frac.x ), mp );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( loc.x( )[1], frac.x, mp );
		}
	}
	else
	{
		if( pTL.y + 1 < src.height( ) )
		{
			// on the last column, but not the bottom-right corner pixel
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( *loc, ( 1.0 - frac.y ), mp );
			++loc.y( );
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( *loc, frac.y, mp );
		}
		else
		{
			// the bottom-right corner pixel
			ttl_detail::add_dst_mul_src<SrcP, F, PixelW > ( )( *loc, 1, mp );
		}
	}

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

/**
 * @brief Get weight for a specific distance, for all BC-cubic resampling (bicubic, catmul-rom, ...).
 * @param[in] B value of B in BC-cubic resampling function
 * @param[in] C value of C in BC-cubic resampling function
 * @param[in] distance between the pixels and the current pixel
 * @param[out] weight return value to weight the pixel in filtering
**/
template < typename F >
bool getBCWeight(const F B, const F C, const F distance, F &weight )
{
	if(distance<1)
	{
		F P = 2.0 - 1.5 * B - C;
		F Q = - 3.0 + 2.0 * B + C;
		F S = 1.0 - B/3.0;
		// note: R is null
		weight = ( P * distance + Q ) *  distance * distance + S;
		return true;
	}
	else
	{
		if(distance<2)
		{
		F T = - 1.0/6.0 * B - C;
		F U = B + 5.0 * C;
		F V = -2.0 * B - 8.0 * C;
		F W = 4.0 / 3.0 * B + 4 * C;
		weight = ( ( T * distance + U ) *  distance + V ) * distance + W;
		return true;
		}
		return false;
	}
}

template <typename SrcP, typename F, typename DstP>
struct bicubic1D
{

        void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, const F* weight, DstP & dst ) const
	{
		DstP mp( 0 );

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, weight[0] , mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, weight[1] , mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, weight[2] , mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, weight[3] , mp );
		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct catmul1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 );

		const float X1 = weight; // x
		const float X2 = weight * weight; // x^2
		const float X3 = weight * X2; // x^3


		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, 0.5*( -  X3 +2*X2 -X1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 0.5*( 3*X3 -5*X2 +2   ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, 0.5*(-3*X3 +4*X2 +X1  ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, 0.5*(   X3 -  X2      ), mp );

		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct keys1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 );

		// fisrt method, but with a lot of access to image memory
		/*
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	,-1, mp3 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, 3, mp3 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	,-3, mp3 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcD	, 1, mp3 );

		ttl_detail::add_dst_mul_src<DstP, float, DstP >()(mp3	, weight*0.5,	mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	, 1,		mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, -2.5,		mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	, 2,		mp2 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcD	, -0.5,		mp2 );

		ttl_detail::add_dst_mul_src<DstP, float, DstP >()(mp2	, weight,	mp1 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcA	, -0.5,		mp1 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcC	, 0.5,		mp1 );

		ttl_detail::add_dst_mul_src<DstP, float, DstP >()(mp1	, weight,	mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP >()(srcB	, 1,		mp );
		 */

		// second methos to minimize image merory access
		const float valueX = weight * 0.5; // x/2
		const float valueXX = weight * valueX; // (x^2)/2
		const float valueXXX = weight - 1; // x-1

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, -valueX * valueXXX*valueXXX, mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 1 + valueXX * ( 3 * weight - 5 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, valueX * ( ( -3 * weight + 4 ) * weight + 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, valueXX*valueXXX, mp );

		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct simon1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 );

		// second methos to minimize image merory access
		const float valueX = weight * 0.25; // x/4
		const float valueXX = weight * valueX; // (x^2)/4
		const float valueXXX = weight - 1; // x-1

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, -3 * valueX * valueXXX*valueXXX, mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 1 + valueXX * ( 5 * weight - 9 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, valueX * ( ( -5 * weight + 6 ) * weight + 3 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, 3 * valueXX*valueXXX, mp );

		dst = mp;
	}
};

template <typename SrcP, typename F, typename DstP>
struct rifman1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, F weight, DstP & dst ) const
	{
		DstP mp( 0 );

		// second method to minimize image memory access
		const float valueXX = weight * weight; // (x^2)
		const float valueXXX = weight - 1; // x-1

		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, -weight * valueXXX*valueXXX, mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, 1 + valueXX * ( weight - 2 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, weight * ( -valueXXX * weight + 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, valueXX*valueXXX, mp );

		dst = mp;
	}
};

/**
 * @brief Get pixels around a particular position.
 * @param[in] loc locator which points to a pixel
 * @param[in] pt0 x,y position of loc
 * @param[in] windowWidth the region inside which we search our pixels
 * @param[out] ptA pixel value to retrieve
 * @param[out] ptB pixel value to retrieve
 * @param[out] ptC pixel value to retrieve
 * @param[out] ptD pixel value to retrieve
 *
 * -----------------
 * | A |-B-| C | D |
 * -----------------
 *       ^..... loc is pointing to B point
 */
template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD )
{
	static const SrcP nullPt( 0 );

	if( p0.x < 0 )
	{
		ptA = loc.x( )[1];
		ptB = loc.x( )[1];
		ptC = loc.x( )[1];
		ptD = ( p0.x + 2 < windowWidth ) ? loc.x( )[2] : ptC;
		return;
	}

	ptB = *loc;
        ptA = ( p0.x - 1 >= 0 ) ? loc.x( )[-1] : ptB;
	ptC = ( p0.x + 1 < windowWidth ) ? loc.x( )[1] : *loc;
	ptD = ( p0.x + 2 < windowWidth ) ? loc.x( )[2] : ptC;

}

/**
 * @brief Get pixels around a particular position.
 * @param[in] loc locator which points to a pixel
 * @param[in] pt0 x,y position of loc
 * @param[in] windowWidth the region inside which we search our pixels
 * @param[out] ptA pixel value to retrieve
 * @param[out] ptB pixel value to retrieve
 * @param[out] ptC pixel value to retrieve
 * @param[out] ptD pixel value to retrieve
 * @param[out] ptE pixel value to retrieve
 * @param[out] ptF pixel value to retrieve
 * @param[out] ptG pixel value to retrieve
 *
 * -----------------------------
 * | A | B | C | D | E | F | G |
 * -----------------------------
 *               ^..... loc is pointing to D point
 */
template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const int windowWidth, SrcP& ptA, SrcP& ptB, SrcP& ptC, SrcP& ptD, SrcP& ptE, SrcP& ptF, SrcP& ptG )
{
	static const SrcP nullPt( 0 );

	if( p0.x < 0 )
	{
		ptA = loc.x( )[1];
		ptB = loc.x( )[1];
		ptC = loc.x( )[1];
		ptD = loc.x( )[1];
		ptE = loc.x( )[1];
		ptF = ( p0.x + 2 < windowWidth ) ? loc.x( )[2] : ptE;
		ptG = ( p0.x + 3 < windowWidth ) ? loc.x( )[3] : ptF;
		return;
	}

	ptD = *loc;
	ptC = ( p0.x - 1 < windowWidth ) ? loc.x( )[-1] : ptD;
	ptB = ( p0.x - 2 < windowWidth ) ? loc.x( )[-2] : ptC;
	ptA = ( p0.x - 3 < windowWidth ) ? loc.x( )[-3] : ptB;
	ptE = ( p0.x + 1 < windowWidth ) ? loc.x( )[1] : ptD;
	ptF = ( p0.x + 2 < windowWidth ) ? loc.x( )[2] : ptE;
	ptG = ( p0.x + 3 < windowWidth ) ? loc.x( )[3] : ptF;
}

template < typename xy_locator, typename SrcP >
void setXPixels( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const unsigned int windowWidth, std::vector< SrcP > src )
{
	static const SrcP nullPt( 0 );

	if( p0.x < 0 )
	{
		for( unsigned int i = 0; i < windowWidth+2; i++)
			src.at(i) = loc.x( )[1];

		unsigned int position = 2;
		for( unsigned int i = windowWidth+2; i < windowWidth*2+1; i++)
		{
			src.at(i) = ( p0.x + position < windowWidth ) ? loc.x( )[position] : src.at( i - 1 );
			position++;
		}
		return;
	}

	// center pixel is the current locator
	src.at(windowWidth) = *loc;
        unsigned int position = 1;
        for( unsigned int i = 0; i < windowWidth; i++)
        {
            // for negative index
            src.at( windowWidth - 1 - i ) = ( p0.x - position < windowWidth ) ? loc.x( )[ -position ] : src.at( windowWidth - i );

            // for positive index
            src.at( windowWidth + 1 + i ) = ( p0.x + position < windowWidth ) ? loc.x( )[  position ] : src.at( windowWidth + i );
            position++;
        }
}

template < typename F >
static F clean(F t)
{
	const F EPSILON = .0000125f;
	if (fabs(t) < EPSILON)
		return 0.0f;
	return t;
}


template< typename F >
F getNearestWeight( const F weight )
{
	if( weight <0.5 && weight >-0.5 )
		return 1;

	return 0;
}

template< typename F >
static F sinc(F x)
{
	x = (x * boost::math::constants::pi<F > ( ));

	if ((x < 0.01f) && (x > -0.01f))
		return 1.0f + x*x*(-1.0f/6.0f + x*x*1.0f/120.0f);

	return sin(x) / x;
}

template <typename F>
static F blackman_exact_window(F x)
{
	return 0.42659071f + 0.49656062f * cos(boost::math::constants::pi<F > ( )*x) + 0.07684867f * cos(2.0f*boost::math::constants::pi<F > ( )*x);
}

#define GAUSSIAN_SUPPORT (1.25f)
template< typename F >
F getGaussianWeight( const F weight )
{
	F w = abs( weight );

        if( w < GAUSSIAN_SUPPORT )
		return clean( exp(-2.0f * w * w) * sqrt(2.0f / boost::math::constants::pi<F > ( )) * blackman_exact_window(w / GAUSSIAN_SUPPORT) );
	else
		return 0;
}


template< typename F >
bool getSincWeight( const F position, F weight, F supportSize )
{
        F w = abs( position );

        if( w < supportSize )
        {
            weight = clean( sinc<F>(w)*sinc<F>(w/supportSize) );
            return true;
        }
	else
        {
            weight = 0;
            return false;
        }
}

template <typename SrcP, typename F, typename DstP>
struct lanczos1D
{
        void operator( )( const std::vector<SrcP> src, const std::vector<F> weight, DstP& dst ) const
        {
                DstP mp( 0 );
                for( unsigned int i = 0; i < src.size(); i++ )
                    ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( src[i], weight[i], mp );

                dst = mp;
        }
};

template <typename SrcP, typename F, typename DstP>
struct gaussian1D
{

	void operator( )( const SrcP& srcA, const SrcP& srcB, const SrcP& srcC, const SrcP& srcD, const SrcP& srcE, const SrcP& srcF, const SrcP& srcG, F weight, DstP & dst ) const
	{
		DstP mp( 0 );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcA, getGaussianWeight( weight + 3 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcB, getGaussianWeight( weight + 2 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcC, getGaussianWeight( weight + 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcD, getGaussianWeight( weight     ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcE, getGaussianWeight( weight - 1 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcF, getGaussianWeight( weight - 2 ), mp );
		ttl_detail::add_dst_mul_src<SrcP, float, DstP > ( )( srcG, getGaussianWeight( weight - 3 ), mp );

		dst = mp;
	}
};

struct ttl_bc_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_bc_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
        typedef typename SrcView::value_type SrcP;
        typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

        point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

        pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
        typedef typename SrcView::xy_locator xy_locator;
        xy_locator loc = src.xy_at( p0.x, p0.y );
        point2<F> frac( p.x - p0.x, p.y - p0.y );

        // values of horizontal filtering, used for vertical process
        SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

        // source pixels
        //
        SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

        // get weight for each pixels on 1D for horizontal filtering
        F weight[4];
        getBCWeight( valB, valC, frac.x+1, weight[0] );
        getBCWeight( valB, valC, frac.x  , weight[1] );
        getBCWeight( valB, valC, 1-frac.x, weight[2] );
        getBCWeight( valB, valC, 2-frac.x, weight[3] );

        if( p0.y < 0 )
        {
                ++loc.y( );
                setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
                bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a0 );

                a1 = a0;
                a2 = a0;

                ++loc.y( );
                if( p0.y + 2 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a3 );
                }
                else
                {
                        a3 = a2;
                }
        }
        else
        {
                if( p0.y < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a1 );
                }
                if( p0.y != 0 && p0.y - 1 < src.height( ) )
                {
                        --loc.y( );
                        setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a0 );
                        ++loc.y( );
                }
                else
                {
                        a0 = a1;
                }

                ++loc.y( );
                if( p0.y + 1 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a2 );
                }
                else
                {
                        a2 = a1;
                }
                ++loc.y( );
                if( p0.y + 2 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a3 );
                }
                else
                {
                        a3 = a2;
                }
        }

        // vertical process
        getBCWeight( valB, valC, frac.y+1, weight[0] );
        getBCWeight( valB, valC, frac.y  , weight[1] );
        getBCWeight( valB, valC, 1-frac.y, weight[2] );
        getBCWeight( valB, valC, 2-frac.y, weight[3] );
        bicubic1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, weight, mp );

        // Convert from floating point average value to the source type
        SrcP src_result;
        cast_pixel( mp, src_result );

        color_convert( src_result, result );
        return true;
}


struct ttl_bicubic_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_bicubic_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
    valB = 0.0;
    valC = 0.0;
    return sample< DstP, SrcView, F>( ttl_bc_sampler(), src, p, result );
}

struct ttl_catmul_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_catmul_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
    valB = 0.0;
    valC = 0.5;
    return sample< DstP, SrcView, F>( ttl_bc_sampler(), src, p, result );
}

struct ttl_mitchell_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_mitchell_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
    valB = 1.0/3.0;
    valC = 1.0/3.0;
    return sample< DstP, SrcView, F>( ttl_bc_sampler(), src, p, result );
}

struct ttl_parzen_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_parzen_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
    valB = 1.0;
    valC = 0.0;
    return sample< DstP, SrcView, F>( ttl_bc_sampler(), src, p, result );
}

struct ttl_keys_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_keys_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		++loc.y( );
		setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
		keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
		if( p0.y < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
		if( p0.y != 0 && p0.y - 1 < src.height( ) )
		{
			--loc.y( );
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );
			++loc.y( );
		}
		else
		{
			a0 = a1;
		}

		++loc.y( );
		if( p0.y + 1 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			keys1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	keys1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_simon_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_simon_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	//if (p0.x < 0 || p0.y < 0 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		++loc.y( );
		setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
		simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
		if( p0.y < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
		if( p0.y != 0 && p0.y - 1 < src.height( ) )
		{
			--loc.y( );
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );
			++loc.y( );
		}
		else
		{
			a0 = a1;
		}

		++loc.y( );
		if( p0.y + 1 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			simon1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	simon1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );


	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_rifman_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_rifman_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 );

	if( p0.y < 0 )
	{
		++loc.y( );
		setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
		rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );

		a1 = a0;
		a2 = a0;

		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}
	else
	{
		if( p0.y < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a1 );
		}
		if( p0.y != 0 && p0.y - 1 < src.height( ) )
		{
			--loc.y( );
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a0 );
			++loc.y( );
		}
		else
		{
			a0 = a1;
		}

		++loc.y( );
		if( p0.y + 1 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a2 );
		}
		else
		{
			a2 = a1;
		}
		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP >( loc, p0, src.width( ), ptA, ptB, ptC, ptD );
			rifman1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, frac.x, a3 );
		}
		else
		{
			a3 = a2;
		}
	}

	// vertical process
	rifman1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, frac.y, mp );

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

struct ttl_lanczos_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_lanczos_sampler, const SrcView& src, const point2<F>& p, const F supportWindow, DstP& result )
{
        typedef typename SrcView::value_type SrcP;
        typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;
        point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

        if (p0.x < -1 || p0.y < -1 || p0.x>=src.width() || p0.y>=src.height()) return false;

        pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
        typedef typename SrcView::xy_locator xy_locator;
        xy_locator loc = src.xy_at( p0.x, p0.y );
        point2<F> frac( p.x - p0.x, p.y - p0.y );

        // If we are on a border, we cheat with the position,
        // so we move to have the 4 corners inside the image
        // and adjust the "frac" coefficient to only use 1 or 2 pixels
        // to do the interpolation (other values are multiplied by 0)

        F windowSize = supportWindow*2+1;

        std::vector< SrcP > ptr;        
        std::vector< SrcC > xProcessed;
        std::vector < F > weight;

        xProcessed.reserve(windowSize);
        ptr.reserve(windowSize);
        weight.reserve(windowSize);

        // get horizontal weight for each pixels
        for( unsigned int i = 0; i < windowSize; i++ )
        {
            getSincWeight( frac.x+supportWindow-i, weight.at(i), supportWindow );
        }

        if( p0.y < 0 )
        {
                ++loc.y( );
                setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at( supportWindow + 2 ) );

                for( unsigned int i = 0; i < supportWindow+1; i++)
                    xProcessed.at(i) = xProcessed.at( supportWindow + 2 );

                unsigned int position = 2;
                for( unsigned int i = supportWindow+2; i < windowSize; i++)
                {
                    ++loc.y( );
                    if( p0.y + position < src.height( ) )
                    {
                            setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                            lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(i) );
                    }
                    else
                    {
                            xProcessed.at(i) = xProcessed.at( i-1 );
                    }
                    position++;
                }

        }
        else
        {
                setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(supportWindow) );

                --loc.y( );
                if( p0.y != 0 && p0.y - 1 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                        lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(2) );
                }
                else
                {
                        xProcessed.at(2) = xProcessed.at(3);
                }

                --loc.y( );
                if( p0.y + 2 != 0 && p0.y - 2 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                        lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(1) );
                }
                else
                {
                        xProcessed.at(1) = xProcessed.at(2);
                }

                --loc.y( );
                if( p0.y + 3 != 0 && p0.y - 3 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                        lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(0) );
                }
                else
                {
                        xProcessed.at(0) = xProcessed.at(1);
                }

                loc.y( ) += 4;

                if( p0.y + 1 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                        lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(4) );
                }
                else
                {
                        xProcessed.at(4) = xProcessed.at(3);
                }
                ++loc.y( );
                if( p0.y + 2 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                        lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(5) );
                }
                else
                {
                        xProcessed.at(5) = xProcessed.at(4);
                }
                ++loc.y( );
                if( p0.y + 3 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptr );
                        lanczos1D< SrcP, F, SrcC > ( )( ptr, weight, xProcessed.at(6) );
                }
                else
                {
                        xProcessed.at(6) = xProcessed.at(5);
                }
        }
        // get vertical weight for each pixels
        for( unsigned int i = 0; i < windowSize; i++)
        {
            getSincWeight( frac.y+supportWindow-i, weight.at(i), supportWindow );
        }
        // vertical process
        lanczos1D< SrcC, F, SrcC > ( )( xProcessed, weight, mp );

        // Convert from floating point average value to the source type
        SrcP src_result;
        cast_pixel( mp, src_result );

        color_convert( src_result, result );
        return true;
}

struct ttl_lanczos3_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_lanczos3_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
        return sample< DstP, SrcView, F>( ttl_lanczos_sampler(), src, p, 3.0, result );
}

struct ttl_lanczos4_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_lanczos4_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
        return sample< DstP, SrcView, F>( ttl_lanczos_sampler(), src, p, 4.0, result );
}

struct ttl_lanczos6_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_lanczos6_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
        return sample< DstP, SrcView, F>( ttl_lanczos_sampler(), src, p, 6.0, result );
}

struct ttl_lanczos12_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_lanczos12_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
        return sample< DstP, SrcView, F>( ttl_lanczos_sampler(), src, p, 12.0, result );
}

struct ttl_gaussian_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( ttl_gaussian_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;
	point2<std::ptrdiff_t> p0( ifloor( p ) ); // the closest integer coordinate top left from p

	if (p0.x < -1 || p0.y < -1 || p0.x>=src.width() || p0.y>=src.height()) return false;

	pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( p0.x, p0.y );
	point2<F> frac( p.x - p0.x, p.y - p0.y );

	SrcC a0( 0 ), a1( 0 ), a2( 0 ), a3( 0 ), a4( 0 ), a5( 0 ), a6( 0 );

	SrcP ptA( 0 ), ptB( 0 ), ptC( 0 ), ptD( 0 ), ptE( 0 ), ptF( 0 ), ptG( 0 );

	if( p0.y < 0 )
	{
		++loc.y( );
		setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
		gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a4 );

		a0 = a2 = a3 = a4;

		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a5 );
		}
		else
		{
			a5 = a4;
		}
		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a6 );
		}
		else
		{
			a6 = a5;
		}
	}
	else
	{
		if( p0.y < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a3 );
		}
		--loc.y( );
		if( p0.y != 0 && p0.y - 1 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a2 );
		}
		else
		{
			a2 = a3;
		}

		--loc.y( );
		if( p0.y + 2 != 0 && p0.y - 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a1 );
		}
		else
		{
			a1 = a2;
		}

		--loc.y( );
		if( p0.y + 3 != 0 && p0.y - 3 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a0 );
		}
		else
		{
			a0 = a1;
		}

		loc.y( ) += 4;

		if( p0.y + 1 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a4 );
		}
		else
		{
			a4 = a3;
		}
		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a5 );
		}
		else
		{
			a5 = a4;
		}
		++loc.y( );
		if( p0.y + 2 < src.height( ) )
		{
			setXPixels<xy_locator, SrcP > ( loc, p0, src.width( ), ptA, ptB, ptC, ptD, ptE, ptF, ptG );
			gaussian1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, ptE, ptF, ptG, frac.x, a6 );
		}
		else
		{
			a6 = a5;
		}
	}

	// vertical process
	gaussian1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, a4, a5, a6, frac.y, mp );

	// Convert from floating point average value to the source type
	SrcP src_result;
	cast_pixel( mp, src_result );

	color_convert( src_result, result );
	return true;
}

}
}

#endif
