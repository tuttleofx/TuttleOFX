#ifndef _ofxsUtilities_h_
#define _ofxsUtilities_h_

#include <iostream>

#ifndef COUT

/**
 * @def   COUT(...)
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief affichage sur la console uniquement en mode debug
 **/
 #define COUT(... )  std::cout << "[PLUGIN] " << __VA_ARGS__ << std::endl

 #define COUT_VAR( a )  COUT( # a << ": " << a )
 #define COUT_VAR2( a, b )  COUT( # a << ": " << a << ", " << # b << ": " << b )
 #define COUT_VAR3( a, b, c )  COUT( # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c )
 #define COUT_VAR4( a, b, c, d )  COUT( # a << ": " << a << ", " << # b << ": " << b << ", " << # c << ": " << c << ", " << # d << ": " << d )

/**
 * @def   INFOS
 * @brief contient les infos : nom de fichier, numero de ligne, nom de fonction
 **/
 #define INFOS  "file: " << __FILE__ << ",  line: " << __LINE__ << std::endl << "function: " /*<< __PRETTY_FUNCTION__*/

/**
 * @def   COUT_INFOS_DEBUG
 * @brief affichage les infos : nom de fichier, numero de ligne, nom de fonction
 **/
 #define COUT_INFOS COUT( INFOS )

/**
 * @def   COUT_INFOS_DEBUG(...)
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief affichage sur la console uniquement en mode debug (avec des infos : fichier, ligne, fonction)
 **/
 #define COUT_WITHINFOS(... )  \
    COUT( INFOS << \
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

 #define COUT_EXCEPTION(... )  \
    std::cerr << "Exception:" << \
    std::endl << INFOS << \
    std::endl << "\t" << __VA_ARGS__  << std::endl

#endif

#endif

