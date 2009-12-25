#ifndef _TUTTLE_HOST_OFX_CORE_HPP_
#define	_TUTTLE_HOST_OFX_CORE_HPP_

#include "ofxCore.h"

#include <ostream>


std::ostream& operator<<( std::ostream& os, const OfxPlugin& v );

std::ostream& operator<<( std::ostream& os, const OfxRectI& v );
std::ostream& operator<<( std::ostream& os, const OfxRectD& v );


#endif

