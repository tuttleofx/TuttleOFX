#ifndef _TERRY_SAMPLER_DETAILS_HPP_
#define _TERRY_SAMPLER_DETAILS_HPP_

#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

#include <terry/clamp.hpp>

#include <cmath>
#include <vector>


namespace terry {
using namespace boost::gil;
namespace sampler {
namespace details {

template< typename F>
void proc( F& pixel)
{
}

template< >
void proc( rgb8_pixel_t& pixel)
{
	TUTTLE_COUT("rgb8");
}

template< >
void proc( rgb16_pixel_t& pixel)
{
	TUTTLE_COUT("rgb16");
}

template< >
void proc( rgb32_pixel_t& pixel)
{
	TUTTLE_COUT("rgb32");
}

template< >
void proc( rgb8s_pixel_t& pixel)
{
	TUTTLE_COUT("rgb8s");
}

template< >
void proc( rgb16s_pixel_t& pixel)
{
	TUTTLE_COUT("rgb16s");
}

template< >
void proc( rgb32s_pixel_t& pixel)
{
	TUTTLE_COUT("rgb32s");
}

// alpha

template< >
void proc( rgba8_pixel_t& pixel)
{
	TUTTLE_COUT("rgba8");
}

template< >
void proc( rgba16_pixel_t& pixel)
{
	TUTTLE_COUT("rgba16");
}

template< >
void proc( rgba32_pixel_t& pixel)
{
	TUTTLE_COUT("rgba32");
}

template< >
void proc( rgba8s_pixel_t& pixel)
{
	TUTTLE_COUT("rgba8s");
}

template< >
void proc( rgba16s_pixel_t& pixel)
{
	TUTTLE_COUT("rgba16s");
}

template< >
void proc( rgba32s_pixel_t& pixel)
{
	TUTTLE_COUT("rgba32s");
}

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
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const ssize_t& windowWidth, const ssize_t& imageWidth, std::vector< SrcP >& src )
{
	ssize_t middlePosition = floor( (src.size() - 1) * 0.5 );

	if( p0.x < 0 )
	{
		src.at( middlePosition ) = SrcP(0);
	}
	else
	{
		src.at( middlePosition ) = *loc;
	}

	// from center to left
	for( ssize_t i = middlePosition - 1; i > -1; i-- )
	{
		src.at( i ) = ( p0.x - (middlePosition - i) < 0 ) ?  SrcP(0) : loc.x( )[ - (middlePosition - i) ];
	}

	// from center to right
	for( ssize_t i = middlePosition + 1; i < (ssize_t) src.size(); i++ )
	{
		src.at( i ) = ( p0.x - (middlePosition - i) < imageWidth ) ? loc.x( )[  - (middlePosition - i) ] : SrcP(0);
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
	typedef pixel<float, devicen_layout_t<num_channels<SrcView>::value> > SrcC;

	point2<std::ptrdiff_t> pTL( ifloor( p ) ); // the closest integer coordinate top left from p


	SrcC mp( 0 );


	std::vector < SrcP > ptr;
	std::vector < SrcC > xProcessed;

	ptr.assign       ( windowSize, SrcP(0) );
	xProcessed.assign( windowSize, SrcC(0) );

	size_t middlePosition = floor((windowSize - 1) * 0.5);

	//TUTTLE_COUT( pTL.y << " // " << src.height() );


	// first process the middle point
	// if it's mirrored, we need to copy the center point
	if( (pTL.y < 0) || (pTL.y > (ssize_t) src.height( ) ) )
	{
		xProcessed.at( middlePosition ) = SrcP(0);
	}
	else
	{
		getPixelsPointers( loc, pTL, windowSize, src.width() , ptr );
		process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
	}

	// from center to bottom
	for( ssize_t i = middlePosition - 1; i > -1; i-- )
	{
		if( (ssize_t) ( pTL.y - (middlePosition - i) ) < (ssize_t) src.height( ) )
		{
			loc.y( ) += (middlePosition - i);
			getPixelsPointers( loc, pTL, windowSize, src.width(), ptr );
			process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( i ) );
			loc.y( ) -= (middlePosition - i);
		}
		else
		{
			xProcessed.at(i) = SrcP(0);
		}
	}

	// from center to top
	for( ssize_t i = middlePosition + 1; i < (ssize_t)windowSize; i++ )
	{
		if( (ssize_t) ( pTL.y + (i - middlePosition) ) < (ssize_t) src.height( ) )
		{
			loc.y( ) -= (middlePosition - i);
			getPixelsPointers( loc, pTL, windowSize, src.width(), ptr );
			process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( i ) );
			loc.y( ) += (middlePosition - i);
		}
		else
		{
			xProcessed.at(i) = SrcP(0);
		}
	}


	// vertical process
	process1Dresampling<SrcC, F, SrcC> () ( xProcessed, yWeights, mp );

	// result is rgba8
	proc( mp );
	// Convert from floating point average value to the source type
	DstP src_result;
	cast_pixel  ( mp, src_result );
	color_convert( src_result, result );

	return true;
}


}
}
}

#endif
