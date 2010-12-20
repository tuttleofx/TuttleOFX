#ifndef _TUTTLE_COMMON_OFX_UTILITIES_HPP_
#define _TUTTLE_COMMON_OFX_UTILITIES_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <string>
#include <vector>

namespace tuttle {
namespace ofx {

/// class that is a std::vector of std::strings
typedef std::vector<std::string> StringVec;

inline void setStringVecValue( StringVec& sv, const std::string& value, size_t index = 0 )
{
	size_t size = sv.size();

	if( size <= index )
	{
		while( size < index )
		{
			sv.push_back( "" );
			++size;
		}
		sv.push_back( value );
	}
	else
		sv[index] = value;
}

/// get the min value
template<class T>
inline T minimum( const T& a, const T& b )
{
	return a < b ? a : b;
}

/// get the min value
template<class T>
inline T maximum( const T& a, const T& b )
{
	return a > b ? a : b;
}

/// clamp the value
template<class T>
inline T clamp( const T& v, const T& min, const T& max )
{
	if( v < min )
		return min;
	if( v > max )
		return max;
	return v;
}

/// clamp the rect in v to the given bounds
inline OfxRectD clamp( const OfxRectD& v,
                       const OfxRectD& bounds )
{
	OfxRectD r;

	r.x1 = clamp( v.x1, bounds.x1, bounds.x2 );
	r.x2 = clamp( v.x2, bounds.x1, bounds.x2 );
	r.y1 = clamp( v.y1, bounds.y1, bounds.y2 );
	r.y2 = clamp( v.y2, bounds.y1, bounds.y2 );
	return r;
}

/// get the union of the two rects
inline OfxRectD rectUnion( const OfxRectD& a,
                           const OfxRectD& b )
{
	OfxRectD r;

	r.x1 = minimum( a.x1, b.x1 );
	r.x2 = maximum( a.x2, b.x2 );
	r.y1 = minimum( a.y1, b.y1 );
	r.y2 = maximum( a.y2, b.y2 );
	return r;
}

}
}

#endif

