#ifndef _TERRY_SAMPLER_BILINEAR_HPP_
#define _TERRY_SAMPLER_BILINEAR_HPP_

#include "details.hpp"

namespace terry {
using namespace boost::gil;
namespace sampler {

struct bilinear_sampler
{
	const size_t _windowSize;

	bilinear_sampler() :
		_windowSize ( 2.0 )
	{}

	/**
	 * @brief Get weight for a specific distance, for all bilinear resampler.
	 *
	 * @param[in] distance between the pixels and the current pixel
	 * @param[out] weight return value to weight the pixel in filtering
	 */
	template<typename Weight>
	void operator()( const RESAMPLING_CORE_TYPE& distance, Weight& weight )
	{
		RESAMPLING_CORE_TYPE absDistance = std::abs( (RESAMPLING_CORE_TYPE) distance );
		if( absDistance < 1 )
		{
			weight = ( 1.0 - absDistance );
		}
		else
		{
			weight = 0;
		}
	}
};

}
}

#endif

