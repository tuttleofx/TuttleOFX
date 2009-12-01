#include "hd3d_tetra_interpolator.h"
#include "hd3d_color.h"
#include "hd3d_abstract_lut.h"
#include <cmath>

using namespace std;

namespace tuttle
{

Color TetraInterpolator::interpolate(const AbstractLut *lut, const double x, const double y, const double z) const {
    Color p000, p001, p010, p011, p100, p101, p110, p111;
    unsigned int x0, y0, z0, x1, y1, z1;
    double dx, dy, dz;
    Color c0, c1, c2, c3;
    size_t dimSize = lut->dimSize();

    x0 = (unsigned int)std::floor(x * (dimSize-1));
    y0 = (unsigned int)std::floor(y * (dimSize-1));
    z0 = (unsigned int)std::floor(z * (dimSize-1));

    // par defaut le pas de la grille est de 1
    x1 = x0 + 1;
    y1 = y0 + 1;
    z1 = z0 + 1;

    if ( x1 >= dimSize) x1 = dimSize-1;
    if ( y1 >= dimSize) y1 = dimSize-1;
    if ( z1 >= dimSize) z1 = dimSize-1;

    p000 = lut->getIndexedColor(x0, y0, z0);
    p001 = lut->getIndexedColor(x0, y0, z1);
    p010 = lut->getIndexedColor(x0, y1, z0);
    p011 = lut->getIndexedColor(x0, y1, z1);
    p100 = lut->getIndexedColor(x1, y0, z0);
    p101 = lut->getIndexedColor(x1, y0, z1);
    p110 = lut->getIndexedColor(x1, y1, z0);
    p111 = lut->getIndexedColor(x1, y1, z1);

    dx = x * (dimSize - 1.0) - (double)x0;
    dy = y * (dimSize - 1.0) - (double)y0;
    dz = z * (dimSize - 1.0) - (double)z0;

    c0 = p000;

    if( dx >= dy && dy >= dz )     // T1
    {
        c1 = (p100 - p000); c2 = (p110 - p100);c3 = (p111 - p110);
    }
    else if( dx >= dz && dz >= dy )// T2
    {
        c1 = (p100 - p000); c2 = (p111 - p101); c3 = (p101 - p100);
    }
    else if( dz >= dx && dx >= dy )// T3
    {
        c1 = (p101 - p001); c2 = (p111 - p101); c3 = (p001 - p000);
    }
    else if( dy >= dx && dx >= dz )// T4
    {
        c1 = (p110 - p010); c2 = (p010 - p000); c3 = (p111 - p110);
    }
    else if( dy >= dz && dz >= dx )// T5
    {
        c1 = (p111 - p011); c2 = (p010 - p000); c3 = (p011 - p010);
    }
    else if( dz >= dy && dy >= dx )// T6
    {
        c1 = (p111 - p011); c2 = (p011 - p001); c3 = (p001 - p000);
    }
    c1 *= dx; c2 *= dy; c3 *= dz;

    return Color(c0.x + c1.x + c2.x + c3.x,
                 c0.y + c1.y + c2.y + c3.y,
                 c0.z + c1.z + c2.z + c3.z );
}

};
