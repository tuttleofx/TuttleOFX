#ifndef StaticSingleton_HPP
#define StaticSingleton_HPP

/**
 * @brief StaticSingleton<ClassStaticSingleton> Can transform a class into static Singleton (by inheriting this class)
 *
 * @par Purpose
 * Forcing (limit) the presence of a single instance of a class
 *
 * @par Usage
 * The class T, to use as static Singleton, must derive from this class (: public StaticSingleton <T>).
 * The macro MAKE_StaticSingleton(T) create necessary elements (including constructors).
 */
template <class T>
class StaticSingleton
{
private:
	static T inst;

private:
	StaticSingleton( const StaticSingleton& ) {}
	StaticSingleton& operator=( const StaticSingleton& ) {}

protected:
	StaticSingleton() {}
	virtual ~StaticSingleton() = 0;

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

template <class T>
T StaticSingleton<T>::inst;
template <class T>
StaticSingleton<T>::~StaticSingleton() {}

///macro to implement StaticSingleton. Use it in derived class declaration
#define MAKE_StaticSingleton( Class ) \
	public: \
		friend class StaticSingleton < Class >; \
	private: \
		Class() {} \
		~Class() {}

#endif
