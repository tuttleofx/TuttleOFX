#ifndef _TERRY_SAMPLER_GAUSSIAN_HPP_
#define _TERRY_SAMPLER_GAUSSIAN_HPP_

#include "details.hpp"
#include <boost/math/constants/constants.hpp>

namespace terry {
using namespace boost::gil;
namespace sampler {

// from http://avisynth.org/mediawiki/Resampling#Gaussian_resampler


struct gaussian_sampler
{
	const size_t         _windowSize;
	RESAMPLING_CORE_TYPE _sigma;

	gaussian_sampler() :
		_windowSize ( 4.0 ), // size = 3.0
		_sigma      ( 1.0 )
	{
	}

	gaussian_sampler( size_t windowSize, size_t sigma ) :
		_windowSize ( windowSize + 1 ),
		_sigma      ( sigma )
	{
	}
	
	template< typename Weight >
	void operator()( const RESAMPLING_CORE_TYPE& distance, Weight& weight )
	{
		if( _sigma > -std::numeric_limits<RESAMPLING_CORE_TYPE>::epsilon() &&
		    _sigma < std::numeric_limits<RESAMPLING_CORE_TYPE>::epsilon())
		{
			weight = 0.0;
			return;
		}
		//weight = std::pow( 2.f, - 0.1 * _sigma * distance * distance );
		weight = 1.0 / ( _sigma * std::sqrt( 2 * boost::math::constants::pi<RESAMPLING_CORE_TYPE>() )) * std::exp( - distance * distance / ( 2 * _sigma ) ) ;
	}
};

}
}

#endif

