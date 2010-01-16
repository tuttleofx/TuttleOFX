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
	inline virtual Color interpolate( const AbstractLut* lut, const Color& color ) const;
};

inline Color Interpolator::interpolate( const AbstractLut* lut, const Color& color ) const
{
	return interpolate( lut, color.x, color.y, color.z );
}

};

#endif //INTERPOLATOR_H
