#ifndef _TERRY_SAMPLER_NEARESTNEIGHBOR_HPP_
#define _TERRY_SAMPLER_NEARESTNEIGHBOR_HPP_

#include "details.hpp"

namespace terry {
using namespace boost::gil;
namespace sampler {

struct nearest_neighbor_sampler {};

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

}
}

#endif

