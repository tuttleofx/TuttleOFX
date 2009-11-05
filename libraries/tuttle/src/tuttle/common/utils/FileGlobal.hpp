/**
 * @brief    Manipuler un Fichier de facon global, un fichier de log, un log de debug.
 * @author   Fabien Castan
 * @date     2007/02/02
 */

#ifndef _FileGlobal_HPP
#define _FileGlobal_HPP


#include <iostream> //pour la sortie d'erreur
#include <fstream> // pour la sortie dans un fichier
#include <patterns/Singleton.hpp> // pour travailler sur la meme instance (pour la sortie dans un fichier commun)


/**
 * @brief Permet de sortir des donnees dans un meme fichier, depuis n'importe ou.
 *        
 **/
class FileGlobal : public Singleton<FileGlobal>
{
public :
	friend class Singleton<FileGlobal>;
private :
	FileGlobal() : _flux(NULL) { }
	~FileGlobal(){}

public:
	std::ofstream _flux;

public:
	/// @param[in] fileName : nom du fichier a creer
	/// @brief Creation du fichier
	void openFile(std::string fileName)
	{
	#ifndef WINDOWS
		_flux.open( fileName.c_str() );
	#else
		_flux.open( fileName.c_str(), std::ios::binary );
	#endif
		if ( !_flux )
		{
			std::cerr << "impossible de creer le fichier global : " << fileName << std::endl;
		}
	}
	
	/// @brief Fermeture du fichier
	void closeFile(){ _flux.close(); }
};




//______________________________________________________________________________
// Macros pour sortir dans un fichier

/**
 * @def   FILENAME_DEBUG(FILENAME)
 * @param[in] FILENAME : prend une chaine de carateres
 * @brief specifie un nom de fichier, pour l'utilisation de FILE_DEBUG (uniquement en mode debug)
**/
#define FILEGLOBAL_FILENAME(FILENAME) \
	if( FileGlobal::instance()._flux ){ FileGlobal::instance().closeFile();} \
		FileGlobal::instance().openFile( FILENAME );

/**
 * @def   FILE_DEBUG(...)
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief Affiche le texte dans le fichier de debuggage specifie par FILNAME_DEBUG (uniquement en mode debug).
  *        Si aucun fichier n'a ete specifie, il cree le fichier "out.log"
**/
#define FILEGLOBAL(...) \
	if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << __VA_ARGS__ << std::endl;

#define FILEGLOBAL_VAR( a ) \
        if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << #a << ": " << a << std::endl;

#define FILEGLOBAL_VAR2( a, b ) \
        if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << #a << ": " << a <<", "<< #b << ": " << b << std::endl;

#define FILEGLOBAL_VAR3( a, b, c ) \
        if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << #a << ": " << a <<", "<< #b << ": " << b <<", "<< #c << ": " << c << std::endl;

#define FILEGLOBAL_VAR4( a, b, c, d ) \
        if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << #a << ": " << a <<", "<< #b << ": " << b <<", "<< #c << ": " << c <<", "<< #d << ": " << d << std::endl;

/**
 * @def   FILE_INFO_DEBUG(...)
 * @param[in] ... : prend tous les parametres pour lesquel l'operator << est definit
 * @brief Affiche le texte dans le fichier de debuggage specifie par FILNAME_DEBUG (uniquement en mode debug).
  *        Si aucun fichier n'a ete specifie, il cree le fichier "out.log"
**/
#define FILEGLOBAL_INFO(...) \
	if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << INFO << \
	"\t" << __VA_ARGS__ << std::endl;

#define FILEGLOBAL_FUNCTION \
	if( ! FileGlobal::instance()._flux ){ FileGlobal::instance().openFile("out.log");} \
	FileGlobal::instance()._flux << INFO << std::endl;







#endif
