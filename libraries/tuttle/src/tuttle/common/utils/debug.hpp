#ifndef __TUTTLE_DEBUG__
#define __TUTTLE_DEBUG__

//directives pre-processeur :
//	# : prend ce qui suit comme une chaine de characteres
//	## : concatener (ou devant __VA_ARGS__ pour indiquer qu'il peut etre vide)
//	__FILE__ : nom du fichier
//	__LINE__ : numero de la ligne
//	__FUNCTION__ : nom de la fonction avec sa declaration
//	__PRETTY_FUNCTION__ : juste le nom de la fonction
//	__DATE__ : "Mmm dd yyyy" -> mois, jour, annee
//	__TIME__ : "hh:mm:ss"


//_____________________________________________________________________________
// Macros pour sortir sur la console uniquement en mode debug


/**
 * @def   COUT_DEBUG(...) 
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief affichage sur la console uniquement en mode debug
**/
#define COUT_DEBUG COUT

/**
 * @def   COUT_INFOS_DEBUG
 * @brief affichage les infos : nom de fichier, numero de ligne, nom de fonction
**/
#define COUT_INFOS_DEBUG COUT_INFOS

/**
 * @def   COUT_INFOS_DEBUG(...) 
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief affichage sur la console uniquement en mode debug (avec des infos : fichier, ligne, fonction)
**/
#define COUT_WITHINFOS_DEBUG COUT_WITHINFOS

/**
 * @def   IF_DEBUG(...)
 * @param[in] ... : prend tous les parametres
 * @brief execute le code dans les parentheses de la macro uniquement en mode debug
**/
#define IF_DEBUG(...)  __VA_ARGS__


#ifdef WINDOWS
#include "windows/MemoryLeaks.hpp"
#endif

#endif

