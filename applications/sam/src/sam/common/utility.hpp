#ifndef _SAM_UTILITY_HPP_
#define	_SAM_UTILITY_HPP_

#include <boost/algorithm/string/predicate.hpp>

#include <string>

bool string_to_boolean( const std::string& str )
{
	return ( str == "1" || boost::iequals(str, "y") || boost::iequals(str, "yes") || boost::iequals(str, "true") );
}

#endif
