#ifndef __TUTTLE_GLOBAL__
#define __TUTTLE_GLOBAL__

////////////////////////////////////////////////////////////////////////////////
// Needed for windows compilation
#ifndef WINDOWS
    #if ( defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__) || defined(__TOS_WIN__) || defined(__WIN32__) )
        #define WINDOWS
        #include "windows/windows.h"
    #endif
#endif

// Needed for MinGW:
#ifndef _MSC_VER
#define SHGFP_TYPE_CURRENT 0
#define strcpy_s(dst, dst_length, src) strcpy(dst, src)
#define strcat_s(dst, dst_length, src) strcat(dst, src)
#endif

////////////////////////////////////////////////////////////////////////////////
// Assert that need to be everywhere
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/current_function.hpp>

namespace boost {
    template<class T> class scoped_ptr;
    template<class T> class shared_ptr;
    namespace gil {}
}
// Ce que l'on utilise de facon standard sans namespace
namespace tuttle {
    using boost::scoped_ptr;
    using boost::shared_ptr;
    namespace bgil = boost::gil;
}


////////////////////////////////////////////////////////////////////////////////
// Define functions to display infos in the console.
#include <iostream>

// @todo: translate in english.

/**
 * @def   INFOS
 * @brief contient les infos : nom de fichier, numero de ligne, nom de fonction
**/
#define INFOS  "file: " << __FILE__ << ",  line: " << __LINE__ << std::endl << "function: " << BOOST_CURRENT_FUNCTION


/**
 * @def   COUT(...)
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief affichage sur la console uniquement en mode debug
**/
#define COUT(...)  std::cout << __VA_ARGS__ << std::endl

#define COUT_VAR( a )  std::cout << #a << ": " << a << std::endl
#define COUT_VAR2( a, b )  std::cout << #a << ": " << a << ", " << #b << ": " << b << std::endl
#define COUT_VAR3( a, b, c )  std::cout << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << std::endl
#define COUT_VAR4( a, b, c, d )  std::cout << #a << ": " << a << ", " << #b << ": " << b << ", " << #c << ": " << c << ", " << #d << ": " << d << std::endl

/**
 * @def   COUT_INFOS_DEBUG
 * @brief affichage les infos : nom de fichier, numero de ligne, nom de fonction
**/
#define COUT_INFOS COUT(INFOS)

/**
 * @def   COUT_INFOS_DEBUG(...)
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief affichage sur la console uniquement en mode debug (avec des infos : fichier, ligne, fonction)
**/
#define COUT_WITHINFOS(...)  \
	COUT_DEBUG( INFOS << \
	std::endl << "\t" << __VA_ARGS__ )

#define COUT_WARNING(...)  \
	std::cerr << "Warning:" << \
	std::endl << INFOS << \
	std::endl << "\t" << __VA_ARGS__  << std::endl

#define COUT_ERROR(...)  \
	std::cerr << "Error:" << \
	std::endl << INFOS << \
	std::endl << "\t" << __VA_ARGS__  << std::endl

#define COUT_FATALERROR(...)  \
	std::cerr << "Fatal error:" << \
	std::endl << INFOS << \
	std::endl << "\t" << __VA_ARGS__  << std::endl

#define COUT_EXCEPTION(...)  \
	std::cerr << "Exception:" << \
	std::endl << INFOS << \
	std::endl << "\t" << __VA_ARGS__  << std::endl


////////////////////////////////////////////////////////////////////////////////
// Les choses specifiques aux versions release/debug
#ifdef DEBUG
#include "debug.hpp"
#else
#include "release.hpp"
#endif

#endif
