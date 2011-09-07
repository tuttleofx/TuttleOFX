#ifndef _TERRY_SAMPLER_DETAILS_HPP_
#define _TERRY_SAMPLER_DETAILS_HPP_

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
//#include <boost/math/constants/constants.hpp>

#include <cmath>
#include <vector>


namespace terry {
using namespace boost::gil;
namespace sampler {
namespace details {

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
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const ssize_t& windowWidth, std::vector< SrcP >& src )
{
	int maxPosition = floor( src.size() * 0.5 );
	int minPosition = maxPosition - src.size() + 1;
	int position    = minPosition;

	if( p0.x < 0 )
	{
		for( ssize_t i = 0; i < -minPosition + 1; i++)
			src.at(i) = SrcP(0);//loc.x( )[1];

		src.at( - minPosition + 1) = loc.x( )[1];

		unsigned int pos = 2;

		for( ssize_t i = 1; i < maxPosition + 1; i++)
		{
			src.at( i ) = ( p0.x + pos < windowWidth ) ? loc.x( )[ pos ] : src.at( i - 1 );
			pos++;
		}

		return;
	}

	src.at( -minPosition ) = *loc;

	for( ssize_t i = 0; i > minPosition; i--)
	{
		src.at( i ) = ( p0.x + position < 0 ) ?  src.at( i + 1 ) : loc.x( )[ position ];
		position++;
	}

	for( ssize_t i = 1; i < maxPosition + 1; i++)
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
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const ssize_t& windowWidth, const size_t& supportWidth, std::vector< SrcP >& src )
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


template <typename SrcP, typename F, typename DstP>
struct process1Dresampling
{
	void operator( )( const std::vector<SrcP>& src, const std::vector<F>& weight, DstP& dst ) const
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
bool process2Dresampling( Sampler& sampler, const SrcView& src, const point2<F>& p, const std::vector<double>& xWeights, const std::vector<double>& yWeights, const size_t& windowSize,typename SrcView::xy_locator& loc, DstP& result )
{
	typedef typename SrcView::value_type SrcP;
	typedef pixel<F, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> pTL( ifloor( p ) ); // the closest integer coordinate top left from p


	SrcC mp( 0 );


	std::vector < SrcP > ptr;
	std::vector < SrcC > xProcessed;

	ptr.assign       ( windowSize, SrcP(0) );
	xProcessed.assign( windowSize, SrcC(0) );

	if( pTL.y < 0 )
	{

		++loc.y( );
		getPixelsPointers( loc, pTL, windowSize, ptr );
		process1Dresampling<SrcP, F, SrcC> () (ptr, xWeights, xProcessed.at(1));

		xProcessed.at(0) = xProcessed.at(1);
		xProcessed.at(2) = xProcessed.at(1);

		++loc.y( );
		if( pTL.y + 2 < src.height( ) )
		{
			getPixelsPointers( loc, pTL, windowSize, ptr );
			process1Dresampling<SrcP, F, SrcC> () (ptr, xWeights, xProcessed.at(3));
		}
		else
		{
			xProcessed.at(3) = xProcessed.at(2);
		}
	}
	else
	{

		if( pTL.y < src.height( ) )
		{
			getPixelsPointers( loc, pTL, windowSize, ptr );
			process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at(1) );
		}

		if( pTL.y != 0 && pTL.y - 1 < src.height( ) )
		{
			--loc.y( );
			getPixelsPointers( loc, pTL, windowSize, ptr );
			process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at(0) );
			++loc.y( );
		}
		else
		{
			xProcessed.at(0) = xProcessed.at(1);
		}

		++loc.y( );
		if( pTL.y + 1 < src.height( ) )
		{
			getPixelsPointers( loc, pTL, windowSize, ptr );
			process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at(2) );
		}
		else
		{
			xProcessed.at(2) = xProcessed.at(1);
		}
		++loc.y( );
		if( pTL.y + 2 < src.height( ) )
		{
			getPixelsPointers( loc, pTL, windowSize, ptr );
			process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at(3) );
		}
		else
		{
			xProcessed.at(3) = xProcessed.at(2);
		}
	}

	// vertical process
	process1Dresampling<SrcC, F, SrcC> () ( xProcessed, yWeights, mp );

	// Convert from floating point average value to the source type
	DstP src_result;

	cast_pixel( mp, src_result );
	//cast_pixel( xProcessed.at(1), src_result );

	color_convert( src_result, result );
	return true;
}


}
}
}

#endif
