#ifndef _SAM_UTILITY_HPP_
#define	_SAM_UTILITY_HPP_

#include <boost/algorithm/string/predicate.hpp>

#include <string>
#include <iostream>
#include <tuttle/common/utils/global.hpp>
#include <csignal>

bool string_to_boolean( const std::string& str )
{
	return ( str == "1" || boost::iequals(str, "y") || boost::iequals(str, "yes") || boost::iequals(str, "true") );
}

void signal_callback_handler( int signum )
{
	std::ostringstream os;
	os << "Signal "<< strsignal(signum) << "(" << signum << ") was caught.\nTerminate sam.";
	TUTTLE_LOG_WARNING(os.str());
	exit(signum);
}

#endif
