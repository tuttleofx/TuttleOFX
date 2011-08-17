
#ifndef _TERRY_SAMPLER_BC_HPP_
#define _TERRY_SAMPLER_BC_HPP_

#include "details.hpp"

namespace terry {
using namespace boost::gil;
namespace sampler {

struct bc_sampler
{
    double valB;
    double valC;
};

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

template <typename DstP, typename SrcView, typename F>
bool sample( bc_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
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


		// get horizontal weight for each pixels
		for( ssize_t i = 0; i < windowSize; i++ )
		{
				getWeight( frac.x+2-i, xWeights.at(i), sampler );
				getWeight( frac.y+2-i, yWeights.at(i), sampler );
		}
		//process2Dresampling( Sampler& sampler, const SrcView& src, const point2<F>& p, const std::vector<double>& xWeights, const std::vector<double>& yWeights, const size_t& windowSize,typename SrcView::xy_locator& loc, DstP& result )
		bool res = details::process2Dresampling( sampler, src, p, xWeights, yWeights, windowSize, loc, result );

		return res;
}

}
}

#endif
