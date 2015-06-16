#include "exceptions.hpp"

namespace tuttle {
namespace exception {

std::string format_exception_message( const ::boost::exception& e )
{
	std::ostringstream tmp;
	if( const ::boost::error_info_sstream* m = ::boost::get_error_info<tuttle::exception::user>(e) )
	{
		tmp << *m;
		return tmp.str();
	}
	else if( const ::boost::error_info_sstream* m = boost::get_error_info<tuttle::exception::dev>(e) )
	{
		tmp << *m;
		return tmp.str();
	}
	return std::string();
}

std::string format_exception_info( const ::boost::exception& e )
{
	std::ostringstream tmp;
	if( ::boost::get_error_info<tuttle::exception::user>(e) )
	{
		if( const ::boost::error_info_sstream* m = ::boost::get_error_info<tuttle::exception::dev>(e) )
		{
			tmp << " * Advanced message: " << *m << std::endl;
		}
	}
	if( const ::std::string* m = ::boost::get_error_info<tuttle::exception::pluginIdentifier>(e) )
	{
		tmp << " * Plugin identifier: " << *m << std::endl;
	}
	if( const ::std::string* m = ::boost::get_error_info<tuttle::exception::pluginName>(e) )
	{
		tmp << " * Plugin name: " << *m << std::endl;
	}
	if( const ::std::string* m = ::boost::get_error_info<tuttle::exception::nodeName>(e) )
	{
		tmp << " * Node name: " << *m << std::endl;
	}
	if( const OfxTime* t = ::boost::get_error_info<tuttle::exception::time>(e) )
	{
		tmp << " * Time: " << *t << std::endl;
	}
	tmp << "----------" << std::endl;
	tmp << " * Full exception information:" << std::endl;
	tmp << ::boost::current_exception_diagnostic_information() << std::endl;
#ifdef TUTTLE_DEBUG
	tmp << std::endl;
	tmp << " * Backtrace:" << std::endl;
	tmp << ::boost::trace(e) << std::endl;
#endif
	tmp << "----------" << std::endl;
	return tmp.str();
}

std::string format_current_exception()
{
	std::ostringstream tmp;
	tmp << "----------" << std::endl;
	tmp << " * Full exception information:" << std::endl;
	tmp << ::boost::current_exception_diagnostic_information() << std::endl;
	tmp << "----------" << std::endl;
	return tmp.str();
}

}
}
