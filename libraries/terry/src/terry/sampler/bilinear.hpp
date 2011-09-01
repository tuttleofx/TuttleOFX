#ifndef _TERRY_SAMPLER_BILINEAR_HPP_
#define _TERRY_SAMPLER_BILINEAR_HPP_

#include "details.hpp"

namespace terry {
using namespace boost::gil;
namespace sampler {


struct bilinear_sampler{};

/**
 * @brief Get weight for a specific distance, for all bilinear resampler.
 *
 * @param[in] distance between the pixels and the current pixel
 * @param[out] weight return value to weight the pixel in filtering
**/
template < typename F >
bool getWeight ( const F distance, F& weight, bilinear_sampler sampler )
{
	if( distance < 0 )
	{
		TUTTLE_COUT("negative bilinear");
		if( distance > -1 )
		{
			weight = 1.0 + distance; // distance < 0 !
			return true;
		}
		else
		{
			if( distance > -2 )
			{
				weight = 0;
				return true;
			}
			return false;
		}
	}
	else
	{
		if( distance < 1 )
        {
                weight = distance;
                return true;
        }
        else
        {
                if( distance < 2 )
                {
                        weight = 0;
                        return true;
                }
                return false;
        }
	}
}

template <typename DstP, typename SrcView, typename F>
bool sample( bilinear_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
{

		/*
		 * pTL is the closest integer coordinate top left from p
		 *
		 *   pTL ---> x      x
		 *              o <------ p
		 *
		 *            x      x
		 */
		point2<std::ptrdiff_t> pTL( ifloor( p ) ); //

		// if we are outside the image, we return false to process miror/black operations
		if( 	pTL.x < -1 ||
				pTL.y < -1 ||
				pTL.x > src.width() - 1 ||
				pTL.y > src.height() - 1 )
		{
				return false;
		}

		// loc is the point in the source view
		typedef typename SrcView::xy_locator xy_locator;
		xy_locator loc = src.xy_at( pTL.x, pTL.y );
		point2<F> frac( p.x - pTL.x, p.y - pTL.y );

		ssize_t windowSize  = 4;             // 4 pixels:    A B C D

		std::vector<double> xWeights, yWeights;

		xWeights.assign( windowSize , 0);
		yWeights.assign( windowSize , 0);

		// get horizontal weight for each pixels
		/*for( ssize_t i = 0; i < windowSize; i++ )
		{
			// frac.x+2-i
			//frac.y+2-i
			getWeight( i-1-frac.x, xWeights.at(i), sampler );
			getWeight( i-1-frac.y, yWeights.at(i), sampler );
		}*/
		getWeight( 1+frac.x, xWeights.at(0), sampler );
		getWeight(   frac.x, xWeights.at(1), sampler );
		getWeight( 1-frac.x, xWeights.at(2), sampler );
		getWeight( 2-frac.x, xWeights.at(3), sampler );
		getWeight( 1+frac.y, yWeights.at(0), sampler );
		getWeight(   frac.y, yWeights.at(1), sampler );
		getWeight( 1-frac.y, yWeights.at(2), sampler );
		getWeight( 2-frac.y, yWeights.at(3), sampler );

		//process2Dresampling( Sampler& sampler, const SrcView& src, const point2<F>& p, const std::vector<double>& xWeights, const std::vector<double>& yWeights, const size_t& windowSize,typename SrcView::xy_locator& loc, DstP& result )
		bool res = details::process2Dresampling( sampler, src, p, xWeights, yWeights, windowSize, loc, result );

		return res;
}



}
}

#endif

