#ifndef _TERRY_SAMPLER_DETAILS_HPP_
#define _TERRY_SAMPLER_DETAILS_HPP_

#include <terry/numeric/operations.hpp>

#include "sampler.hpp"

#include <terry/clamp.hpp>
#include <terry/globals.hpp>
#include <terry/basic_colors.hpp>

#include <terry/typedefs.hpp>

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
	void operator( )( const SrcChannel& src, DstChannel& dst ) const
	{
		dst += DstChannel( channel_convert<DstChannel>(src) * _w );
	}
};

template <typename SrcP, typename Weight, typename DstP>
struct add_dst_mul_src
{
	void operator( )( const SrcP& src, const Weight weight, DstP & dst ) const
	{
		static_for_each( src, dst, add_dst_mul_src_channel<Weight>( weight ) );
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
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const int& windowWidth, const int& imageWidth, const EParamFilterOutOfImage& outOfImageProcess, std::vector< SrcP >& src )
{
	int middlePosition = floor( (src.size() - 1) * 0.5 );

	if( ( p0.x < 0 )  || ( p0.x > imageWidth - 1 ) )
	{
		switch( outOfImageProcess )
		{
			case eParamFilterOutBlack :
			{
				src.at( middlePosition ) = get_black<SrcP>();
				break;
			}
			case eParamFilterOutTransparency :
			{
				src.at( middlePosition ) = SrcP(0);
				break;
			}
			case eParamFilterOutCopy :
			{
				src.at( middlePosition ) = loc.x()[ ( p0.x < 0 ) ? (- p0.x) : - 1 - p0.x + imageWidth  ];
				break;
			}
			case eParamFilterOutMirror :
			{
				src.at( middlePosition ) = SrcP(0);
				break;
			}
		}
	}
	else
	{
		src.at( middlePosition ) = *loc;
	}

	// from center to left
	for( int i = middlePosition - 1; i > -1; i-- )
	{
		if( ( p0.x - (middlePosition - i) > -1 ) )
		{
			if( ( p0.x - (middlePosition - i) < imageWidth ) )
			{
				src.at( i ) = loc.x( )[ - (middlePosition - i) ];
			}
			else
			{
				switch( outOfImageProcess )
				{
					case eParamFilterOutBlack :
					{
						src.at( i ) = get_black<SrcP>();
						break;
					}
					case eParamFilterOutTransparency :
					{
						src.at( i ) = SrcP(0);
						break;
					}
					case eParamFilterOutCopy :
					{
						src.at( i ) = src.at( i + 1 );
						break;
					}
					case eParamFilterOutMirror :
					{
						src.at( i ) = SrcP(0);
						break;
					}
				};
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					src.at( i ) = get_black<SrcP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					src.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					src.at( i ) = src.at( i + 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					src.at( i ) = SrcP(0);
					break;
				}
			};
		}
	}

	// from center to right
	for( int i = middlePosition + 1; i < (int) src.size(); i++ )
	{
		if( ( p0.x - (middlePosition - i) < imageWidth ) )
		{
			if( p0.x - (middlePosition - i) < 0 )
			{
				switch( outOfImageProcess )
				{
					case eParamFilterOutBlack :
					{
						src.at( i ) = get_black<SrcP>();
						break;
					}
					case eParamFilterOutTransparency :
					{
						src.at( i ) = SrcP(0);
						break;
					}
					case eParamFilterOutCopy :
					{
						src.at( i ) = src.at( i - 1 );
						break;
					}
					case eParamFilterOutMirror :
					{
						src.at( i ) = SrcP(0);
						break;
					}
				};
			}
			else
			{
				src.at( i ) = loc.x( )[  - (middlePosition - i) ];
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					src.at( i ) = get_black<SrcP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					src.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					src.at( i ) = src.at( i - 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					src.at( i ) = SrcP(0);
					break;
				}
			};
		}
	}

}

template <typename SrcP, typename Weight, typename DstP>
struct process1Dresampling
{
	void operator( )( const std::vector<SrcP>& src, const std::vector<Weight>& weight, DstP& dst ) const
	{
		DstP mp( 0 );
		for( std::size_t i = 0; i < src.size(); i++ )
			details::add_dst_mul_src< SrcP, Weight, DstP > ( )( src.at(i), weight.at(i) , mp );
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

}

template <typename Sampler, typename DstP, typename SrcView, typename F>
bool sample( Sampler& sampler, const SrcView& src, const point2<F>& p, DstP& result, const EParamFilterOutOfImage outOfImageProcess )
{
	typedef typename SrcView::value_type                     SrcP;
	typedef typename floating_pixel_from_view<SrcView>::type SrcC; //PixelFloat;
	typedef typename boost::gil::bits64f                     Weight;
	typedef typename SrcView::xy_locator                     xy_locator;

	// xWeights and yWeights are weights for in relation of the distance to each point
	std::vector<Weight> xWeights, yWeights;

	SrcC                mp( 0 );
	std::vector<SrcP>   ptr;
	std::vector<SrcC>   xProcessed;

	/*
	 * pTL is the closest integer coordinate top left from p
	 *
	 *   pTL ---> x      x
	 *              o <------ p
	 *
	 *            x      x
	 */
	point2<std::ptrdiff_t> pTL( ifloor( p ) );

	// loc is the point in the source view
	xy_locator loc = src.xy_at( pTL.x, pTL.y );
	// frac is the distance between the point pTL and the current point
	point2<RESAMPLING_CORE_TYPE> frac( p.x - pTL.x, p.y - pTL.y );

	// assign Weights vector to the window size of the sampler
	xWeights.assign   ( sampler._windowSize , 0 );
	yWeights.assign   ( sampler._windowSize , 0 );

	ptr.assign        ( sampler._windowSize, SrcP(0) );
	xProcessed.assign ( sampler._windowSize, SrcC(0) );

	// compute the middle position on the filter
	std::size_t middlePosition = floor( ( sampler._windowSize - 1.0 ) * 0.5 );

	// get weights for each pixels
	for( size_t i = 0; i < sampler._windowSize; i++ )
	{
		RESAMPLING_CORE_TYPE distancex = - frac.x - middlePosition + i ;
		sampler( distancex, xWeights.at(i) );
		RESAMPLING_CORE_TYPE distancey = - frac.y - middlePosition + i ;
		sampler( distancey, yWeights.at(i) );
	}
/*
	for( int i=0; i<xWeights.size(); i++ )
	{
		std::cout << xWeights.at(i) << "\t" ;
	}
	std::cout << std::endl;
	*/
	// first process the middle point
	// if it's mirrored, we need to copy the center point
	if( (pTL.y < 0.0) || (pTL.y > (int) ( src.height( ) - 1.0 ) ) )
	{
		if( pTL.y < 0.0 ) // under the image
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					xProcessed.at( middlePosition ) = get_black<DstP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					xProcessed.at( middlePosition ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					loc.y( ) -= pTL.y;
					details::getPixelsPointers( loc, pTL, sampler._windowSize, src.width(), outOfImageProcess, ptr );
					details::process1Dresampling<SrcP, Weight, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
					loc.y( ) += pTL.y;
					break;
				}
				case eParamFilterOutMirror :
				{
					xProcessed.at( middlePosition ) = SrcP(1);
					break;
				}
			}
		}
		else // upper the image
		{
			//TUTTLE_LOG_INFO( src.height() << " @@ " << (pTL.y - src.height() ) );
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					xProcessed.at( middlePosition ) = get_black<DstP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					xProcessed.at( middlePosition ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					loc.y( ) -= pTL.y - src.height() + 1.0 ;
					details::getPixelsPointers( loc, pTL, sampler._windowSize, src.width(), outOfImageProcess, ptr );
					details::process1Dresampling<SrcP, Weight, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
					loc.y( ) += pTL.y - src.height() + 1.0;
					break;
				}
				case eParamFilterOutMirror :
				{
					xProcessed.at( middlePosition ) = SrcP(1);
					break;
				}
			}
		}
	}
	else
	{
		details::getPixelsPointers( loc, pTL, sampler._windowSize, src.width() , outOfImageProcess, ptr );
		details::process1Dresampling<SrcP, Weight, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
	}

	// from center to bottom
	for( int i = middlePosition - 1; i > -1; i-- )
	{
		if( (int) ( pTL.y - (middlePosition - i) ) < (int) src.height( ) )
		{
			if( (int) ( pTL.y - (middlePosition - i) ) < 0 )
			{
				switch( outOfImageProcess )
				{
					case eParamFilterOutBlack :
					{
						xProcessed.at( i ) = get_black<DstP>();
						break;
					}
					case eParamFilterOutTransparency :
					{
						xProcessed.at( i ) = SrcP(0);
						break;
					}
					case eParamFilterOutCopy :
					{
						xProcessed.at( i ) = xProcessed.at( i + 1 );
						break;
					}
					case eParamFilterOutMirror :
					{
						xProcessed.at( i ) = xProcessed.at( i + 1 );
						break;
					}
				}
			}
			else
			{
				loc.y( ) -= (middlePosition - i);
				details::getPixelsPointers( loc, pTL, sampler._windowSize, src.width(), outOfImageProcess, ptr );
				details::process1Dresampling<SrcP, Weight, SrcC> () ( ptr, xWeights, xProcessed.at( i ) );
				loc.y( ) += (middlePosition - i);
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					xProcessed.at( i ) = get_black<DstP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					xProcessed.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					xProcessed.at( i ) = xProcessed.at( i + 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					xProcessed.at( i ) = xProcessed.at( i + 1 );
					break;
				}
			}
		}
	}

	// from center to top
	for( int i = middlePosition + 1; i < (int)sampler._windowSize; i++ )
	{
		if( (int) ( pTL.y + (i - middlePosition) ) < (int) src.height( ) )
		{
			if( (int) ( pTL.y + (i - middlePosition) )  < 0 )
			{
				xProcessed.at( i ) = xProcessed.at( i - 1 );
			}
			else
			{
				loc.y( ) -= ( middlePosition - i );
				details::getPixelsPointers( loc, pTL, sampler._windowSize, src.width(), outOfImageProcess, ptr );
				details::process1Dresampling<SrcP, Weight, SrcC> () ( ptr, xWeights, xProcessed.at( i ) );
				loc.y( ) += ( middlePosition - i );
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					xProcessed.at( i ) = get_black<DstP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					xProcessed.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					xProcessed.at( i ) = xProcessed.at( i - 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					xProcessed.at( i ) = xProcessed.at( i - 1 );
					break;
				}
			}
		}
	}

	// vertical process
	details::process1Dresampling<SrcC, Weight, SrcC> () ( xProcessed, yWeights, mp );

	// Convert from floating point average value to the destination type
	color_convert( mp, result );

	return true;
}

}
}

#endif
