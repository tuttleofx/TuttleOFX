#ifndef TETRA_INTERPOLATOR_H
#define TETRA_INTERPOLATOR_H

#include "hd3d_interpolator.h"
#include "hd3d_abstract_lut.h"
#include "hd3d_color.h"

namespace tuttle
{

class TetraInterpolator : public Interpolator
{
public:
	TetraInterpolator() : Interpolator() {}
	Color interpolate( const AbstractLut* lut, const double x, const double y, const double z ) const;
};

};

#endif //TETRA_INTERPOLATOR_H
