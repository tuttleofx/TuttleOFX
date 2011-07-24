
#ifndef _TERRY_SAMPLER_NEARESTNEIGHBOR_HPP_
#define _TERRY_SAMPLER_NEARESTNEIGHBOR_HPP_

#include "details.hpp"

namespace terry {
using namespace boost::gil;
namespace sampler {

struct nearest_neighbor_sampler {};

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
