#include "TetraInterpolator.hpp"
#include "Color.hpp"
#include "AbstractLut.hpp"

#include <cmath>

namespace tuttle
{

template <class T>
T clamp(const T v, const T min, const T max)
{
    return std::max(min, std::min(v, max));
}

Color TetraInterpolator::interpolate(const AbstractLut* lut, const double x, const double y, const double z) const
{
    const std::size_t dimSize = lut->dimSize();

    const double xc = clamp(x, 0.0, 1.0);
    const double yc = clamp(y, 0.0, 1.0);
    const double zc = clamp(z, 0.0, 1.0);
    const unsigned int x0 = (unsigned int)std::floor(xc * (dimSize - 1));
    const unsigned int y0 = (unsigned int)std::floor(yc * (dimSize - 1));
    const unsigned int z0 = (unsigned int)std::floor(zc * (dimSize - 1));

    // par defaut le pas de la grille est de 1
    const unsigned int x1 = x0 + 1;
    const unsigned int y1 = y0 + 1;
    const unsigned int z1 = z0 + 1;

    const Color p000 = lut->getIndexedColor(x0, y0, z0);
    const Color p001 = lut->getIndexedColor(x0, y0, z1);
    const Color p010 = lut->getIndexedColor(x0, y1, z0);
    const Color p011 = lut->getIndexedColor(x0, y1, z1);
    const Color p100 = lut->getIndexedColor(x1, y0, z0);
    const Color p101 = lut->getIndexedColor(x1, y0, z1);
    const Color p110 = lut->getIndexedColor(x1, y1, z0);
    const Color p111 = lut->getIndexedColor(x1, y1, z1);

    const double dx = x * (dimSize - 1.0) - (double)x0;
    const double dy = y * (dimSize - 1.0) - (double)y0;
    const double dz = z * (dimSize - 1.0) - (double)z0;

    const Color c0 = p000;

    Color c1, c2, c3;
    if(dx >= dy && dy >= dz) // T1
    {
        c1 = (p100 - p000);
        c2 = (p110 - p100);
        c3 = (p111 - p110);
    }
    else if(dx >= dz && dz >= dy) // T2
    {
        c1 = (p100 - p000);
        c2 = (p111 - p101);
        c3 = (p101 - p100);
    }
    else if(dz >= dx && dx >= dy) // T3
    {
        c1 = (p101 - p001);
        c2 = (p111 - p101);
        c3 = (p001 - p000);
    }
    else if(dy >= dx && dx >= dz) // T4
    {
        c1 = (p110 - p010);
        c2 = (p010 - p000);
        c3 = (p111 - p110);
    }
    else if(dy >= dz && dz >= dx) // T5
    {
        c1 = (p111 - p011);
        c2 = (p010 - p000);
        c3 = (p011 - p010);
    }
    else if(dz >= dy && dy >= dx) // T6
    {
        c1 = (p111 - p011);
        c2 = (p011 - p001);
        c3 = (p001 - p000);
    }
    c1 *= dx;
    c2 *= dy;
    c3 *= dz;

    return Color(c0.x + c1.x + c2.x + c3.x, c0.y + c1.y + c2.y + c3.y, c0.z + c1.z + c2.z + c3.z);
}
};
