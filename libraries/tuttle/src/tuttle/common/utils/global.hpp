#ifndef _TUTTLE_COMMON_UTILS_GLOBAL_HPP_
#define _TUTTLE_COMMON_UTILS_GLOBAL_HPP_

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

#ifdef NDEBUG
#  if defined( _MSC_VER )
#    define TUTTLE_FORCEINLINE __forceinline
#  elif defined( __GNUC__ ) && __GNUC__ > 3
#    define TUTTLE_FORCEINLINE inline __attribute__ ( ( always_inline ) )
#  else
#    define TUTTLE_FORCEINLINE inline
#  endif
#else
#  define TUTTLE_FORCEINLINE inline
#endif

#ifndef TUTTLE_COUT

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
 #define TUTTLE_COUT(... )  ::std::cout << __VA_ARGS__ << ::std::endl
 #define CERR(... )  ::std::cerr << __VA_ARGS__ << ::std::endl

 #define TUTTLE_COUT_X( N, ... ) \
    for( unsigned int i = 0; i < N; ++i ) { ::std::cout << __VA_ARGS__; } \
    ::std::cout << ::std::endl

#else
 #define TUTTLE_COUT(...)
 #define CERR(...)
 #define TUTTLE_COUT_X( N, ... )
#endif


 #define TUTTLE_COUT_VAR( a )  TUTTLE_COUT( VAR( a ) )
 #define TUTTLE_COUT_VAR2( a, b )  TUTTLE_COUT( VAR2( a, b ) )
 #define TUTTLE_COUT_VAR3( a, b, c )  TUTTLE_COUT( VAR3( a, b, c ) )
 #define TUTTLE_COUT_VAR4( a, b, c, d )  TUTTLE_COUT( VAR4( a, b, c, d ) )

/**
 * @brief terminal information display
 **/
 #define TUTTLE_COUT_INFOS TUTTLE_COUT( INFOS )

/**
 * @param[in] ... : all parameters with an operator << defined
 * @brief terminal information display
 **/
 #define TUTTLE_COUT_WITHINFOS(... )  \
    TUTTLE_COUT( INFOS << \
          ::std::endl << "\t" << __VA_ARGS__ )

 #define TUTTLE_COUT_WARNING(... )  \
    CERR( "Warning:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

 #define TUTTLE_COUT_ERROR(... )  \
    CERR( "Error:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << __VA_ARGS__  )

 #define TUTTLE_COUT_FATALERROR(... )  \
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
// TUTTLE_TCOUT* defines are used by developpers for temporary displays during development stages.
// They are removed in production mode.
#ifndef TUTTLE_PRODUCTION
	#define TUTTLE_TCOUT TUTTLE_COUT
	#define TUTTLE_TCOUT_X TUTTLE_COUT_X
	#define TUTTLE_TCOUT_VAR TUTTLE_COUT_VAR
	#define TUTTLE_TCOUT_VAR2 TUTTLE_COUT_VAR2
	#define TUTTLE_TCOUT_VAR3 TUTTLE_COUT_VAR3
	#define TUTTLE_TCOUT_VAR4 TUTTLE_COUT_VAR4
	#define TUTTLE_TCOUT_INFOS TUTTLE_COUT_INFOS
	#define TUTTLE_TCOUT_WITHINFOS TUTTLE_COUT_WITHINFOS
	#define TUTTLE_TCOUT_EXCEPTION TUTTLE_COUT_EXCEPTION
#else
	#define TUTTLE_TCOUT TUTTLE_COUT_DEBUG
	#define TUTTLE_TCOUT_X TUTTLE_COUT_X_DEBUG
	#define TUTTLE_TCOUT_VAR TUTTLE_COUT_VAR_DEBUG
	#define TUTTLE_TCOUT_VAR2 TUTTLE_COUT_VAR2_DEBUG
	#define TUTTLE_TCOUT_VAR3 TUTTLE_COUT_VAR3_DEBUG
	#define TUTTLE_TCOUT_VAR4 TUTTLE_COUT_VAR4_DEBUG
	#define TUTTLE_TCOUT_INFOS TUTTLE_COUT_INFOS_DEBUG
	#define TUTTLE_TCOUT_WITHINFOS TUTTLE_COUT_WITHINFOS_DEBUG
	#define TUTTLE_TCOUT_EXCEPTION TUTTLE_COUT_EXCEPTION_DEBUG
#endif

#endif
