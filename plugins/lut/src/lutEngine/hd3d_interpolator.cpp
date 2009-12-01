#include "hd3d_interpolator.h"
#include "hd3d_color.h"
#include "hd3d_abstract_lut.h"

namespace tuttle
{

Color Interpolator::interpolate( const AbstractLut* lut, const Color& color ) const
{
	return interpolate( lut, color.x, color.y, color.z );
}

};
