#ifndef _TERRY_SAMPLER_LANCZOS_HPP_
#define _TERRY_SAMPLER_LANCZOS_HPP_

#include "details.hpp"

#include <boost/math/constants/constants.hpp>

#include <cmath>
#include <limits>

#define TUTTLE_PI boost::math::constants::pi<RESAMPLING_CORE_TYPE>()

namespace terry
{
using namespace boost::gil;
namespace sampler
{

// source: http://src.chromium.org/svn/trunk/src/skia/ext/image_operations.cc

// Evaluates the Lanczos filter of the given filter size window for the given
// position.
//
// |filter_size| is the width of the filter (the "window"), outside of which
// the value of the function is 0. Inside of the window, the value is the
// normalized sinc function:
//   lanczos(x) = sinc(x) * sinc(x / filter_size);
// where
//   sinc(x) = sin(pi*x) / (pi*x);

// float EvalLanczos(int filter_size, float x) {
//  if (x <= -filter_size || x >= filter_size)
//    return 0.0f;  // Outside of the window.
//  if (x > -std::numeric_limits<float>::epsilon() &&
//      x < std::numeric_limits<float>::epsilon())
//    return 1.0f;  // Special case the discontinuity at the origin.
//  float xpi = x * static_cast<float>( boost::math::constants::pi<float>() );
//  return (sin(xpi) / xpi) *  // sinc(x)
//          sin(xpi / filter_size) / (xpi / filter_size);  // sinc(x/filter_size)
//}

struct lanczos_sampler
{
    const size_t _windowSize;
    const RESAMPLING_CORE_TYPE _sharpen;

    lanczos_sampler(std::size_t filterSize, RESAMPLING_CORE_TYPE sharpen)
        : _windowSize(filterSize * 2)
        , _sharpen(sharpen)
    {
    }

    RESAMPLING_CORE_TYPE sinc(RESAMPLING_CORE_TYPE x)
    {
        if(x > -std::numeric_limits<RESAMPLING_CORE_TYPE>::epsilon() &&
           x < std::numeric_limits<RESAMPLING_CORE_TYPE>::epsilon())
        {
            // Special case the discontinuity at the origin.
            return 1.f;
        }
        RESAMPLING_CORE_TYPE xpi = x * TUTTLE_PI;
        return sinf(xpi) / (xpi);
    }

    template <typename Weight>
    void operator()(const RESAMPLING_CORE_TYPE& distance, Weight& weight)
    {
        weight = sinc(distance) * sinc(_sharpen * distance / _windowSize);
    }
};

struct lanczos3_sampler : public lanczos_sampler
{
    lanczos3_sampler()
        : lanczos_sampler(3.0, 1.0)
    {
    }
};

struct lanczos4_sampler : public lanczos_sampler
{
    lanczos4_sampler()
        : lanczos_sampler(4.0, 1.0)
    {
    }
};

struct lanczos6_sampler : public lanczos_sampler
{
    lanczos6_sampler()
        : lanczos_sampler(6.0, 1.0)
    {
    }
};

struct lanczos12_sampler : public lanczos_sampler
{
    lanczos12_sampler()
        : lanczos_sampler(12.0, 1.0)
    {
    }
};
}
}

#endif
