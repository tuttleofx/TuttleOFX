#include "Lut.hpp"
#include "TrilinInterpolator.hpp"
#include "LutReader.hpp"

namespace tuttle
{

Lut3D::Lut3D(Interpolator* interpolator, size_t dimSize, double* data /* = NULL*/)
    : AbstractLut(interpolator, dimSize, data)
{
    if(data == NULL)
    {
        _data = new double[totalSize() * 3];
        memset(_data, 0, sizeof(double) * totalSize() * 3);
    }
}

Lut3D::Lut3D(Lut3D const& lut3d)
{
    _shared = true;
    _interpolator = lut3d._interpolator;
    _dimSize = lut3d._dimSize;
    _data = lut3d._data;
}
}
