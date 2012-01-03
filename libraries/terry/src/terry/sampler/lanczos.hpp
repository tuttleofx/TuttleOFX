#ifndef _TERRY_SAMPLER_LANCZOS_HPP_
#define _TERRY_SAMPLER_LANCZOS_HPP_

#include "details.hpp"
#include <cmath>
#include <boost/math/constants/constants.hpp>

namespace terry {
using namespace boost::gil;
namespace sampler {

struct lanczos_sampler{
	const size_t _windowSize;

	lanczos_sampler() :
		_windowSize ( 3.0 )
	{
	}

	lanczos_sampler( size_t windowSize ) :
		_windowSize ( windowSize )
	{
	}

	template<typename Weight>
	void operator()( const RESAMPLING_CORE_TYPE& distance, Weight& weight )
	{
		if( distance == 0.0 )
		{
			weight = 1.0;
			return;
		}
		weight = sin( boost::math::constants::pi<RESAMPLING_CORE_TYPE>() * distance ) * sin( ( boost::math::constants::pi<RESAMPLING_CORE_TYPE>() / _windowSize ) * distance ) / ( ( boost::math::constants::pi<RESAMPLING_CORE_TYPE>() * boost::math::constants::pi<RESAMPLING_CORE_TYPE>() / _windowSize ) * distance * distance );
	}
};

struct lanczos3_sampler : public lanczos_sampler
{
	lanczos3_sampler() {}
};

struct lanczos4_sampler : public lanczos_sampler
{
	lanczos4_sampler() :
		lanczos_sampler( 4.0 )
	{
	}
};

struct lanczos6_sampler : public lanczos_sampler
{
	lanczos6_sampler() :
		lanczos_sampler( 6.0 )
	{
	}
};

struct lanczos12_sampler : public lanczos_sampler
{
	lanczos12_sampler() :
		lanczos_sampler( 12.0 )
	{
	}
};

}
}

#endif

