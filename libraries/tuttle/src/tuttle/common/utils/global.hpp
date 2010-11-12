#ifndef __TUTTLE_GLOBAL__
#define __TUTTLE_GLOBAL__

//#define TUTTLE_NO_COUT

////////////////////////////////////////////////////////////////////////////////
// windows stuff
#include "windows/windows.h"

////////////////////////////////////////////////////////////////////////////////
// Assert needs to be everywhere
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/current_function.hpp>

////////////////////////////////////////////////////////////////////////////////
// compatibility problems...
#include <cstddef>
namespace std {
#ifdef _MSC_VER
	typedef SSIZE_T ssize_t;
#else
	typedef ::ssize_t ssize_t;
#endif
}


////////////////////////////////////////////////////////////////////////////////
// Define functions to display infos in the console
#include <iostream>

#ifdef _DEBUG
 #    define TUTTLE_FORCEINLINE inline
#else
 #ifdef NDEBUG
  #if   defined( _MSC_VER )
   #    define TUTTLE_FORCEINLINE __forceinline
  #elif defined( __GNUC__ ) && __GNUC__ > 3
   #    define TUTTLE_FORCEINLINE inline __attribute__ ( ( always_inline ) )
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

 #define VAR( a )  # a << ": " << a
 #define VAR2( a, b )  # a << ": " << a << ", " << # b << ": " << b
 #define VAR3( a, b, c )  # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c
 #define VAR4( a, b, c, d )  # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << ", " << # d << ": " << d
 #define VAR_ENDL( a )  # a << ":" << ::std::endl << a

#ifndef TUTTLE_NO_COUT
/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal display
 **/
 #define COUT(... )  ::std::cout << __VA_ARGS__ << ::std::endl
 #define CERR(... )  ::std::cerr << __VA_ARGS__ << ::std::endl

 #define COUT_X( N, ... ) \
    for( unsigned int i = 0; i < N; ++i ) { ::std::cout << __VA_ARGS__; } \
    ::std::cout << ::std::endl

#else
 #define COUT(...)
 #define CERR(...)
 #define COUT_X( N, ... )
#endif


 #define COUT_VAR( a )  COUT( VAR( a ) )
 #define COUT_VAR2( a, b )  COUT( VAR2( a, b ) )
 #define COUT_VAR3( a, b, c )  COUT( VAR3( a, b, c ) )
 #define COUT_VAR4( a, b, c, d )  COUT( VAR4( a, b, c, d ) )

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
    CERR( "Warning:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

 #define COUT_ERROR(... )  \
    CERR( "Error:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

 #define COUT_FATALERROR(... )  \
    CERR( "Fatal error:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

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
#define TCOUT_X COUT_X
#define TCOUT_VAR COUT_VAR
#define TCOUT_VAR2 COUT_VAR2
#define TCOUT_VAR3 COUT_VAR3
#define TCOUT_VAR4 COUT_VAR4
#define TCOUT_INFOS COUT_INFOS
#define TCOUT_WITHINFOS COUT_WITHINFOS
#define TCOUT_EXCEPTION COUT_EXCEPTION

#endif
