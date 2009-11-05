/**
 * @brief StaticSingleton class, header file
 * @author Fabien Castan
 * @date 2007/01/30
*/

#ifndef StaticSingleton_HPP
#define StaticSingleton_HPP


#include <cstdlib>


/**
 * @class StaticSingleton <designPattern/StaticSingleton.hpp>
 *
 * @brief StaticSingleton<ClassStaticSingleton> Permet de transformer une classe en StaticSingleton (en heritant de cette classe)
 *
 * @par But du StaticSingleton :
 * Forcer (limiter) la présence d'une seule instance d'une classe
 *
 * @par Utilisation :
 *	La classe T a mettre en StaticSingleton, doit deriver de cette classe StaticSingleton (:public StaticSingleton<T>).
 *	La macro MAKE_StaticSingleton(T) permet de mettre les elements necessaires (notamment les constructeurs).
 */
template <class T>
class StaticSingleton
{
private:
	static T inst; //instance de la classe

	// personne ne peut les utiliser
	StaticSingleton(const StaticSingleton &){}
	StaticSingleton& operator=(const StaticSingleton&){}

protected:
	StaticSingleton(){}
	~StaticSingleton(){}

public:

	/**
	 * @brief return the unique instance of StaticSingleton<T> class
	 * @return T the unique instance of StaticSingleton<T> class
	*/
	static T& instance()
	{
		return inst;
	}
};

template <class T> T StaticSingleton<T>::inst; //declaration de la variable statique


/// macro a placer dans la declaration de la classe
#define MAKE_StaticSingleton(Class) \
	public : \
		friend class StaticSingleton<Class>; \
	private : \
		Class(){} \
		~Class(){}


#endif
