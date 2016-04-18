#ifndef _LUTENGINE_COLOR_HPP_
#define _LUTENGINE_COLOR_HPP_

#include <ostream>

namespace tuttle
{

class Color
{
public:
    Color()
        : x(0)
        , y(0)
        , z(0)
    {
    }
    Color(double _x, double _y, double _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }
    Color(const Color& _color)
        : x(_color.x)
        , y(_color.y)
        , z(_color.z)
    {
    }
    inline bool operator==(const Color _color) const;
    inline bool operator!=(const Color _color) const;
    inline Color operator-(const Color _color) const;
    inline Color operator+(const Color _color) const;
    inline Color operator*(const Color _color) const;
    inline Color operator*(double value) const;
    inline Color operator/(const Color _color) const;
    inline Color operator/(double value) const;

    inline Color operator=(const Color _color);
    inline Color operator+=(const Color _color);
    inline Color operator-=(const Color _color);
    inline Color operator*=(const Color _color);
    inline Color operator*=(double _value);
    inline Color operator/=(const Color _color);
    inline Color operator/=(double _value);

    friend std::ostream& operator<<(std::ostream& o, Color const& _color);
    double x, y, z;
};

inline Color Color::operator=(const Color _color)
{
    x = _color.x;
    y = _color.y;
    z = _color.z;
    return (*this);
}

inline Color Color::operator-(const Color _color) const
{
    return (Color(x - _color.x, y - _color.y, z - _color.z));
}

inline Color Color::operator+(const Color _color) const
{
    return (Color(x + _color.x, y + _color.y, z + _color.z));
}

inline Color Color::operator*(const Color _color) const
{
    return (Color(x * _color.x, y * _color.y, z * _color.z));
}

inline Color Color::operator*(double _value) const
{
    return (Color(x * _value, y * _value, z * _value));
}

inline Color Color::operator/(double _value) const
{
    return (Color(x / _value, y / _value, z / _value));
}

inline Color Color::operator/(const Color _color) const
{
    return (Color(x / _color.x, y / _color.y, z / _color.z));
}

inline Color Color::operator+=(const Color _color)
{
    x += _color.x;
    y += _color.y;
    z += _color.z;
    return (*this);
}

inline Color Color::operator-=(const Color _color)
{
    x -= _color.x;
    y -= _color.y;
    z -= _color.z;
    return (*this);
}

inline Color Color::operator*=(const Color _color)
{
    x *= _color.x;
    y *= _color.y;
    z *= _color.z;
    return (*this);
}

inline Color Color::operator*=(double _value)
{
    x *= _value;
    y *= _value;
    z *= _value;
    return (*this);
}

inline Color Color::operator/=(const Color _color)
{
    x /= _color.x;
    y /= _color.y;
    z /= _color.z;
    return (*this);
}

inline Color Color::operator/=(double _value)
{
    x /= _value;
    y /= _value;
    z /= _value;
    return (*this);
}

inline bool Color::operator==(const Color _color) const
{
    if(x == _color.x && y == _color.y && z == _color.z)
        return true;
    return false;
}

inline bool Color::operator!=(const Color _color) const
{
    return !((*this) == _color);
}

inline std::ostream& operator<<(std::ostream& o, Color const& _color)
{
    return o << "(" << _color.x << ", " << _color.y << ", " << _color.z << ")";
}
}

#endif
