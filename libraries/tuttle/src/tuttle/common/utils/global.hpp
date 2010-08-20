#ifndef __TUTTLE_GLOBAL__
#define __TUTTLE_GLOBAL__

////////////////////////////////////////////////////////////////////////////////
// windows stuff
#include "windows/windows.h"

////////////////////////////////////////////////////////////////////////////////
// Assert needs to be everywhere
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/current_function.hpp>

////////////////////////////////////////////////////////////////////////////////
// Define functions to display infos in the console
#include <iostream>


#ifdef _DEBUG
#    define TUTTLE_FORCEINLINE inline
#else
#ifdef NDEBUG
#if   defined(_MSC_VER)
#    define TUTTLE_FORCEINLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#    define TUTTLE_FORCEINLINE inline __attribute__ ((always_inline))
#else
#    define TUTTLE_FORCEINLINE inline
#endif
#else
#    define TUTTLE_FORCEINLINE inline
#endif
#endif


#ifndef COUT

/**
 * @def   INFOS
 * @brief informations : filename, line number, function name
 **/
#define INFOS  "file: " << __FILE__ << ",  line: " << __LINE__ << ::std::endl << "function: " << BOOST_CURRENT_FUNCTION

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
#define COUT(... )  ::std::cout << __VA_ARGS__ << ::std::endl

#define COUT_X( N, ... )  for( unsigned int i = 0; i < N; ++i ){ ::std::cout << __VA_ARGS__; } ::std::cout << __VA_ARGS__ << ::std::endl

#define COUT_VAR( a )  ::std::cout << # a << ": " << a << ::std::endl
#define COUT_VAR2( a, b )  ::std::cout << # a << ": " << a << ", " << # b << ": " << b << ::std::endl
#define COUT_VAR3( a, b, c )  ::std::cout << # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << ::std::endl
#define COUT_VAR4( a, b, c, d )  ::std::cout << # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << ", " << # d << ": " << d << ::std::endl

/**
 * @brief terminal information display
 **/
#define COUT_INFOS COUT( INFOS )

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal information display
 **/
#define COUT_WITHINFOS(... )  \
    COUT( INFOS << \
                ::std::endl << "\t" << __VA_ARGS__ )

#define COUT_WARNING(... )  \
    ::std::cerr << "Warning:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  << ::std::endl

#define COUT_ERROR(... )  \
    ::std::cerr << "Error:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  << ::std::endl

#define COUT_FATALERROR(... )  \
    ::std::cerr << "Fatal error:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  << ::std::endl

#endif

////////////////////////////////////////////////////////////////////////////////
// Some specifics things to debug or release version
#ifdef DEBUG
 #include "debug.hpp"
#else
 #include "release.hpp"
#endif

////////////////////////////////////////////////////////////////////////////////

#define TCOUT COUT
#define TCOUT_VAR COUT_VAR
#define TCOUT_VAR2 COUT_VAR2
#define TCOUT_VAR3 COUT_VAR3
#define TCOUT_VAR4 COUT_VAR4
#define TCOUT_INFOS COUT_INFOS
#define TCOUT_WITHINFOS COUT_WITHINFOS
#define TCOUT_EXCEPTION COUT_EXCEPTION

#endif
