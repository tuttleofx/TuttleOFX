#include "TrilinInterpolator.hpp"
#include "Color.hpp"
#include "AbstractLut.hpp"
#include <cmath>

namespace tuttle
{

Color TrilinInterpolator::interpolate(const AbstractLut* lut, const double x, const double y, const double z) const
{
    Color p000, p001, p010, p011, p100, p101, p110, p111;
    unsigned int x0, y0, z0, x1, y1, z1;
    unsigned int x1MinusX0, y1MinusY0, z1MinusZ0;
    double oneByX1MinusX0, oneByY1MinusY0, oneByZ1MinusZ0;
    double dx, dy, dz;
    Color c0, c1, c2, c3, c4, c5, c6, c7;
    Color black(0., 0., 0.);
    size_t dimSize = lut->dimSize();

    x0 = (unsigned int)std::floor(x * (dimSize - 1));
    y0 = (unsigned int)std::floor(y * (dimSize - 1));
    z0 = (unsigned int)std::floor(z * (dimSize - 1));

    x1 = x0 + 1;
    y1 = y0 + 1;
    z1 = z0 + 1;

    if(x1 >= dimSize)
        x1 = dimSize - 1;
    if(y1 >= dimSize)
        y1 = dimSize - 1;
    if(z1 >= dimSize)
        z1 = dimSize - 1;

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
    x1MinusX0 = x1 - x0;
    y1MinusY0 = y1 - y0;
    z1MinusZ0 = z1 - z0;
    oneByX1MinusX0 = 1.0 / (double)x1MinusX0;
    oneByY1MinusY0 = 1.0 / (double)y1MinusY0;
    oneByZ1MinusZ0 = 1.0 / (double)z1MinusZ0;
    if(x1MinusX0)
    {
        c1 = (p100 - p000) * oneByX1MinusX0;
        c1 *= dx;
    }
    else
    {
        c1 = black;
    }
    if(y1MinusY0)
    {
        c2 = (p010 - p000) * oneByY1MinusY0;
        c2 *= dy;
    }
    else
        c2 = black;
    if(z1MinusZ0)
    {
        c3 = (p001 - p000) * oneByZ1MinusZ0;
        c3 *= dz;
    }
    else
        c3 = black;
    if(x1MinusX0 * y1MinusY0)
    {
        c4 = (p110 - p010 - p100 + p000) * (oneByX1MinusX0 * oneByY1MinusY0);
        c4 *= dx * dy;
    }
    else
        c4 = black;
    if(x1MinusX0 * z1MinusZ0)
    {
        c5 = (p101 - p001 - p100 + p000) * (oneByX1MinusX0 * oneByZ1MinusZ0);
        c5 *= dx * dz;
    }
    else
        c6 = black;
    if(y1MinusY0 * z1MinusZ0)
    {
        c6 = (p011 - p001 - p010 + p000) * (oneByY1MinusY0 * oneByZ1MinusZ0);
        c6 *= dy * dz;
    }
    else
        c6 = black;
    if(x1MinusX0 * y1MinusY0 * z1MinusZ0)
    {
        c7 = (p111 - p011 - p101 - p110 + p100 + p001 + p010 - p000) * (oneByX1MinusX0 * oneByY1MinusY0 * oneByZ1MinusZ0);
        c7 *= dx * dy * dz;
    }
    else
        c7 = black;

    return Color(c0.x + c1.x + c2.x + c3.x + c4.x + c5.x + c6.x + c7.x,
                 c0.y + c1.y + c2.y + c3.y + c4.y + c5.y + c6.y + c7.y,
                 c0.z + c1.z + c2.z + c3.z + c4.z + c5.z + c6.z + c7.z);
}
};
