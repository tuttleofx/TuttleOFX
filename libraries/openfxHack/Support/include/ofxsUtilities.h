#ifndef _ofxsUtilities_h_
#define _ofxsUtilities_h_

#include <iostream>

#ifndef OFXS_NO_COUT
/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
 #define OFXS_COUT(... )  ::std::cout << __VA_ARGS__ << ::std::endl
 #define OFXS_CERR(... )  ::std::cerr << __VA_ARGS__ << ::std::endl

 #define OFXS_COUT_X( N, ... ) \
    for( unsigned int i = 0; i < N; ++i ) { ::std::cout << __VA_ARGS__; } \
    ::std::cout << ::std::endl

#else
 #define OFXS_COUT(...)
 #define OFXS_CERR(...)
 #define OFXS_COUT_X( N, ... )
#endif

/**
 * @def   TUTTLE_INFOS
 * @brief informations : filename, line number, function name
 **/
 #define OFXS_INFOS  "file: " << __FILE__ << ",  line: " << __LINE__ << ::std::endl << "function: " << BOOST_CURRENT_FUNCTION


#define OFXS_COUT_WARNING(... )  \
  OFXS_CERR( "Warning:" << \
  ::std::endl << OFXS_INFOS << \
  ::std::endl << "\t" << __VA_ARGS__  )

#define OFXS_COUT_ERROR(... )  \
  OFXS_CERR( "Error:" << \
  ::std::endl << OFXS_INFOS << \
  ::std::endl << "\t" << __VA_ARGS__  )
/*
#include <tuttle/common/utils/global.hpp>

#ifndef OFXS_NO_COUT
#ifdef OFXS_COUT
#undef OFXS_COUT
#endif
#define OFXS_COUT(... ) ::std::cout << "[PLUGIN] " << __VA_ARGS__ << std::endl
#ifdef OFXS_CERR
#undef OFXS_CERR
#endif
#define OFXS_CERR(... ) ::std::cerr << "[PLUGIN] " << __VA_ARGS__ << std::endl
#endif
*/
#endif

