#ifndef _TERRY_SAMPLER_BC_HPP_
#define _TERRY_SAMPLER_BC_HPP_

#include "details.hpp"

namespace terry
{
using namespace boost::gil;
namespace sampler
{

struct bc_sampler
{
    const size_t _windowSize;
    const RESAMPLING_CORE_TYPE _valB;
    const RESAMPLING_CORE_TYPE _valC;

    bc_sampler()
        : _windowSize(4.0)
        , // 4 pixels:    A B C D
        _valB(0.0)
        , _valC(0.0)
    {
    }

    bc_sampler(RESAMPLING_CORE_TYPE valB, RESAMPLING_CORE_TYPE valC)
        : _windowSize(4.0)
        , // 4 pixels:    A B C D
        _valB(valB)
        , _valC(valC)
    {
    }

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
    template <typename Weight>
    void operator()(const RESAMPLING_CORE_TYPE& distance, Weight& weight)
    {
        RESAMPLING_CORE_TYPE absDistance = std::abs((RESAMPLING_CORE_TYPE)distance);
        if(absDistance <= 1.0)
        {
            RESAMPLING_CORE_TYPE P = 12.0 - 9.0 * _valB - 6.0 * _valC;
            RESAMPLING_CORE_TYPE Q = -18.0 + 12.0 * _valB + 6.0 * _valC;
            RESAMPLING_CORE_TYPE S = 6.0 - 2.0 * _valB;
            // note: R is null
            weight = ((P * absDistance + Q) * absDistance * absDistance + S) / 6.0;
        }
        else
        {
            if(absDistance < 2.0)
            {
                RESAMPLING_CORE_TYPE T = -_valB - 6.0 * _valC;
                RESAMPLING_CORE_TYPE U = 6.0 * _valB + 30.0 * _valC;
                RESAMPLING_CORE_TYPE V = -12.0 * _valB - 48.0 * _valC;
                RESAMPLING_CORE_TYPE W = 8.0 * _valB + 24.0 * _valC;
                weight = (((T * absDistance + U) * absDistance + V) * absDistance + W) / 6.0;
                return;
            }
            weight = 0.0;
        }
    }
};

//
// valC is equal to -a in the equation
//
struct cubic_sampler : bc_sampler
{
    cubic_sampler()
        : bc_sampler(0.0, 0.0)
    {
    }
    cubic_sampler(RESAMPLING_CORE_TYPE a)
        : bc_sampler(0.0, -a)
    {
    }
};

struct bicubic_sampler : cubic_sampler
{
    bicubic_sampler()
        : cubic_sampler(0.0)
    {
    }
};

// catmul-rom resampling function
struct catrom_sampler : cubic_sampler
{
    catrom_sampler()
        : cubic_sampler(-0.5)
    {
    }
};

// equal to catrom resampling function
struct keys_sampler : cubic_sampler
{
    keys_sampler()
        : cubic_sampler(-0.5)
    {
    }
};

struct simon_sampler : cubic_sampler
{
    simon_sampler()
        : cubic_sampler(-0.75)
    {
    }
};

struct rifman_sampler : cubic_sampler
{
    rifman_sampler()
        : cubic_sampler(-1.0)
    {
    }
};

struct mitchell_sampler : bc_sampler
{
    mitchell_sampler()
        : bc_sampler(1.0 / 3.0, 1.0 / 3.0)
    {
    }
};

struct parzen_sampler : bc_sampler
{
    parzen_sampler()
        : bc_sampler(1.0, 0.0)
    {
    }
};
}
}

#endif
