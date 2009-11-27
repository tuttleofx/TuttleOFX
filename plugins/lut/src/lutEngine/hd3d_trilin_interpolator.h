#ifndef TRILIN_INTERPOLATOR_H
#define TRILIN_INTERPOLATOR_H

#include "hd3d_interpolator.h"
#include "hd3d_abstract_lut.h"
#include "hd3d_color.h"

namespace tuttle
{

class TrilinInterpolator : public Interpolator {
    public:
        TrilinInterpolator(): Interpolator() { };
        Color interpolate(const AbstractLut *lut, const double x, const double y, const double z) const;
};

};

#endif //TRILIN_INTERPOLATOR_H
