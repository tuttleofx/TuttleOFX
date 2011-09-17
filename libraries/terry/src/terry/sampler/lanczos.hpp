#ifndef _TERRY_SAMPLER_LANCZOS_HPP_
#define _TERRY_SAMPLER_LANCZOS_HPP_

#include "details.hpp"
#include <cmath>

double PI = std::atan(1.0f) * 4.0f;

namespace terry {
using namespace boost::gil;
namespace sampler {

struct lanczos_sampler{
	size_t size;
	lanczos_sampler()
	{
		size = 3.0;
	}
};
struct lanczos3_sampler{};
struct lanczos4_sampler{};
struct lanczos6_sampler{};
struct lanczos12_sampler{};

template < typename F >
void getLanczosWeight( const F& distance, F& weight, lanczos_sampler& sampler )
{
	if( distance == 0.0 )
	{
		weight = 1.0;
		return;
	}
	weight = sampler.size * sin( distance * PI ) * sin( distance * PI / sampler.size ) / ( pow( PI * distance, 2 ) );
}


template <typename DstP, typename SrcView, typename F>
bool sample( lanczos_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
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
	/*if( 	pTL.x < -1 ||
		pTL.y < -1 ||
		pTL.x > src.width() - 1 ||
		pTL.y > src.height() - 1 )
	{
		return false;
	}*/

	// loc is the point in the source view
	typedef typename SrcView::xy_locator xy_locator;
	xy_locator loc = src.xy_at( pTL.x, pTL.y );
	point2<F> frac( p.x - pTL.x, p.y - pTL.y );

	ssize_t windowSize  = sampler.size;

	std::vector<double> xWeights, yWeights;

	xWeights.assign( windowSize , 0);
	yWeights.assign( windowSize , 0);

	size_t middlePosition = floor((windowSize - 1) * 0.5);

	//xWeights.at(middlePosition) = 1.0;
	//yWeights.at(middlePosition) = 1.0;

	// get horizontal weight for each pixels

	for( ssize_t i = 0; i < windowSize; i++ )
	{
		getLanczosWeight( std::abs( (i-middlePosition) - frac.x ), xWeights.at(i), sampler );
		getLanczosWeight( std::abs( (i-middlePosition) - frac.y ), yWeights.at(i), sampler );
	}

	// process current sample
	bool res = details::process2Dresampling( sampler, src, p, xWeights, yWeights, windowSize, loc, result );

	return res;
}

template <typename DstP, typename SrcView, typename F>
bool sample( lanczos3_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	lanczos_sampler s;
	s.size = 3;
	return sample( s, src, p, result );
}

template <typename DstP, typename SrcView, typename F>
bool sample( lanczos4_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	lanczos_sampler s;
	s.size = 4;
	return sample( s, src, p, result );
}

template <typename DstP, typename SrcView, typename F>
bool sample( lanczos6_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	lanczos_sampler s;
	s.size = 6;
	return sample( s, src, p, result );
}

template <typename DstP, typename SrcView, typename F>
bool sample( lanczos12_sampler sampler, const SrcView& src, const point2<F>& p, DstP& result )
{
	lanczos_sampler s;
	s.size = 12;
	return sample( s, src, p, result );
}

}
}

#endif

