#ifndef _TUTTLE_HOST_OFX_CORE_HPP_
#define _TUTTLE_HOST_OFX_CORE_HPP_

#include <ofxCore.h>

#include <ostream>

std::ostream& operator<<( std::ostream& os, const OfxPlugin& v );

std::ostream& operator<<( std::ostream& out, const OfxPointI& p );
std::ostream& operator<<( std::ostream& out, const OfxPointD& p );
std::ostream& operator<<( std::ostream& out, const OfxRangeI& r );
std::ostream& operator<<( std::ostream& out, const OfxRangeD& r );
std::ostream& operator<<( std::ostream& out, const OfxRectI& r );
std::ostream& operator<<( std::ostream& out, const OfxRectD& r );

#endif

