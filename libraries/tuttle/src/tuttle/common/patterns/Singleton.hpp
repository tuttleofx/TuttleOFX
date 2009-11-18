/**
 * @brief Singleton class
 * @author Fabien Castan
 * @date 2007/01/30
 */
#ifndef Singleton_HPP
#define Singleton_HPP

#include <cstdlib>

/**
 * @class Singleton <de/utils/Singleton.hpp>
 *
 * @brief Singleton<ClassSingleton> Permet de transformer une classe en Singleton (en heritant de cette classe)
 *
 * @par But du Singleton :
 * Forcer (limiter) la présence d'une seule instance d'une classe
 *
 * @par Utilisation :
 *	La classe T a mettre en Singleton, doit deriver de cette classe Singleton (:public Singleton<T>).
 *	La macro MAKE_SINGLETON(T) permet de mettre les elements necessaires (notamment les constructeurs).
 */
template <class T>
class Singleton
{
private:
    static T* inst; //instance de la classe

    // personne ne peut les utiliser
    Singleton( const Singleton & ){}
    Singleton& operator=( const Singleton& ){}

protected:
    Singleton( ) { }
    ~Singleton( ) { }

public:
    /**
     * @brief return the unique instance of Singleton<T> class
     * @return T the unique instance of Singleton<T> class
     */
    static T& instance( )
    {
        if( !inst )
            inst = new T;
        return *inst;
    }

    /**
     * @brief destroy the unique instance of Singleton<T> class
     */
    static void destroy( )
    {
        delete inst;
        inst = NULL;
    }
};

template <class T> T* Singleton<T>::inst = NULL; //declaration de la variable statique

///macro to implement singleton. Use it in derivated class declaration
#define MAKE_SINGLETON(Class) \
	public : \
		friend class Singleton<Class>; \
		private : \
        		Class(){} \
			~Class(){}

///macro to implement singleton. Use it in derivated class declaration
#define MAKE_SINGLETON_WITHCONSTRUCTORS(Class) \
	public : \
		friend class Singleton<Class>; \
		private : \
        		Class(); \
			~Class();


#endif
