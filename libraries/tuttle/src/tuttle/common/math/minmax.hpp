/*
 * @author fabien castan
 * @date   2009/09/11
 */

#ifndef TUTTLE_MATH_HPP
#define TUTTLE_MATH_HPP
#include <cmath>
#include <algorithm>

#ifndef _MSC_VER
using std::min;
using std::max;
#else
 #undef min
 #undef max

namespace tuttle {

template<typename T>
inline T min( const T& a, const T& b )
{
	return std::_cpp_min( a, b );
}

template<typename T>
inline T max( const T& a, const T& b )
{
	return std::_cpp_max( a, b );
}

}
#endif

namespace tuttle {

template<typename T>
inline T min( const T& a, const T& b, const T& c )
{
	return min( min( a, b ), c );
}

template<typename T>
inline T max( const T& a, const T& b, const T& c )
{
	return max( max( a, b ), c );
}

template<typename T>
inline T min( const T& a, const T& b, const T& c, const T& d )
{
	return min( min( a, b ), min( c, d ) );
}

template<typename T>
inline T max( const T& a, const T& b, const T& c, const T& d )
{
	return max( max( a, b ), max( c, d ) );
}

}

#endif  /* TUTTLE_MATH_HPP */

