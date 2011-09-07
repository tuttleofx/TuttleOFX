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
bool getWeight ( const long int&  pTLXOrY, const F distance, const size_t index, F& weight, bilinear_sampler sampler )
{
	if( pTLXOrY < 0 ) // in case of pTL < 0  (equal to -1)
	{
		if( index == 0 )
		{
			weight = 1.0;
			return true;
		}
		else
		{
			weight = 0.0;
			return true;
		}
	}

	if( distance < 1 )
    {
		weight = 1.0 - distance;
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

		// get weight for each pixels
/*
		getWeight( pTL.x, 1+frac.x, 0, xWeights.at(0), sampler );
		getWeight( pTL.x,   frac.x, 1, xWeights.at(1), sampler );
		getWeight( pTL.x, 1-frac.x, 2, xWeights.at(2), sampler );
		getWeight( pTL.x, 2-frac.x, 3, xWeights.at(3), sampler );
		getWeight( pTL.y, 1+frac.y, 0, yWeights.at(0), sampler );
		getWeight( pTL.y,   frac.y, 1, yWeights.at(1), sampler );
		getWeight( pTL.y, 1-frac.y, 2, yWeights.at(2), sampler );
		getWeight( pTL.y, 2-frac.y, 3, yWeights.at(3), sampler );
*/
		for(int i=0; i<windowSize; i++)
		{
			int coef = (i>1)? -1 : 1;
			getWeight( pTL.x, std::abs(i-1) + coef * frac.x, i, xWeights.at(i), sampler );
			getWeight( pTL.y, std::abs(i-1) + coef * frac.y, i, yWeights.at(i), sampler );
		}

		// process current sample
		bool res = details::process2Dresampling( sampler, src, p, xWeights, yWeights, windowSize, loc, result );

		return res;
}



}
}

#endif

