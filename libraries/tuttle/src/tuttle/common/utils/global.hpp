#ifndef __TUTTLE_GLOBAL__
#define __TUTTLE_GLOBAL__

////////////////////////////////////////////////////////////////////////////////
// windows stuff
#include "windows/windows.h"

////////////////////////////////////////////////////////////////////////////////
// Needed for MinGW
#ifndef _MSC_VER
 #define SHGFP_TYPE_CURRENT 0
 #define strcpy_s( dst, dst_length, src ) strcpy( dst, src )
 #define strcat_s( dst, dst_length, src ) strcat( dst, src )
#endif

////////////////////////////////////////////////////////////////////////////////
// Assert that need to be everywhere
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/current_function.hpp>

namespace boost {
template<class T>
class scoped_ptr;
template<class T>
class shared_ptr;
namespace gil {}
}
// some standards things inside the project
namespace tuttle {
using boost::scoped_ptr;
using boost::shared_ptr;
namespace bgil = boost::gil;
}

////////////////////////////////////////////////////////////////////////////////
// Define functions to display infos in the console
#include <iostream>

/**
 * @def   INFOS
 * @brief informations : filename, line number, function name
 **/
#define INFOS  "file: " << __FILE__ << ",  line: " << __LINE__ << std::endl << "function: " << BOOST_CURRENT_FUNCTION

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
#define COUT(... )  std::cout << __VA_ARGS__ << std::endl

#define COUT_VAR( a )  std::cout << # a << ": " << a << std::endl
#define COUT_VAR2( a, b )  std::cout << # a << ": " << a << ", " << # b << ": " << b << std::endl
#define COUT_VAR3( a, b, c )  std::cout << # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << std::endl
#define COUT_VAR4( a, b, c, d )  std::cout << # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << ", " << # d << ": " << d << std::endl

/**
 * @brief terminal information display
 **/
#define COUT_INFOS COUT( INFOS )

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal information display
 **/
#define COUT_WITHINFOS(... )  \
    COUT_DEBUG( INFOS << \
                std::endl << "\t" << __VA_ARGS__ )

#define COUT_WARNING(... )  \
    std::cerr << "Warning:" << \
    std::endl << INFOS << \
    std::endl << "\t" << __VA_ARGS__  << std::endl

#define COUT_ERROR(... )  \
    std::cerr << "Error:" << \
    std::endl << INFOS << \
    std::endl << "\t" << __VA_ARGS__  << std::endl

#define COUT_FATALERROR(... )  \
    std::cerr << "Fatal error:" << \
    std::endl << INFOS << \
    std::endl << "\t" << __VA_ARGS__  << std::endl

#define COUT_EXCEPTION( e )  \
    std::cerr << "Exception:" << \
    std::endl << INFOS << \
    std::endl << "\t" << e.what() << std::endl

////////////////////////////////////////////////////////////////////////////////
// Some specifics things to debug or release version
#ifdef DEBUG
 #include "debug.hpp"
#else
 #include "release.hpp"
#endif

#endif
