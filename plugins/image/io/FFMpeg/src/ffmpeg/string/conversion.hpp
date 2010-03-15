#ifndef __STRINGCONVERSION_HPP__
#define __STRINGCONVERSION_HPP__


#include <algorithm>
#include <cctype>    // std::toupper
#include <string>

#include <ostream>
#include <sstream>


/// \todo a suppimer utiliser => getStringFrom
inline static std::string intToString ( const int number )
{
	std::ostringstream oss;
	oss<< number;
	return oss.str();
}

template<class TScalar>
inline static std::string getStringFrom ( const TScalar number )
{
	std::ostringstream oss;
	oss<< number;
	return oss.str();
}

template<class TScalar>
inline static std::string getStringWith2CharFrom ( const TScalar number )
{
	std::ostringstream oss;
	if(number < 10) oss << "0";
	oss<< number;
	return oss.str();
}
/*
template<N, TScalar>
inline static std::string getStringWithNCharFrom ( const TScalar number )
{
	std::ostringstream oss;
	for(unsigned int i=à; i<N; i++)
		if(number < 10) oss << number;
	oss<< number;
	return oss.str();
}
*/

//attention... par ici avec les const... a revoir

inline static std::string& toUpper ( std::string& str )
{
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) std::toupper);
	return str;
}


inline static std::string toUpper ( const std::string& str )
{
	std::string tmpStr = str;
	std::transform(tmpStr.begin(), tmpStr.end(), tmpStr.begin(), (int(*)(int)) std::toupper);
	return tmpStr;
}

inline static std::string& toLower ( std::string& str )
{
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) std::tolower);
	return str;
}

inline static std::string toLower ( const std::string& str )
{
	std::string tmpStr = str;
	std::transform(tmpStr.begin(), tmpStr.end(), tmpStr.begin(), (int(*)(int)) std::tolower);
	return tmpStr;
}



#endif // __STRINGCONVERSION_HPP__
