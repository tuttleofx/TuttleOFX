#ifndef _LUTENGINE_TRILININTERPOLATOR_HPP_
#define _LUTENGINE_TRILININTERPOLATOR_HPP_

#include "Interpolator.hpp"
#include "AbstractLut.hpp"
#include "Color.hpp"

namespace tuttle
{

class TrilinInterpolator : public Interpolator
{
public:
    TrilinInterpolator()
        : Interpolator()
    {
    }
    Color interpolate(const AbstractLut* lut, const double x, const double y, const double z) const;
};
}

#endif
