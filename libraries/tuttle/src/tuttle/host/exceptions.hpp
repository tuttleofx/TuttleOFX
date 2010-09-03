#ifndef _TUTTLE_HOST_EXCEPTION_HPP_
#define _TUTTLE_HOST_EXCEPTION_HPP_

#include <tuttle/common/exceptions.hpp>

#define COUT_EXCEPTION( e )  \
    ::std::cerr << "Exception:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << ::boost::diagnostic_information( e ) << ::std::endl

#define COUT_CURRENT_EXCEPTION  \
    ::std::cerr << "Exception:" << \
    ::std::endl << INFOS << \
    ::std::endl << "\t" << ::boost::current_exception_diagnostic_information() << ::std::endl

namespace tuttle {
namespace host {
namespace exception {

using namespace tuttle::exception;

}
}
}

#endif
