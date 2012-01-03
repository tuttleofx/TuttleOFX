#ifndef _TERRY_SAMPLER_HPP_
#define _TERRY_SAMPLER_HPP_

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace terry {
namespace sampler {

#ifndef RESAMPLING_CORE_TYPE
// type of internal variables used for resampling
#define RESAMPLING_CORE_TYPE float
#endif

BOOST_STATIC_ASSERT( boost::is_floating_point<RESAMPLING_CORE_TYPE>::value );

enum EParamFilter
{
	eParamFilterNearest = 0,
	eParamFilterBilinear,
	eParamFilterBC,
	eParamFilterBicubic,
	eParamFilterCatrom,
	eParamFilterKeys,
	eParamFilterSimon,
	eParamFilterRifman,
	eParamFilterMitchell,
	eParamFilterParzen,
	eParamFilterLanczos,
	eParamFilterLanczos3,
	eParamFilterLanczos4,
	eParamFilterLanczos6,
	eParamFilterLanczos12,
	eParamFilterGaussian
};

enum EParamFilterOutOfImage
{
	eParamFilterOutBlack = 0,
	eParamFilterOutTransparency,
	eParamFilterOutCopy,
	eParamFilterOutMirror
};

}
}

#endif

