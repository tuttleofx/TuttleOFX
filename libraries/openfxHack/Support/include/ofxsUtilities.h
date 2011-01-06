#ifndef _ofxsUtilities_h_
#define _ofxsUtilities_h_

#include <tuttle/common/utils/global.hpp>

#ifndef TUTTLE_NO_COUT
#ifdef TUTTLE_COUT
#undef TUTTLE_COUT
#endif
#define TUTTLE_COUT(... ) ::std::cout << "[PLUGIN] " << __VA_ARGS__ << std::endl
#ifdef CERR
#undef CERR
#endif
#define CERR(... ) ::std::cerr << "[PLUGIN] " << __VA_ARGS__ << std::endl
#endif

#endif

