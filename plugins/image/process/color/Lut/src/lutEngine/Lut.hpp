#ifndef _LUTENGINE_LUT_HPP_
#define _LUTENGINE_LUT_HPP_

#include "AbstractLut.hpp"
#include "Color.hpp"
#include "Interpolator.hpp"

namespace tuttle
{

class Lut3D : public AbstractLut
{
public:
    Lut3D()
        : AbstractLut()
    {
    }

    Lut3D(Interpolator* interpolator, size_t _dimSize, double* data = NULL);
    Lut3D(const Lut3D& lut3d);

    const size_t totalSize() const;
    inline Color getIndexedColor(int _x, int _y, int _z) const;
    inline Color getColor(Color const& color) const { return _interpolator->interpolate(this, color); }
    inline Color getColor(double _r, double _g, double _b) const { return _interpolator->interpolate(this, _r, _g, _b); }
    inline void setIndexedColor(int _x, int _y, int _z, Color _color);
    inline void setIndexedValues(int _x, int _y, int _z, double _r, double _g, double _b);

private:
    inline int getIndex(const int _x, const int _y, const int _z) const
    {
        return (((_x * _dimSize * _dimSize) + (_y * _dimSize) + _z) * 3);
    }
};

inline const size_t Lut3D::totalSize() const
{
    return _dimSize * _dimSize * _dimSize;
}

inline Color Lut3D::getIndexedColor(int _x, int _y, int _z) const
{
    int ref = getIndex(_x, _y, _z);

    return Color(_data[ref], _data[ref + 1], _data[ref + 2]);
}

inline void Lut3D::setIndexedColor(int _x, int _y, int _z, Color _color)
{
    setIndexedValues(_x, _y, _z, _color.x, _color.y, _color.z);
}

inline void Lut3D::setIndexedValues(int _x, int _y, int _z, double _r, double _g, double _b)
{
    int ref = getIndex(_x, _y, _z);

    _data[ref] = _r;
    _data[ref + 1] = _g;
    _data[ref + 2] = _b;
}
}

#endif // HD3DLUT_H
