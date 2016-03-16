#ifndef TUTTLE_MATH_HPP
#define TUTTLE_MATH_HPP

namespace tuttle
{

using std::min;
using std::max;

template <typename T>
inline T min(const T& a, const T& b, const T& c)
{
    return min(min(a, b), c);
}

template <typename T>
inline T max(const T& a, const T& b, const T& c)
{
    return max(max(a, b), c);
}

template <typename T>
inline T min(const T& a, const T& b, const T& c, const T& d)
{
    return min(min(a, b), min(c, d));
}

template <typename T>
inline T max(const T& a, const T& b, const T& c, const T& d)
{
    return max(max(a, b), max(c, d));
}
}

#endif
