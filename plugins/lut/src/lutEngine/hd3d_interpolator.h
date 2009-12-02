#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "hd3d_color.h"

namespace tuttle
{

class AbstractLut;

class Interpolator
{
public:
	Interpolator() {}
	virtual ~Interpolator() {}
	virtual Color interpolate( const AbstractLut* lut, const double x, const double y, const double z ) const = 0;
	virtual Color interpolate( const AbstractLut* lut, const Color& color ) const;
};

};

#endif //INTERPOLATOR_H
