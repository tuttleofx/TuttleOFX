#ifndef _TUTTLE_COMMON_OFX_CORE_HPP_
#define _TUTTLE_COMMON_OFX_CORE_HPP_

#include <ofxCore.h>
#include <ostream>

namespace tuttle {
namespace ofx {

/** @brief maps status to a string */
std::string mapStatusToString( const OfxStatus stat );

}
}

std::ostream& operator<<( std::ostream& os, const OfxPlugin& v );


inline std::ostream& operator<<( std::ostream& out, const OfxPointI& p )
{
	return out << "x:" << p.x << " y:" << p.y;
}

inline std::ostream& operator<<( std::ostream& out, const OfxPointD& p )
{
	return out << "x:" << p.x << " y:" << p.y;
}

inline std::ostream& operator<<( std::ostream& out, const OfxRangeI& r )
{
	return out << "min:" << r.min << " max:" << r.max;
}

inline std::ostream& operator<<( std::ostream& out, const OfxRangeD& r )
{
	return out << "min:" << r.min << " max:" << r.max;
}

inline std::ostream& operator<<( std::ostream& out, const OfxRectI& r )
{
	return out << "x1:" << r.x1 << " y1:" << r.y1 << " x2:" << r.x2 << " y2:" << r.y2;
}

inline std::ostream& operator<<( std::ostream& out, const OfxRectD& r )
{
	return out << "x1:" << r.x1 << " y1:" << r.y1 << " x2:" << r.x2 << " y2:" << r.y2;
}


#endif

