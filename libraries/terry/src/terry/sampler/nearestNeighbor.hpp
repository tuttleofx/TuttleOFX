#ifndef _TERRY_SAMPLER_NEARESTNEIGHBOR_HPP_
#define _TERRY_SAMPLER_NEARESTNEIGHBOR_HPP_

#include "details.hpp"

namespace terry {
using namespace boost::gil;
namespace sampler {

struct nearest_neighbor_sampler {};

template <typename DstP, typename SrcView, typename F>
bool sample( nearest_neighbor_sampler, const SrcView& src, const point2<F>& p, DstP& result, const int& outOfImageProcess )
{
	point2<std::ptrdiff_t> center( iround( p ) );

	// if we are outside the image
        if( center.x < 0 )
        {
            switch( outOfImageProcess )
            {
                case 0 : return false; // black pixel
                case 1 : center.x = 0;
                case 2 : center.x = -center.x - 1;
            }
        }
        if( center.x > src.width() - 1.0 )
        {
            switch( outOfImageProcess )
            {
                case 0 : return false; // black pixel
                case 1 : center.x = src.width() - 1.0;
                case 2 : center.x = src.width() - 1.0 - center.x ;
            }
        }
        if( center.y < 0 )
        {
            switch( outOfImageProcess )
            {
                case 0 : return false; // black pixel
                case 1 : center.y = 0;
                case 2 : center.y = -center.y - 1;
            }
        }

        if( center.y > src.height() - 1.0 )
        {
            switch( outOfImageProcess )
            {
                case 0 : return false; // black pixel
                case 1 : center.y = src.height() - 1.0;
                case 2 : center.y = src.height() - 1.0 - center.y;
            }
	}

	result = src( center.x, center.y );
	return true;
}

}
}

#endif

