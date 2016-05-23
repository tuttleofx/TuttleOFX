#ifndef _LUTENGINE_TETRA_INTERPOLATOR_HPP_
#define _LUTENGINE_TETRA_INTERPOLATOR_HPP_

#include "Interpolator.hpp"
#include "AbstractLut.hpp"
#include "Color.hpp"

namespace tuttle
{

class TetraInterpolator : public Interpolator
{
public:
    TetraInterpolator()
        : Interpolator()
    {
    }
    Color interpolate(const AbstractLut* lut, const double x, const double y, const double z) const;
};
}

#endif
