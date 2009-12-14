#ifndef _OFXHCORE_HPP_
#define	_OFXHCORE_HPP_

#include "ofxCore.h"

#include <ostream>

std::ostream& operator<<( std::ostream& os, const OfxPlugin& v );

std::ostream& operator<<( std::ostream& os, const OfxRectI& v );
std::ostream& operator<<( std::ostream& os, const OfxRectD& v );


#endif

