#ifndef _TERRY_SAMPLER_HPP_
#define _TERRY_SAMPLER_HPP_

#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/math/constants/constants.hpp>

#include <boost/math/constants/constants.hpp>

#include <cmath>
#include <vector>


namespace terry {
using namespace boost::gil;

namespace sampler {


/// \brief A sampler that sets the destination pixel without interpolation from the closest pixels from the source.
// http://www.paulinternet.nl/?page=bicubic

struct nearest_neighbor_sampler {};
struct bc_sampler
{
    double valB;
    double valC;
};

template <typename DstP, typename SrcView, typename F>
bool sample( nearest_neighbor_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	point2<std::ptrdiff_t> center( iround( p ) );

	// if we are outside the image
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




namespace detail {

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

/**
 * @brief Get pixels around a particular position.
 * @param[in] loc locator which points to a pixel
 * @param[in] pt0 x,y position of loc
 * @param[in] windowWidth the region inside which we search our pixels
 * @param[out] ptN pixel value to retrieve
 *
 * it's to use with (B,C) filter
 * number of points need to be even
 *
 * -----------------
 * | A | B | C | D |
 * -----------------
 *       ^..... loc is pointing to D point
 */
template < typename xy_locator, typename SrcP >
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const int windowWidth, std::vector< SrcP > src )
{
        int maxPosition = floor(src.size()*0.5);
        int minPosition = maxPosition - src.size() + 1;
        int position    = minPosition;

        if( p0.x < 0 )
        {
                for( int i = 0; i < -minPosition + 1; i++)
                        src.at(i) = loc.x( )[1];

                unsigned int position = 2;

                for( int i = 1; i < maxPosition + 1; i++)
                {
                        src.at( i ) = ( p0.x + position < windowWidth ) ? loc.x( )[ position ] : src.at( i - 1 );
                        position++;
                }
                return;
        }

        src.at( -minPosition ) = *loc;

        for( int i = -minPosition; i > 0; i++)
        {
                src.at( i ) = ( p0.x + position < 0 ) ?  src.at( i + 1 ) : loc.x( )[ position ];
                position++;
        }

        for( int i = 1; i < maxPosition + 1; i++)
        {
                src.at( i ) = ( p0.x + position < windowWidth ) ? loc.x( )[ position ] : src.at( i - 1 );
                position++;
        }
}

/**
 * @brief Get pixels around a particular position.
 * @param[in] loc locator which points to a pixel
 * @param[in] pt0 x,y position of loc
 * @param[in] windowWidth the region inside which we search our pixels
 * @param[out] ptN pixel value to retrieve
 *
 * it's to use with symetric windows like Lanczos, Gaussian filter
 * number of points need to be odd
 *
 * -----------------------------
 * | A | B | C | D | E | F | G |
 * -----------------------------
 *               ^..... loc is pointing to D point
 */
template < typename xy_locator, typename SrcP >
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const int windowWidth, const unsigned  int supportWidth, std::vector< SrcP > src )
{
        if( p0.x < 0 )
        {
                for( unsigned int i = 0; i < supportWidth+2; i++)
                        src.at(i) = loc.x( )[1];

                unsigned int position = 2;
                for( unsigned int i = supportWidth+2; i < supportWidth*2+1; i++)
                {
                        src.at(i) = ( p0.x + position < windowWidth ) ? loc.x( )[position] : src.at( i - 1 );
                        position++;
                }
                return;
        }

        // center pixel is the current locator
        src.at(supportWidth) = *loc;
        int position = 1;
        for( unsigned int i = 0; i < supportWidth; i++)
        {
                // for negative index
                src.at( supportWidth - 1 - i ) = ( p0.x - position < windowWidth ) ? loc.x( )[ -position ] : src.at( supportWidth - i );

                // for positive index
                src.at( supportWidth + 1 + i ) = ( p0.x + position < windowWidth ) ? loc.x( )[  position ] : src.at( supportWidth + i );
                position++;
        }
}

/*
template< typename SamplerMethod, typename F >
bool getWeight ( const F position, F& weight, SamplerMethod sampler )
{
        return false;
}*/

/**
 * @brief Get weight for a specific distance, for nearest neightbor resampling ).
 *
 *  >--<  distance between current point and the nearest point
 *
 *  x  o
 *  ^  ^-- the current point which be resampling
 *  |
 *  | the nearest point on top left
 *
 * more explaination can be found here [http://avisynth.org/mediawiki/Resampling]
 *
 * @param[position] distance between the pixels and the current pixel
 * @param[weight] weight return value to weight the pixel in filtering
**/
template< typename F >
bool getWeight ( const F& position, std::vector<F> weight, nearest_neighbor_sampler sampler )
{
        if( position <0.5 && position >-0.5 )
        {
                weight = 1;
                return true;
        }
        weight = 0;
        return false;
}

/**
 * @brief Get weight for a specific distance, for all BC-cubic resampling (bicubic, catmul-rom, ...).
 *
 * For compute cubic BC resampler weights, we use these functions
 * [ Reconstruction Filters in Computer Graphics,
 *   Don P. Mitchell, Arun N. Netravali,
 *   Computer Graphics - volume 22 number 4 - August 1988
 *   <a href="http://www.cs.utexas.edu/users/fussell/courses/cs384g/lectures/mitchell/Mitchell.pdf">online paper</a>
 * ]:
 *
 * \f[ W(x) =
 * \begin{cases}
 * (a+2)|x|^3-(a+3)|x|^2+1 & \text{for } |x| \leq 1 \\
 * a|x|^3-5a|x|^2+8a|x|-4a & \text{for } 1 < |x| < 2 \\
 * 0                       & \text{otherwise}
 * \end{cases}
 * \f]
 * @param[in] B value of B in BC-cubic resampling function
 * @param[in] C value of C in BC-cubic resampling function
 * @param[in] distance between the pixels and the current pixel
 * @param[out] weight return value to weight the pixel in filtering
**/
template < typename F >
bool getWeight ( const F distance, F& weight, bc_sampler sampler )
{
        if(distance<1)
        {
                double P =   2.0 - 1.5 * sampler.valB - sampler.valC;
                double Q = - 3.0 + 2.0 * sampler.valB + sampler.valC;
                double S =   1.0 -       sampler.valB / 3.0;
                // note: R is null
                weight = ( P * distance + Q ) *  distance * distance + S;
                return true;
        }
        else
        {
                if( distance < 2 )
                {
                        double T = - 1.0 / 6.0 * sampler.valB -       sampler.valC;
                        double U =               sampler.valB + 5.0 * sampler.valC;
                        double V = - 2.0       * sampler.valB - 8.0 * sampler.valC;
                        double W =   4.0 / 3.0 * sampler.valB + 4.0 * sampler.valC;
                        weight = ( ( T * distance + U ) *  distance + V ) * distance + W;
                        return true;
                }
                return false;
        }
}






template <typename SrcP, typename F, typename DstP>
struct process1Dresampling
{
	void operator( )( const std::vector<SrcP> src, const std::vector<F> weight, DstP & dst ) const
	{
		DstP mp( 0 );
		for( size_t i = 0; i < src.size(); i++ )
			detail::add_dst_mul_src<SrcP, float, DstP > ( )( src.at(i), weight.at(i) , mp );
		dst = mp;
	}
};

/// @todo specialization for SIMD
//template <typename F>
//struct process1Dresampling
//{
//	void operator()( const std::vector<rgba32f_t> src, const std::vector<F> weight, rgba32f_t & dst ) const
//	{
//		//...
//	}
//};

template <typename DstP, typename SrcView, typename Sampler, typename F>
bool process2Dresampling( Sampler& sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
        typedef typename SrcView::value_type SrcP;
        typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

        point2<std::ptrdiff_t> pTL( ifloor( p ) ); // the closest integer coordinate top left from p

        // if we are outside the image
        if( pTL.x < -1 ||
            pTL.y < -1 ||
            pTL.x > src.width() - 1 ||
            pTL.y > src.height() - 1 )
        {
                return false;
        }

        pixel<F, devicen_layout_t<num_channels<SrcView>::value> > mp( 0 );
        typedef typename SrcView::xy_locator xy_locator;
        xy_locator loc = src.xy_at( pTL.x, pTL.y );
        point2<F> frac( p.x - pTL.x, p.y - pTL.y );



        std::vector < SrcP > ptr;
        std::vector < SrcC > xProcessed;
        std::vector < F >    weights;
        unsigned int windowSize = 4;             // 4 pixels:    A B C D

        xProcessed.assign( windowSize, SrcC(0) );
        ptr       .assign( windowSize, SrcP(0) );
        weights   .assign( windowSize, 0.0 );

        // get horizontal weight for each pixels
        for( unsigned int i = 0; i < windowSize; i++ )
        {
                getWeight < Sampler > ( sampler.valB, sampler.valC, frac.x+2-i, weights.at(i) );
        }
/*
        if( pTL.y < 0 )
        {
                ++loc.y( );
                setXPixels( loc, pTL, src.width(), ptr );
                setXPixels<xy_locator, SrcP >( loc, pTL, src.width( ), ptA, ptB, ptC, ptD );
                bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a0 );

                a1 = a0;
                a2 = a0;

                ++loc.y( );
                if( pTL.y + 2 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, pTL, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a3 );
                }
                else
                {
                        a3 = a2;
                }
        }
        else
        {
                if( pTL.y < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, pTL, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a1 );
                }
                if( pTL.y != 0 && pTL.y - 1 < src.height( ) )
                {
                        --loc.y( );
                        setXPixels<xy_locator, SrcP >( loc, pTL, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a0 );
                        ++loc.y( );
                }
                else
                {
                        a0 = a1;
                }

                ++loc.y( );
                if( pTL.y + 1 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, pTL, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a2 );
                }
                else
                {
                        a2 = a1;
                }
                ++loc.y( );
                if( pTL.y + 2 < src.height( ) )
                {
                        setXPixels<xy_locator, SrcP >( loc, pTL, src.width( ), ptA, ptB, ptC, ptD );
                        bicubic1D< SrcP, F, SrcC > ( )( ptA, ptB, ptC, ptD, weight, a3 );
                }
                else
                {
                        a3 = a2;
                }
        }

        // vertical process
        getBCWeight( sampler.valB, sampler.valC, frac.y+1, weight[0] );
        getBCWeight( sampler.valB, sampler.valC, frac.y  , weight[1] );
        getBCWeight( sampler.valB, sampler.valC, 1-frac.y, weight[2] );
        getBCWeight( sampler.valB, sampler.valC, 2-frac.y, weight[3] );
        bicubic1D< SrcC, F, SrcC > ( )( a0, a1, a2, a3, weight, mp );
*/
        // Convert from floating point average value to the source type
        SrcP src_result;
        cast_pixel( mp, src_result );

        color_convert( src_result, result );
        return true;
}


}


template <typename DstP, typename SrcView, typename F>
bool sample( bc_sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
    return false;
}

}

}
#endif
